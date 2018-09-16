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

#include "libmnp4/packets/mpping.h"

DEFINE_MNP_PACKET(MPPing);

MPPing::MPPing(Type t)
    :MPacket("PING"),type(t)
{
}

bool MPPing::load2(QByteArray data)
{
    if(data.size()!=1)
    {
        qWarning("PING packet isn't 1 byte long (%d)!",data.size());
        return false;
    }
    type=(Type)data.at(0);
    if(type&~1)
    {
        qWarning("Bad type in PING packet (%d)!",type);
        return false;
    }
    return true;
}

QByteArray MPPing::save2()
{
    if(type&~1)
    {
        qDebug("Bad type when saving MPPing (%d)!",type);
        return QByteArray();
    }
    return QByteArray(1,(char)type);
}
