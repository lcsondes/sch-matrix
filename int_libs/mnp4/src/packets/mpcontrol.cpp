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

#include "libmnp4/packets/mpcontrol.h"
#include "libmnp4/core/mctime.h"

DEFINE_MNP_PACKET(MPControl);

MPControl::MPControl()
    :MPacket("CTRL"),cmd(LAST__),data1(0),data2(0)
{
}

MPControl::MPControl(Command cmd)
    :MPacket("CTRL"),cmd(cmd),time(MCTime::getTimeMs()),data1(0),data2(0)
{
}

bool MPControl::load2(QByteArray d)
{
    if(d.size()!=4+8+8+8)
    {
        return false;
    }
    const uchar* dd=(const uchar*)d.constData();
    cmd=(Command)qFromLittleEndian<quint32>(dd);
    if(quint32(cmd)>=LAST__) //nem kell <0 esettel foglalkozni így
    {
        return false;
    }
    time=qFromLittleEndian<qint64>(dd+4);
    data1=qFromLittleEndian<qint64>(dd+4+8);
    data2=qFromLittleEndian<qint64>(dd+4+8+8);
    return true;
}

QByteArray MPControl::save2()
{
    QByteArray retval;
    retval.resize(4+8+8+8);
    uchar* d=(uchar*)retval.data();
    if(quint32(cmd)>=LAST__)
    {
        qDebug("Invalid command when serializing MPControl");
        return QByteArray();
    }
    qToLittleEndian<quint32>(cmd,d);
    qToLittleEndian<qint64>(time,d+4);
    qToLittleEndian<qint64>(data1,d+4+8);
    qToLittleEndian<qint64>(data2,d+4+8+8);
    return retval;
}
