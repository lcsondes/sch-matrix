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

#include "libmnp4/core/mcanimplayer.h"
#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/packets/mpcontrol.h"
#include "libmnp4/core/mctime.h"

/*
 * Arról, hogy minek kell idő egy csomó parancshoz.
 *
 * A parancsokat jellemzően a kliens küldi, és nála van a hanglejátszás is.
 * Mire ez eljut a szerverig, idő. Szerencsére van egy többé-kevésbé
 * szinkronizált óra, ezért adódik a megoldás:
 * A kliens elküldi a parancsot időbélyeggel, és megkezdi a hang lejátszását.
 * A szerver kicsit később értesül erről, és az animáció elejét "gyorsabban"
 * teszi ki.
 *
 * Tweak: A kliens a lejátszást a jövőben kezdi el, így a szerver
 * még a hang elindulása előtt értesül arról, hogy mikor kell elkezdenie a
 * lejátszást.
 */

DEFINE_MNP_INITIALIZABLE_STRICT(MCAnimPlayer);

MCAnimPlayer::MCAnimPlayer()
    :lock_(QMutex::Recursive),state_(IDLE)
{
    moveToThread(this);
    MCEventDispatcher::instance()->
            connect<MPControl>(this,MCEVENT_SLOT(controlReceived));
    MCEventDispatcher::instance()->
            connect<MPFrame>(this,MCEVENT_SLOT(frameReceived));
    clear();
    start();
}

MCAnimPlayer::~MCAnimPlayer()
{
    quit();
    wait();
}

/**
 * Elindítja a működést, saját szálon 100 Hz-es pontossággal.
 */
void MCAnimPlayer::run()
{
    timer_=new QTimer(this);
    connect(timer_,SIGNAL(timeout()),this,SLOT(tick()));
    timer_->start(10);
    exec();
    delete timer_;
}

/**
 * Tickenként(=10ms) hívódik meg. Megállapítja, hogy ki kell-e tennie
 * képet, és ha igen, kiteszi.
 */
void MCAnimPlayer::tick()
{
    if(state_!=PLAYING)return;

    QMutexLocker locker(&lock_);
    qint64 now=MCTime::getTimeMs();
    while(startedAt_+sumDelays_<=now)
    {
        //new frame, yay!
        if(frames_.size()<=atFrame_+1)return; //elfogytak a képek :(
        MPFrame& nextFrame=frames_[atFrame_+1];
        if(!nextFrame.isValid())
        {
            return; //nincs több valid kép :(
        }
        sumDelays_+=nextFrame.delay;
        atFrame_++;
        /*qDebug()<<"Time is"<<now<<", anim started at"<<startedAt_<<
        ", sumDelays_="<<sumDelays_<<", next frame due in"<<
        startedAt_+sumDelays_-now<<"ms";*/
        qint64 nextframein=startedAt_+sumDelays_-now;
        if(nextframein>=0)
        {
            emit frameAppeared(nextFrame);
            return;
        }
    }
}

/**
 * \return Ennyi érvényes keret van a bufferben az elejétől folyamatosan
 * (az első "lyuk" előtt, ha van ilyen)
 */
qint32 MCAnimPlayer::validFrames()
{
    QMutexLocker locker(&lock_);
    return lastValidFrame_+1;
}

/**
 * Parancs érkezett
 */
void MCAnimPlayer::controlReceived(QSharedPointer<const MPacket> packet,
                                   QHostAddress)
{
    const MPControl* p=packet.staticCast<const MPControl>().data();
    qDebug()<<"Control received: cmd"<<int(p->cmd)<<"time"<<int(p->time)<<
              "data1"<<int(p->data1)<<"data2"<<int(p->data2);
    switch(p->cmd) //ne tegyél ide defaultot és warningol a compiler jól
    {
        case MPControl::BUFFER:
            stop();clear();break;
        case MPControl::PLAY:
            play(p->time);break;
        case MPControl::STOP:
            stop();break;
        case MPControl::PAUSE:
            pause(p->time);break;
        case MPControl::CONTINUE:
            cont(p->time);break;
        case MPControl::SEEK:
            seek(p->time,p->data1,p->data2);break;
        case MPControl::LAST__:; //warningot nyel el
    }
}

