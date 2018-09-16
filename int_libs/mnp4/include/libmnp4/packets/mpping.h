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

#ifndef LIBMNP_MPPING_H
#define	LIBMNP_MPPING_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Életjel küldésére szolgáló csomag.
 */
class MNP4_EXPORT MPPing : public MPacket
{
DECLARE_MNP_PACKET(MPPing);

protected:
    override bool load2(QByteArray);
    override QByteArray save2();
public:
    ///A csomag típusa.
    enum Type{REQUEST=0,RESPONSE=1,INVALID} type; //1 bájtként szerializálódik

    MPPing(Type _=INVALID);
};

#endif
