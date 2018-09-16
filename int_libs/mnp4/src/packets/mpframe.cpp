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

#include "libmnp4/packets/mpframe.h"
#include "libmnp4/core/mcconfig.h"

DEFINE_MNP_PACKET(MPFrame);

const QString MPFrame::WIDTH_QUERY("libmnp4.frame.width");
const QString MPFrame::HEIGHT_QUERY("libmnp4.frame.height");
/*const*/ int MPFrame::WIDTH=0;
/*const*/ int MPFrame::HEIGHT=0;

void MPFrame::init()
{
    bool ok;
    WIDTH=MCConfig::query(WIDTH_QUERY).toUInt(&ok);
    if(!ok)
    {
        qCritical()<<"Couldn't get frame width from config";
    }
    HEIGHT=MCConfig::query(HEIGHT_QUERY).toUInt(&ok);
    if(!ok)
    {
        qCritical()<<"Couldn't get frame height from config";
    }
}

void MPFrame::shutdown()
{
}

void MPFrame::ctor()
{
    //lehet, hogy a WIDTH és a HEIGHT 0. Ez van akkor, ha csak kamu objektum
    //jön létre az MCEventDispatcherhez, még a libmnp4_init() előtt.
    if(!WIDTH)
    {
        pixels=NULL;
        return;
    }
    pixels=new quint32*[WIDTH];
    for(int i=0;i<WIDTH;++i)
    {
        pixels[i]=new quint32[HEIGHT];
    }
}

void MPFrame::copy(const MPFrame& o)
{
    if(!WIDTH)return;
    ctor();
    for(int i=0;i<WIDTH;++i)
    {
        memcpy(pixels[i],o.pixels[i],sizeof(quint32)*HEIGHT);
    }
}

void MPFrame::dtor()
{
    if(!pixels)return;
    for(int i=0;i<WIDTH;++i)
    {
        delete[] pixels[i];
    }
    delete[] pixels;
}

MPFrame::MPFrame()
    :MPacket("FREM"),seq(-1) //frém
{
    ctor();
}

MPFrame::MPFrame(BlankIt)
    :MPacket("FREM"),seq(-1),delay(0)
{
    ctor();
    for(int i=0;i<WIDTH;++i) //WIDTH=0: nem fut le a ciklus
    {
        memset(pixels[i],0,sizeof(quint32)*HEIGHT);
    }
}

MPFrame::MPFrame(const MPFrame& o)
    :MPacket("FREM"),seq(o.seq),delay(o.delay)
{
    copy(o);
}

MPFrame::MPFrame(MPFrame&& o)
    :MPacket("FREM"),seq(o.seq),delay(o.delay)
{
    pixels=o.pixels;
    o.pixels=NULL;
}

void MPFrame::operator=(const MPFrame& o)
{
    dtor();
    seq=o.seq;delay=o.delay;
    copy(o);
}

MPFrame::~MPFrame()
{
    dtor();
}

bool MPFrame::load2(QByteArray data)
{
    if(data.size()!=int(sizeof(seq)+sizeof(delay))+WIDTH*HEIGHT*3)
    {
        return false;
    }
    const uchar* p=(const uchar*)data.constData();
    seq=qFromLittleEndian<quint32>(p);
    p+=sizeof(seq);
    delay=qFromLittleEndian<quint32>(p);
    p+=sizeof(delay);
    for(int y=0;y<HEIGHT;++y)
    {
        for(int x=0;x<WIDTH;++x)
        {
            quint8 red=*p++;
            quint8 green=*p++;
            quint8 blue=*p++;
            pixels[x][y]=red<<16|green<<8|blue;
        }
    }
    return isValid();
}

QByteArray MPFrame::save2()
{
    QByteArray retval;
    if(!isValid())return retval;
    retval.resize(sizeof(seq)+sizeof(delay)+WIDTH*HEIGHT*3);
    uchar* p=(uchar*)retval.data();
    qToLittleEndian<quint32>(seq,p);
    p+=sizeof(seq);
    qToLittleEndian<quint32>(delay,p);
    p+=sizeof(delay);
    for(int y=0;y<HEIGHT;++y)
    {
        for(int x=0;x<WIDTH;++x)
        {
            *p++=(pixels[x][y]>>16)&0xFF;
            *p++=(pixels[x][y]>>8)&0xFF;
            *p++=pixels[x][y]&0xFF;
        }
    }
    return retval;
}

bool MPFrame::isValid() const
{
    return seq!=(decltype(seq))(-1);
}