/**
 * Keret érkezett
 */
void MCAnimPlayer::frameReceived(QSharedPointer<const MPacket> packet,
                                 QHostAddress)
{
    const MPFrame* p=packet.staticCast<const MPFrame>().data();
    //qDebug()<<"Frame received: seq"<<p->seq<<"delay"<<p->delay;
    if(p->seq==0)
    {
        //immediate
        pause(MCTime::getTimeMs());
        emit frameAppeared(*p);
    }
    else
    {
        QMutexLocker locker(&lock_);
        while(uint(frames_.size())<p->seq)
        {
            frames_.append(MPFrame(MPFrame::BlankIt()));
        }
        Q_ASSERT(p->seq&&p->isValid()); //a load2 szűri az invalidot
        frames_[(p->seq)-1]=*p;
        if(lastValidFrame_==int(p->seq)-2) //jó helyre jött frame
        {
            //nézzük meg, hogy van-e utána, hátha lyukat tömött be	
            for(int i=lastValidFrame_+1;i<frames_.size();++i)
            {
                if(frames_[i].isValid())lastValidFrame_++;
            }
        }
    }
}

/**
 * Kiüríti a buffert. Ne menjen a lejátszás!
 */
void MCAnimPlayer::clear()
{
    QMutexLocker locker(&lock_);
    if(state_!=IDLE)stop(); //úgyis el fogja rontani valaki
    frames_.clear();
    lastValidFrame_=-1;
}

/**
 * Leállítja a lejátszást és fekete képet tesz ki, továbbá "visszatekeri"
 * az animációt.
 */
void MCAnimPlayer::stop()
{
    QMutexLocker locker(&lock_);
    state_=IDLE;
    atFrame_=-1;
    startedAt_=0;
    pausedAt_=0;
    sumDelays_=0;
    emit frameAppeared(MPFrame(MPFrame::BlankIt()));
}

/**
 * Elkezdi az animáció lejátszását az elejétől.
 * \param time A lejátszás elkezdésének időpontja.
 */
void MCAnimPlayer::play(qint64 time)
{
    QMutexLocker locker(&lock_);
    state_=PLAYING;
    atFrame_=-1;
    startedAt_=time;
    pausedAt_=0;
    sumDelays_=0;
}

/**
 * Szünetelteti a lejátszást. A cont() használatával a lejátszás folytatható.
 * \param time A szüneteltetés időpontja.
 */
void MCAnimPlayer::pause(qint64 time)
{
    QMutexLocker locker(&lock_);
    if(state_!=PLAYING)return;
    state_=PAUSED;
    pausedAt_=time; //A cont() fogja ezt felhasználni
}

/**
 * Folytatja a lejátszást. Érdemes pause() után hívni, különben bajod lehet
 * (értsd értelmetlen kimenet lesz).
 * \param time A folytatás időpontja.
 */
void MCAnimPlayer::cont(qint64 time)
{
    QMutexLocker locker(&lock_);
    if(state_!=PAUSED)return;
    //előretekerjük a startedAt_-et
    startedAt_+=time-pausedAt_;
    state_=PLAYING;
}

/**
 * Átteker egy másik időpontra. O(n) időben fut a tárolt frame-ek száma szerint,
 * de így is relatíve gyors.
 * \param time A seek küldésének időpontja.
 * \param seekTo A kívánt időpont (az animációban).
 */
void MCAnimPlayer::seek(qint64 time,qint64 seekTo,qint64 hard)
{
    QMutexLocker locker(&lock_);
    //startedAt_-et át kell állítani
    startedAt_=time-seekTo; //time-kor lesz seekTo-ban
    if(hard&~1LL)qWarning("Invalid data2 in seek command!");
    if(hard)
    {
        //visszavisszük 0-ba, a tick() majd a megfelelő frame-et teszi ki
        sumDelays_=0;
        atFrame_=-1;
    }
}
