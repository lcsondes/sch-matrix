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

#include "libmnp4/packets/mptime.h"

DEFINE_MNP_PACKET(MPTime);

MPTime::MPTime()
    :MPacket("TIME"),phase(0),field1(0),field2(0)
{
}

bool MPTime::load2(QByteArray data)
{
    if(data.size()!=17) //1 bájt phase és 2x8 data
    {
        qDebug("TIME packet with wrong length (%d) received!",data.size());
        return false;
    }
    phase=data[0];
    if(phase<1||phase>3)
    {
        qDebug("TIME packet with wrong phase (%d) received!",phase);
        return false;
    }
    field1=qFromLittleEndian<qint64>((const uchar*)data.constData()+1);
    field2=qFromLittleEndian<qint64>((const uchar*)data.constData()+9);
    return true;
}

QByteArray MPTime::save2()
{
    if(phase<1||phase>3)
    {
        qDebug("Bad phase when saving MPTime!");
        return QByteArray();
    }
    QByteArray retval;
    retval.resize(17);

    uchar* data=(uchar*)retval.data();
    *data=phase;
    qToLittleEndian<qint64>(field1,data+1);
    qToLittleEndian<qint64>(field2,data+9);
    
    return retval;
}
