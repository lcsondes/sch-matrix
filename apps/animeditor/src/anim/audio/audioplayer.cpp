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

#include "audioplayer.h"
#include "libmnp4/core/mctime.h"

/*
 * Ha a channel_-t akarod használni, a létét if(sound_)-dal ellenőrizd!
 * Ha a sound_ létezik, a channel_ is érvényes, viszont ha a sound_ NULL,
 * a channel_ nem feltétlenül NULL.
 */

/**
 * Elindítja az FMOD-ot.
 */
void AudioPlayer::fmod_init()
{
    FMOD_System_Create(&system_);
    FMOD_System_Init(system_,16,FMOD_INIT_NORMAL,NULL);
}

AudioPlayer::AudioPlayer()
    :mutex_(QMutex::Recursive),sound_(NULL),channel_(NULL),state_(IDLE)
{
    fmod_init();
    moveToThread(this);//parenttel nem lehet
    start();
}

AudioPlayer::AudioPlayer(int)
    :mutex_(QMutex::Recursive),sound_(NULL),channel_(NULL),state_(IDLE)
{
    fmod_init();
}

AudioPlayer::~AudioPlayer()
{
    if(isRunning())
    {
        quit();
        wait();
    }
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

/**
 * Beállít magára egy tizedmásodperces timert, és event loop.
 */
void AudioPlayer::run()
{
    tickTimer_=new QTimer(this);
    connect(tickTimer_,SIGNAL(timeout()),this,SLOT(tick()));
    tickTimer_->start(100);

    exec();
}

/**
 * A hanglejátszás pozíciója, ezredmásodpercben.
 */
qint64 AudioPlayer::pos()
{
    return MCTime::getTimeMs()-startedAt_;
}

/**
 * Megmondja egy fájl tényleges formátumát (kiterjesztéstől függetlenül).
 */
QString AudioPlayer::tellFormat(QString filename)
{
    AudioPlayer ap(0);
    FMOD_System_CreateStream(ap.system_,QFile::encodeName(filename),0,NULL,
                             &ap.sound_);
    FMOD_SOUND_TYPE format;
    FMOD_Sound_GetFormat(ap.sound_,&format,NULL,NULL,NULL);
    switch(format) //szép string és mehet is egyből invalid format errorba
    {
		//nem lehet makrózni, mert nem látja a tr-t a Qt
        default:
        case FMOD_SOUND_TYPE_UNKNOWN:     return tr("Unknown (%1)").arg(format);
        //ide ne menjen tr, mert ezzel ellenőrzi a többi kód a formátumot
        case FMOD_SOUND_TYPE_AIFF:        return    "AIFF";
        case FMOD_SOUND_TYPE_ASF:         return    "ASF";
        case FMOD_SOUND_TYPE_AT3:         return    "AT3";
        case FMOD_SOUND_TYPE_CDDA:        return    "CDDA";
        case FMOD_SOUND_TYPE_DLS:         return    "DLS";
        case FMOD_SOUND_TYPE_FLAC:        return    "FLAC";
        case FMOD_SOUND_TYPE_FSB:         return    "FSB";
        case FMOD_SOUND_TYPE_GCADPCM:     return    "GCADPCM";
        case FMOD_SOUND_TYPE_IT:          return    "IT";
        case FMOD_SOUND_TYPE_MIDI:        return    "MIDI";
        case FMOD_SOUND_TYPE_MOD:         return    "MOD";
        case FMOD_SOUND_TYPE_MPEG:        return    "MPEG";
        case FMOD_SOUND_TYPE_OGGVORBIS:   return    "OGG";
        //case FMOD_SOUND_TYPE_PLAYLIST:  return    "";//default
        case FMOD_SOUND_TYPE_RAW:         return    "RAW";
        case FMOD_SOUND_TYPE_S3M:         return    "S3M";
        case FMOD_SOUND_TYPE_SF2:         return    "SF2";
        //case FMOD_SOUND_TYPE_USER:      return    "";//default
        case FMOD_SOUND_TYPE_WAV:         return    "WAV";
        case FMOD_SOUND_TYPE_XM:          return    "XM";
        case FMOD_SOUND_TYPE_XMA:         return    "XMA";
        case FMOD_SOUND_TYPE_VAG:         return    "VAG";
        //case FMOD_SOUND_TYPE_AUDIOQUEUE:return    "";//default
        case FMOD_SOUND_TYPE_XWMA:        return    "XWMA";
        case FMOD_SOUND_TYPE_BCWAV:       return    "BCWAV";
    }
    //a soundot kitakarítja a dtor
}

/**
 * Tizedmásodpercenként hívódok meg. Frissíti a hanglejátszás gondolt
 * pozícióját az FMOD által szolgáltatott pozícióra (akkor érdekes,
 * ha beakad a hang)
 */
void AudioPlayer::tick()
{
    QMutexLocker lock(&mutex_);
    if(!sound_||state_!=PLAYING)return;
    uint pos=uint(-1);
    FMOD_Channel_GetPosition(channel_,&pos,FMOD_TIMEUNIT_MS);
    if(pos!=uint(-1))
    {
        startedAt_=MCTime::getTimeMs()-pos;
    }
}

/**
 * Betölt egy hangfájlt, opcionálisan le is játssza.
 * \param audio A hangfájl elérési útja és neve
 * \param paused true, ha csak előtöltjük a fájlt
 */
void AudioPlayer::play(QString audio,bool paused/*=false*/)
{
    QMutexLocker lock(&mutex_);
    if(sound_)
    {
        FMOD_Sound_Release(sound_);
        //sound_=NULL; nem kell
    }
    FMOD_System_CreateStream(system_,QFile::encodeName(audio),FMOD_ACCURATETIME,
                             NULL,&sound_);
    FMOD_System_PlaySound(system_,FMOD_CHANNEL_FREE,sound_,paused,&channel_);
    startedAt_=MCTime::getTimeMs();
    state_=paused?PAUSED:PLAYING;
}

/**
 * Leállítja a hanglejátszást, az nem folytatható.
 */
void AudioPlayer::stop()
{
    QMutexLocker lock(&mutex_);
    if(state_!=PLAYING&&state_!=PAUSED)return;
    if(sound_)
    {
        FMOD_Sound_Release(sound_);
        sound_=NULL;
    }
}

/**
 * Vagy szünetelteti, vagy folytatja a lejátszást
 * (ahogy egy play/pause gomb működni szokott)
 */
void AudioPlayer::pauseCont()
{
    QMutexLocker lock(&mutex_);
    switch(state_)
    {
        case PLAYING:pause();
            return;
        case PAUSED:cont();
            return;
        default:return;
    }
}

/**
 * Szünetelteti a lejátszást, de az még folytatható.
 */
void AudioPlayer::pause()
{
    QMutexLocker lock(&mutex_);
    if(state_!=PLAYING)return;
    if(sound_)FMOD_Channel_SetPaused(channel_,true);
    state_=PAUSED;
}

/**
 * Folytatja a lejátszást, ha az szüneteltetve volt.
 */
void AudioPlayer::cont()
{
    QMutexLocker lock(&mutex_);
    if(state_!=PAUSED)return;
    if(sound_)FMOD_Channel_SetPaused(channel_,false);
    state_=PLAYING;
}

/**
 * Elteker a megadott időpontra. Az FMOD csak intet támogat.
 */
void AudioPlayer::seek(qint64 i)
{
    QMutexLocker lock(&mutex_);
    seek(int(i));
}

/**
 * Elteker a megadott időpontra.
 */
void AudioPlayer::seek(int i)
{
    QMutexLocker lock(&mutex_);
    if(state_!=PLAYING)return;
    startedAt_=MCTime::getTimeMs()-i;
    if(sound_)FMOD_Channel_SetPosition(channel_,i,FMOD_TIMEUNIT_MS);
}
