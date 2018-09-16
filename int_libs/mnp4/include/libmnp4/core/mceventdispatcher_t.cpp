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

#ifndef LIBMNP4_MCEVENTDISPATCHER_CPP
#define LIBMNP4_MCEVENTDISPATCHER_CPP
#include "libmnp4/core/mceventdispatcher.h"

//template, ezért külön .cpp fájlban lakik, mert #include-olva lesz.

/**
 * Beköti a megadott slotot az adott csomagtípushoz.
 * \param T A csomag típusa
 * \param tgt A célobjektum, mint a QObject::connect-nél
 * \param slot A célslot, mint a QObject::connect-nél. Az MCEVENT_SLOT makró
 * használatával nem kell kiírni a függvény paramétereit.
 */
template<typename T>
void MCEventDispatcher::connect(QObject* tgt,const char* slot,
                                Qt::ConnectionType ctype)
{
    MPacket* packet=new T; //ez jól megfogja a szar T-ket
    MCEmitter*& emitter=emitters_[packet->id()];
    if(!emitter)
    {
        qDebug("Registered MNP4 packet [%c%c%c%c] to MCEventDispatcher",
               DEBUG_MPACKET_cccc(*packet));
        emitter=new MCEmitter;
    }
    QObject::connect(emitter,SIGNAL(newPacket(QSharedPointer<const MPacket>,
                                              QHostAddress)),tgt,slot,ctype);
    qDebug("MCEventDispatcher: connected %s using %p(%c%c%c%c)",
           tgt->metaObject()->className(),emitter,DEBUG_MPACKET_cccc(*packet));
    delete packet;
}

#endif
