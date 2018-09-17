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

#ifndef LIBMNP_MPTIME_H
#define	LIBMNP_MPTIME_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Időszinkronizációra szolgáló csomag. Az algoritmus leírását lásd az
 * mctime.cpp elején található nagy kommentben.
 */
class MNP4_EXPORT MPTime : public MPacket
{
DECLARE_MNP_PACKET(MPTime);
protected:
    bool load2(QByteArray) override;
    QByteArray save2() override;
public:
    MPTime();
    
    quint8 phase; ///< Az algoritmus fázisa. 1-3 között vehet fel értékeket.
    union
    {
        qint64 field1;
        qint64 clienttime; //phase 1&2
        qint64 delta; //phase 3
    };
    union
    {
        qint64 field2;
        //phase 1: unused
        qint64 servertime; //phase 2
        qint64 rtt; //phase 3
    };
};

#endif
