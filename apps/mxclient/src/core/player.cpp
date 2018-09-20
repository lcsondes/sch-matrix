/*
 *  This file is part of SCH Matrix.
 *
 *  SCH Matrix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SCH Matrix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SCH Matrix. If not, see <https://www.gnu.org/licenses/>.
 */

#include "player.h"

#include <QtIOCompressor>

#include "libmnp4/packets/mpcontrol.h"
#include "libmnp4/packets/mpload.h"
#include "libmnp4/core/mctime.h"
#include "libmnp4/core/mcreplier.h"
#include "libmnp4/network/mnclient.h"

/*
 * Ha a channel_-t akarod használni, a létét if(sound_)-dal ellenőrizd!
 * Ha a sound_ létezik, a channel_ is érvényes, viszont ha a sound_ NULL,
 * a channel_ nem feltétlenül NULL.
 */

Player::Player()
    :sound_(NULL),channel_(NULL),state_(IDLE),transmitter_(NULL)
{
    moveToThread(this); //parenttel nem lehet
    FMOD_System_Create(&system_);
    FMOD_System_Init(system_,16,FMOD_INIT_NORMAL,NULL);
}

Player::~Player()
{
    quit();
    wait();
    //a channelt a system törli
    if(sound_)
    {
        FMOD_Sound_Release(sound_);
    }
    if(system_)
    {
        FMOD_System_Release(system_);
    }
}

void Player::run()
{
    connect(MCReplier::instance(),SIGNAL(statusResponse(MPStatus,QHostAddress)),
            this,SLOT(statusReceived(MPStatus,QHostAddress)));

    tickTimer_=new QTimer(this);
    connect(tickTimer_,SIGNAL(timeout()),this,SLOT(tick()));
    tickTimer_->start(100); //TODO tweak value
    
    exec();
}

void Player::tick()
{
    if(transmitter_)
    {
        sendTick();
    }
    switch(state_)
    {
        case IDLE:
        case PAUSED:
        case PRE_PLAYING:
            return;
        case PLAYING:
            playTick();return;
        default:
            qWarning()<<"WTF?";
    }
}

/*
 * A seek() hívhatja true-val, hogy el is tekerjen az adott helyre.
 */
void Player::playTick(bool hard/*=false*/)
{
    FMOD_System_Update(system_);
    qint64 now=MCTime::getTimeMs();
    if(sound_)
    {
        FMOD_BOOL temp;
        FMOD_Channel_IsPlaying(channel_,&temp);
        if(!temp)
        {
            FMOD_Sound_Release(sound_);
            sound_=NULL;
            goto skip_getpos;
        }
        //ha véget ért a lejátszás time==0
        unsigned int time;
        FMOD_Channel_GetPosition(channel_,&time,FMOD_TIMEUNIT_MS);
        startedAt_=now-time;
skip_getpos:;
    }
    if(now-startedAt_>=currentLengthMs_)
    {
        //vége
        stop();
        emit animFinished();
        return;
    }

    MPControl ctrl;
    ctrl.cmd=MPControl::SEEK;
    ctrl.time=now;
    ctrl.data1=now-startedAt_; //now-(now-pos)==pos
    ctrl.data2=hard;
    MNClient::globalInstance()->send(new MPControl(ctrl),MNClient::UDP);
    emit setPos(now-startedAt_);
}

void Player::sendTick()
{
    MNClient::globalInstance()->sendOneUdp(new MPStatus(MPStatus::REQUEST));
    
    // Esetleg kész van?
    if(lastStatus_.validFrames==currentAnim_.size())
    {
        //minden kép elment, yay!
        delete transmitter_;
        transmitter_=NULL;
        emit setStatus(tr("All frames sent"));
    }

    //Van elég frame már a kezdéshez? (25%+1, hogy az 1 frame-es anim is elinduljon)
    //TODO dinamikusan megállapított prebuffer-méret. Vagy nagyon gyors
    //a hálózat, vagy pocsék (akkor viszont a 25% kevés lesz)
    if(state_==PRE_PLAYING&&lastStatus_.validFrames>=currentAnim_.size()/4+1)
    {
        //elment elég kép, kezdhetjük
        startPlaying();
    }
}

void Player::statusReceived(MPStatus packet,QHostAddress)
{
    if(lastStatus_.validFrames<packet.validFrames)
    {
        //megérkezett legalább 1 új frame
        lastUpdated_=packet.time;
    }
    //státuszfrissítés
    lastStatus_=packet;
    if(transmitter_)
    {
        emit setStatus(tr("%n/%1 frames (%2%) already at server",NULL,
                       packet.validFrames).arg(currentAnim_.size())
                       .arg(100*packet.validFrames/currentAnim_.size()));
    }
}

