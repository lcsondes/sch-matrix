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

#ifndef PLAYCONTROLLER_H
#define	PLAYCONTROLLER_H

#include "global.h"
#include "timeline.h"
#include "transmitter.h"
#include "libmnp4/packets/mpframe.h"
#include "libmnp4/packets/mpstatus.h"

/**
 * A kép- és hanglejátszás közös vezérlését végzi.
 */
class Player : public QThread
{
Q_OBJECT
    FMOD_SYSTEM* system_;
    FMOD_SOUND* sound_;
    FMOD_CHANNEL* channel_;
    QTimer* tickTimer_;
    QByteArray currentAudio_;
    QList<MPFrame> currentAnim_;
    qint64 lastUpdated_;
    qint64 currentLengthMs_;
    qint64 startedAt_;
    qint64 pausedAt_;
    enum State
    {
        //transmitting lehet egyszerre a többivel
        IDLE=0,
        PRE_PLAYING,
        PLAYING,
        PAUSED
    } state_;
    Transmitter* transmitter_;
    MPStatus lastStatus_;
private:
    override void run();
    
    void startPlaying();
    void playTick(bool hard=false);
    void sendTick();
signals: //to timeline
    void setStatus(QString);
    void setRange(qint64);
    void setPos(qint64);
public:
    Player();
    ~Player();
private slots:
    void statusReceived(MPStatus,QHostAddress);
    void tick();
public slots:
    void play(QString,quint32);
    void stop();
    void seek(qint64);
    void pauseCont();
    void pause();
    void cont();
signals:
    void animFinished();
};

#endif
