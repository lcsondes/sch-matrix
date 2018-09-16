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

#include "libmnp4/network/mnclient.h"
#include "libmnp4/network/mnserver.h"
#include "libmnp4/packets/mpacket.h"

/*
 * Az inicializációról
 * 
 * Fontos, hogy az MNClient előbb várjon a waitconditionre, minthogy meghívódna
 * a send(). A start() relatíve lassú, ezért nagyon könnyű idő előtt behívni
 * a send()-be. Ezt akadályozza meg az MNClient inicializációs folyamata.
 *
 * Az inicializáció működése:
 *
 * A konstruktor elindítja a szálat, majd várakozik a QUdpSocket elkészültére
 * (azaz a QMutex zárására). Nem használ igazi lockot, mert pazarlás lenne a
 * későbbiekben.
 * Note: A várakozás kezdetben tiszta spinwait volt, de később kiderült, hogy
 * a yieldelés gyorsítja a folyamatot.
 * A run() ELŐBB zárja a mutexet, AZTÁN készíti el a socketet, majd a
 * waitconditionre vár (ami elengedi a mutexet).
 * A konstruktor a QUdpSocket elkészülte után megpróbálja a mutexet zárni, ez
 * csak akkor sikerül neki, ha a run() már a waitconditionre vár.
 * A mutexet zárása után egyből elengedi.
 */

MNClient* MNClient::globalInstance_=NULL;

/**
 * Konstruktor.
 * \param tgt A szerver címe.
 */
MNClient::MNClient(const QHostAddress& tgt)
    :tgt_(tgt),tcp_(NULL),udp_(NULL)
{
    moveToThread(this);
    //Fontos az események sorrendje. Lásd nagy komment a fájl elején.
    start();
    while(!initHelper_)yieldCurrentThread(); //ez vár az udp_-re
    QMutexLocker locker(&jobs_lock_); //ez vár a mutex nyitására (waitcondition)
    qDebug("MNClient initialized");
}

MNClient::~MNClient()
{
    send(NULL,QUIT); //kilépési parancs beküldése
    wait(); //megvárjuk, amíg elér odáig
    //amit utána küldtek, már nem érdekes
    for(auto i=jobs_.begin();i!=jobs_.end();++i)
    {
        delete i->packet;
    }
}

void MNClient::init(const QHostAddress& tgt)
{
    Q_ASSERT(!globalInstance_);
    globalInstance_=new MNClient(tgt);
}

void MNClient::shutdown()
{
    delete globalInstance_;
}

MNClient* MNClient::globalInstance()
{
    Q_ASSERT(globalInstance_);
    return globalInstance_;
}

/**
 * Elküldi az adott csomagot a megadott módon.
 * \param packet A csomag. Törléséről az MNClient gondoskodik (ha a type nem
 * TCP_RECONNECT).
 * \param type A küldés módja. Lásd #JobType.
 * \sa JobType, sendOneTcp(), sendOneUdp()
 */
void MNClient::send(MPacket* packet,JobType type)
{
    {
        QMutexLocker locker(&jobs_lock_);
        jobs_.append({packet,type});
    }
    jobs_wc_.wakeOne();
}

/**
 * Kényelmi függvény, ekvivalens egy TCP küldéssel és egy utána meghívott
 * tcpReconnect()-tel.
 */
void MNClient::sendOneTcp(MPacket* packet)
{
    send(packet,TCP);
    tcpReconnect();
}

/**
 * Kényelmi függvény, ekvivalens egy UDP küldéssel.
 */
void MNClient::sendOneUdp(MPacket* packet)
{
    send(packet,UDP);
}

/**
 * Kényelmi függvény, ekvivalens egy send(NULL,TCP_RECONNECT) hívással.
 * \sa send
 */
void MNClient::tcpReconnect()
{
    send(NULL,TCP_RECONNECT);
}

/**
 * A beérkezett csomagok folyamatos kiküldését végzi - külön szálon.
 * Ha érkezik egy QUIT JobType, kilép. Ilyet a destruktor küld be.
 */
void MNClient::run()
{
    //fontos, hogy előbb legyen zárva a mutex, aztán készüljön el az udp_,
    //lásd nagy komment a fájl elején
    QMutexLocker locker(&jobs_lock_);
    udp_=new QUdpSocket;
    while(1)
    {
        jobs_wc_.wait(&jobs_lock_);
        while(!jobs_.isEmpty())
        {
            Job j=jobs_.dequeue();
            locker.unlock(); //a feldolgozás alatt nem kell védeni a queue-t
            switch(j.type)
            {
                case UDP:
                {
                    QBuffer buf;
                    buf.open(QIODevice::WriteOnly);
                    j.packet->save(&buf);
                    if(buf.size()>512) //magic udp csomagméret a Qt doksiból
                    {
                        qWarning("Sending > 512 bytes UDP packet (%c%c%c%c)!",
                                 DEBUG_MPACKET_cccc(*j.packet));
                    }
                    if(!udp_->writeDatagram(buf.buffer(),tgt_,MNServer::PORT))
                    {
                        qDebug("Sent datagram was too long!");
                    }
                }break;
                case TCP:
                {
                    if(!tcp_)
                    {
                        tcp_=new QTcpSocket;
                        tcp_->connectToHost(tgt_,MNServer::PORT);
                        tcp_->waitForConnected(1000);
                    }
                    j.packet->save(tcp_);
                    tcp_->flush();
                }break;
                case TCP_RECONNECT:
                {
                    tcp_->disconnectFromHost();
                    tcp_->waitForDisconnected(1000);
                    delete tcp_;
                    tcp_=NULL;
                }break;
                case QUIT:
                {
                    return;
                }
                default:
                {
                    qWarning("Bad job type %d",j.type);
                }
            }
            delete j.packet;
            locker.relock(); //ideiglenes mutexnyitás vége
        }
    }
    delete tcp_;
    delete udp_;
}

QHostAddress MNClient::serverAddress() const
{
    return tgt_;
}
