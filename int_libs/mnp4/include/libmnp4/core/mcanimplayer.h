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

#ifndef LIBMNP_MCANIMPLAYER_H
#define	LIBMNP_MCANIMPLAYER_H

#include "libmnp4/global.h"
#include "libmnp4/init.h"
#include "libmnp4/packets/mpframe.h"

/**
 * Animációlejátszásért felelős singleton osztály.
 * Ez a "stream mód", azaz előre bufferelt animációk lejátszása,
 * lehetőleg tökéletes A-V sync-kel.
 *
 * Egyetlen animáció tárolására képes, ha többet szeretnél, használd pl.
 * az #MCAnimLoader-t gyors váltáshoz.
 */
class MCAnimPlayer : public QThread
{
    Q_OBJECT
    DECLARE_MNP_INITIALIZABLE(MCAnimPlayer);
    MCAnimPlayer();
    ~MCAnimPlayer();

    QTimer* timer_;

    //ezek szálak közt is használva vannak, erre van a lock_. XXX volatile?
    QMutex lock_;
    enum State
    {
        IDLE,
        PLAYING,
        PAUSED
    } state_; ///< A lejátszás állapotát jelzi
    QList<MPFrame> frames_; ///< A képkockákat tárolja
    /// Az első képkocka után ennyi képkocka valid. -1 ha nincs valid képkocka
    qint32 lastValidFrame_;
    qint32 atFrame_; ///< Azt mutatja, hanyadik képnél járunk
    qint64 startedAt_; ///< A lejátszás kezdetének időpontja
    qint64 pausedAt_; ///< A lejátszás szüneteltésének időpontja
    qint64 sumDelays_; ///< Ennyi idővel a startedAt_ után jön a következő kép

    override void run();
protected slots:
    void tick();
public:
    qint32 validFrames(); ///< ennyi képkocka valid az elejétől kezdve
    qint64 pos(); ///<ennyi idő telt el az anim kezdete óta. Negatív: idle
public slots:
    //érdekel, minek idő néhány slothoz? Az mcanimplayer.cpp elején a válasz!
    void clear();
    void stop ();
    void play (qint64);
    void pause(qint64);
    void cont (qint64); //a continue egy C++ kulcsszó
    void seek (qint64,qint64,qint64);
    void controlReceived(QSharedPointer<const MPacket>,QHostAddress);
    void frameReceived(QSharedPointer<const MPacket>,QHostAddress);
signals:
    void frameAppeared(MPFrame);
};

#endif
