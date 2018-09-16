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

#ifndef LIBMNP_MPLOAD_H
#define	LIBMNP_MPLOAD_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Animáció lokális fájlból betöltését kérő csomag.
 */
class MNP4_EXPORT MPLoad : public MPacket
{
DECLARE_MNP_PACKET(MPLoad);
protected:
    override bool load2(QByteArray);
    override QByteArray save2();
public:
    QString name;
    
    MPLoad(QString _=QString());
};

#endif
