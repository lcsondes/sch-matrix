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

#include "libmnp4/packets/mpstatus.h"
#include "libmnp4/core/mcanimplayer.h"
#include "libmnp4/core/mctime.h"

DEFINE_MNP_PACKET(MPStatus);

MPStatus::MPStatus(Type type/*=INVALID*/)
    :MPacket("STAT"),type(type) //ez itt jó
{
}

MPStatus::MPStatus(AutoResponse)
    :MPacket("STAT"),type(RESPONSE),
     validFrames(MCAnimPlayer::instance()->validFrames()),
     time(MCTime::getTimeMs())
{
}

bool MPStatus::load2(QByteArray data)
{
    if(data.size()<4)return false;
    const uchar* d=(const uchar*)data.constData();
    type=(Type)qFromLittleEndian<quint32>(d);
    if(type==RESPONSE)
    {
        if(data.size()<16)return false; //4+4+8
        validFrames=qFromLittleEndian<qint32>(d+4);
        time=qFromLittleEndian<qint64>(d+8);
    }
    switch(type)
    {
        case REQUEST:
        case RESPONSE:
            return true;
        default:
            return false;
    }
}

QByteArray MPStatus::save2()
{
    if(type!=REQUEST&&type!=RESPONSE)
    {
        qDebug("Wanted to serialize invalid MPStatus!");
        return QByteArray();
    }
    QByteArray retval;
    retval.resize(4);
    qToLittleEndian<quint32>(type,(uchar*)retval.data());
    if(type==RESPONSE)
    {
        retval.resize(16); //+4+8
        qToLittleEndian<qint32>(validFrames,(uchar*)retval.data()+4);
        qToLittleEndian<qint64>(time,(uchar*)retval.data()+8);
    }
    return retval;
}
