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

#include "libmnp4/packets/mpacket.h"
#include "libmnp4/network/mnserver.h"

/*
 * Singletont kellett belőle csinálni a static inicializáció bizonytalansága
 * miatt.
 */
QHash<QByteArray,MPacket*(*)(void)>* MPacket::creators_=NULL;

/**
 * MPacket leszármazott osztály regisztrációját végzi a szerializáláshoz.
 * Ne hívd közvetlenül, használd a *_MNP_PACKET() makrókat.
 * \param id Az osztály 4 bájtos ID-je (pl. PING vagy TIME).
 * \param fn A leszármazott osztály egy példányát létrehozó függvény.
 */
void MPacket::register_subclass(QByteArray id,MPacket*(*fn)(void))
{
    if(!creators_)creators_=new QHash<QByteArray,MPacket*(*)(void)>;
    Q_ASSERT(id.size()==4);
    Q_ASSERT(!creators_->value(id)); //ID-ütközés ellen véd
    creators_->insert(id,fn);
    qDebug("Registered MNP4 packet [%c%c%c%c] to MPacket",
           id.constData()[0],
           id.constData()[1],
           id.constData()[2],
           id.constData()[3]);
}

/**
 * Konstruktor.
 * \param id A leszármazott osztály ID-je (ez az osztály absztrakt).
 */
MPacket::MPacket(const QByteArray& id)
    :id_(id)
{
}

MPacket::~MPacket()
{
}

QByteArray MPacket::id()
{
    return id_;
}

/**
 * Szerializálja ezt az objektumot a megadott QIODevice-ba.
 * \sa save2()
 */
void MPacket::save(QIODevice* dev)
{
    QByteArray data=save2();
    if(data.isNull())
    {
        qWarning("MPacket subclass couldn't serialize!");
        return;
    }
    dev->write("MNP4");
    Q_ASSERT(id().size()==4);
    dev->write(id());
    uchar length[4];
    qToLittleEndian<quint32>(data.size(),length);
    dev->write((const char*)length,4);
    dev->write(data);
}

/**
 * Segédfüggvény, adott mennyiségű adat beérkezésére vár.
 * \param dev A QIODevice, amin várakozunk.
 * \param bytes Ennyi bájtra várakozunk.
 * \param timeout Ennyi idő alatt kell megérkeznie az adatnak.
 * \return true, ha érkezett megfelelő mennyiségő adat, false, ha timeout volt.
 */
static bool waitForBytesAvailable(QIODevice* dev,qint64 bytes,int timeout)
{
    QTime t; //TODO Qt 4.7+ esetén ez legyen QElapsedTimer.
    for(;;)
    {
        if(dev->bytesAvailable()>=bytes)return true;
        t.start();
        if(!dev->waitForReadyRead(timeout))return false;
        timeout-=t.elapsed();
        if(timeout<=0)return false;
    }
    Q_ASSERT(false);
}

/**
 * Deszerializálja a megadott QIODevice-on keresztül érkező MNP4 csomagot.
 * Alacsony timeout-tal (1 mp) dolgozik.
 * \param dev A QIODevice.
 * \return A deszerializált MPacket-leszármazott objektum, vagy NULL hiba
 * esetén.
 */
MPacket* MPacket::load(QIODevice* dev)
{
    static const int TIMEOUT=1000;
    if(!::waitForBytesAvailable(dev,12,TIMEOUT))return NULL; //1 mp legyen elég, realtime progi vagyunk
    char header[12]; //MNP4<id><méret>: 12 bájt.
    dev->read(header,12);
    if(memcmp(header,"MNP4",4))return NULL; //rossz header
    MPacket*(*creator)(void)=creators_->value(QByteArray(header+4,4));
    if(!creator)
    {
        qDebug("Packet type [%s] isn't registered",
                 QByteArray(header+4,4).constData());
        return NULL;
    }
    MPacket* retval=(*creator)();
    quint32 size=qFromLittleEndian<quint32>((unsigned char*)header+8);
    if(!::waitForBytesAvailable(dev,size,TIMEOUT)) //még 1 másodperc a böhömnagy csomagoknak
    {
        delete retval;
        return NULL;
    }
    if(!retval->load2(dev->read(size)))
    {
        delete retval;
        return NULL;
    }
    return retval;
}

/**
 * Készít egy QBuffert, beleszerializálja ezt a csomagot, és elküldi az adott
 * címre UDP-vel.
 * \param A cím.
 */
void MPacket::sendViaUdp(QHostAddress addr)
{
    QUdpSocket sock;
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    save(&buf);
    sock.writeDatagram(buf.buffer(),addr,MNServer::PORT);
}
