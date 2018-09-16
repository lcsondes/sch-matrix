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

#ifndef LIBMNP_MCEVENTDISPATCHER_H
#define	LIBMNP_MCEVENTDISPATCHER_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"
#include "libmnp4/core/mcemitter.h"

/**
 * A beérkező csomagokat válogatja szét és juttatja el a megfelelő helyre.
 * A connect()-tel lehet egy adott csomagtípust slothoz kötni.
 *
 * Singleton, de nem a szokásos makrókkal készült, mert nincs kinek init()-et
 * hívni rajta (a main() előtt már fut)
 */
class MNP4_EXPORT MCEventDispatcher : public QObject
{
Q_OBJECT
    static MCEventDispatcher* instance_; ///< Az osztály egyetlen példánya
    MCEventDispatcher();
    ~MCEventDispatcher();

    friend MNP4_EXPORT void libmnp_shutdown();
    static void shutdown();

    QHash<QByteArray,MCEmitter*> emitters_; ///< A signalokat küldő MCEmitterek
public:
    static MCEventDispatcher* instance();
    void dispatch(ownership MPacket*,const QHostAddress&);
    template<typename T> void connect(QObject*,const char*,
                                      Qt::ConnectionType _=Qt::AutoConnection);
};

#define MCEVENT_SLOT(name) SLOT(name(QSharedPointer<const MPacket>,QHostAddress))

#include "libmnp4/core/mceventdispatcher_t.cpp"

#endif