void Player::startPlaying()
{
    static const int WAIT=1000; //ennyivel a lejátszás előtt szólunk a szervernek

    qint64 now=MCTime::getTimeMs();

    Q_ASSERT(!sound_); //a play() stop()-ot hív
    if(!currentAudio_.isEmpty())
    {
        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo,0,sizeof(exinfo));
        exinfo.cbsize=sizeof(exinfo);
        exinfo.length=currentAudio_.size();
        FMOD_System_CreateStream(system_,
                                 currentAudio_.constData(),
                                 FMOD_ACCURATETIME|FMOD_OPENMEMORY_POINT|
                                     FMOD_CREATECOMPRESSEDSAMPLE|FMOD_SOFTWARE,
                                 &exinfo,&sound_);
        FMOD_System_PlaySound(system_,FMOD_CHANNEL_FREE,sound_,true,&channel_);
        unsigned int temp=0;
        FMOD_Sound_GetLength(sound_,&temp,FMOD_TIMEUNIT_MS);
        if(temp>currentLengthMs_)
        {
            currentLengthMs_=temp;
        }
    }

    emit setStatus(tr("Starting..."));
    emit setRange(currentLengthMs_);

    MPControl play;
    play.cmd=MPControl::PLAY;
    play.time=now+WAIT; //a jövőben kezdjük a lejátszást
    MNClient::globalInstance()->sendOneTcp(new MPControl(play));

    msleep(WAIT);
    
    startedAt_=MCTime::getTimeMs();
    if(sound_)
    {
        FMOD_Channel_SetPaused(channel_,false);
    }
    state_=PLAYING;
}

void Player::play(QString q4x,quint32 length)
{
    if(q4x.isEmpty())return;
    stop();
    
    //ha idáig eljutott egy fájl, már ellenőrizve van és valid
    //különben a playlistbe sem kerül be
    QFile f(q4x);
    f.open(QIODevice::ReadOnly);
    f.read(8); //Q4X2wwhh
    quint32 skip;
    f.read((char*)&skip,4);
    skip=qFromBigEndian<quint32>(skip);
    f.seek(f.pos()+skip);
    quint32 sound_offset;
    f.read((char*)&sound_offset,4);
    sound_offset=f.pos()+qFromBigEndian<quint32>(sound_offset);
    QtIOCompressor comp(&f);
    comp.open(QIODevice::ReadOnly);
    
    qDebug()<<comp.readLine();//qpr v1
    comp.readLine();//cím
    comp.readLine();//audio - unused
    comp.readLine();//length, a playlist már kiszámolta a hangból is
    currentLengthMs_=length;

    //animáció előreolvasása
    currentAnim_.clear();
    //szerveren is törlünk
    MNClient::globalInstance()->sendOneTcp(new MPControl(MPControl::BUFFER));
    int seq=1;
    for(;;)
    {
        emit setStatus(tr("Loading frame #%n",NULL,seq));
        MPFrame frame;
        frame.seq=seq++;
        for(int y=0;y<MPFrame::HEIGHT;++y)
        {
            for(int x=0;x<MPFrame::WIDTH;++x)
            {
                quint8 px[3];
                comp.read((char*)px,3);
                //az utolsó szám után lehet még whitespace
                if(comp.atEnd())
                {
                    goto readDone;
                }
                frame.pixels[x][y]=px[0]<<16|px[1]<<8|px[2];
            }
        }
        quint32 delay;
        comp.read((char*)&delay,4);
        delay=qFromBigEndian<quint32>(delay);
        frame.delay=delay;
        currentAnim_.append(frame);
    }
readDone:;
    //Qt bug (4.7.4): a sound_offset seek néha rossz helyre megy. Resync.
    comp.close();
    f.close();
    f.open(QIODevice::ReadOnly);
    
    f.seek(sound_offset);
    quint32 sound_size;
    f.read((char*)&sound_size,4);
    sound_size=qFromBigEndian<quint32>(sound_size);

    if(sound_size) //van hang
    {
        currentAudio_=f.read(sound_size);
    }
    else
    {
        currentAudio_.clear();
    }
    emit setStatus(tr("Sending frames..."));
    lastStatus_.validFrames=0;
    state_=PRE_PLAYING;
    //bepróbálkozunk a szerver-cache-nél
    MNClient::globalInstance()->sendOneTcp
            (new MPLoad(QFileInfo(q4x).fileName().replace(QRegExp("\\.q4x$"),"")));
    //de azért az átvitel is mehet, max potyára
    delete transmitter_;
    transmitter_=new Transmitter(currentAnim_);
    MNClient::globalInstance()->sendOneTcp(new MPStatus(MPStatus::REQUEST));
}

void Player::pauseCont()
{
    switch(state_)
    {
        case PLAYING:pause();return;
        case PAUSED:cont();return;
        default:return;
    }
}

void Player::pause()
{
    if(state_!=PLAYING)return;
    if(sound_)FMOD_Channel_SetPaused(channel_,true);
    pausedAt_=MCTime::getTimeMs();
    state_=PAUSED;
    MNClient::globalInstance()->sendOneTcp(new MPControl(MPControl::PAUSE));
}


void Player::cont()
{
    if(state_!=PAUSED)return;
    if(sound_)FMOD_Channel_SetPaused(channel_,false);
    startedAt_+=MCTime::getTimeMs()-pausedAt_;
    state_=PLAYING;
    MNClient::globalInstance()->sendOneTcp(new MPControl(MPControl::CONTINUE));
}

void Player::stop()
{
    if(sound_)
    {
        FMOD_Channel_Stop(channel_);
        FMOD_Sound_Release(sound_);
        sound_=NULL;
    }
    state_=IDLE;
    delete transmitter_;
    transmitter_=NULL;
    lastUpdated_=0;
    MNClient::globalInstance()->sendOneTcp(new MPControl(MPControl::STOP));
    emit setRange(0);
    emit setStatus(tr("Idle"));
}

void Player::seek(qint64 time)
{
    if(state_!=PLAYING&&state_!=PAUSED)return;
    if(sound_)FMOD_Channel_SetPosition(channel_,(unsigned int)time,
                                       FMOD_TIMEUNIT_MS);
    startedAt_=MCTime::getTimeMs()-time;
    playTick(true); //ez majd jól küld seeket
}
