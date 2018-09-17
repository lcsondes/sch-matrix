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

#ifndef AUDIOPLAYER_H
#define	AUDIOPLAYER_H

#include "global.h"

class AudioPlayer : public QThread
{
Q_OBJECT

    QMutex mutex_;
    FMOD_SYSTEM* system_;
    FMOD_SOUND* sound_;
    FMOD_CHANNEL* channel_;
    enum State
    {
        IDLE,
        PLAYING,
        PAUSED
    } state_;
    QTimer* tickTimer_;
    qint64 startedAt_;

    void run() override;
    
    void fmod_init();
    AudioPlayer(int tag);
    AudioPlayer(const AudioPlayer&)=delete;
public:
    AudioPlayer();
    ~AudioPlayer();
    qint64 pos();
    static QString tellFormat(QString);
public slots:
    void tick();
    void play(QString,bool paused=false);
    void stop();
    void pauseCont();
    void pause();
    void cont();
    void seek(qint64);
    void seek(int);
};

#endif
