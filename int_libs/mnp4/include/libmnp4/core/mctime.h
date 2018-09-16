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

#ifndef LIBMNP_MCTIME_H
#define	LIBMNP_MCTIME_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"
#include "libmnp4/init.h"

/**
 * 1000 Hz-es órát és időszinkronizációt nyújtó singleton osztály. Normál használata
 * esetén a szerver órája igazodik a klienséhez. Ez a folyamat általában 1,
 * de maximum 3 másodperc alatt lezajlik (nem garantált). A getTimeMs()
 * függvénye a szinkronizált óra aktuális értékét adja vissza. Ez a számláló
 * a kliensnél 0-tól indul, ezért többé-kevésbé biztonságosan használható 32
 * bites egészként is.
 * 32 biten kb. 24 napig, 64 biten kb. 290 millió évig bírja overflow nélkül.
 */
class MCTime : public QThread
{
Q_OBJECT
DECLARE_MNP_INITIALIZABLE(MCTime);

    QReadWriteLock lock_; ///< A delta_-t és a measurements_-et védő lock.
    qint64 delta_; ///< Ennyi időkorrekció megy rá a core timerre.
    QTimer* sync_tick_; ///< A folyamatos szinkronizációt vezérlő timer.

    static const int MEASUREMENT_COUNT=11; ///< Ennyi mérést várunk be

    /**
     * Mérési eredményt tároló struct.
     */
    struct Measurement
    {
        qint64 delta;
        qint64 rtt;
    } measurements_[MEASUREMENT_COUNT]; ///< A mérési eredményeket tároló tömb.
    int meas_ptr_; ///< A beérkezett mérések számát tárolja.

    MCTime();
    ~MCTime();
    override void run();

    /**
     * Platformfüggő implementációja van. A feladata, hogy egy
     * ezredmásodpercenként eggyel növekvő 64 bites értéket állítson elő.
     */
    qint64 getTimeMsCore();
    void evaluateMeasurements();
private slots:
    void packetReceived(QSharedPointer<const MPacket>,QHostAddress);
    void startSyncing2();
    void syncTick();
public:
    static void startSyncing();
    static qint64 getTimeMs();
};

#endif
