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

#ifndef LIBMNP_MCANIMLOADER_H
#define	LIBMNP_MCANIMLOADER_H

#include "libmnp4/global.h"
#include "libmnp4/init.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Animációt tölt be lokális cache-ből. Singleton.
 * 
 * Az animáció #MPFrame csomagok formájában kerül be az #MCEventDispatcher-be,
 * így nem kell kétféle kódot megírni a streamelt és betöltött animációkhoz,
 * továbbá részleges cache is lehetséges (az első x frame cache-ből, utána stream)
 */
class MCAnimLoader : public QObject
{
    Q_OBJECT
    DECLARE_MNP_INITIALIZABLE(MCAnimLoader);
    
    //kulcsrakész megnyitott QIODevice, amit már csak kiolvasni meg törölni kell
    ownership QIODevice* open_q4x(QString);
    ownership QIODevice* open_qpr(QString);
public:
    static const QString PATH_QUERY;
    
    MCAnimLoader();
public slots:
    void packetReceived(QSharedPointer<const MPacket>,QHostAddress);
};

#endif
