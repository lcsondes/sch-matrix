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

#include "libmnp4/network/mnserver.h"
#include "libmnp4/network/mnworker.h"

DEFINE_MNP_INITIALIZABLE_STRICT(MNServer);

const int MNServer::PORT;

/**
 * Konstruktor. Egyből el is indítja a szálat.
 */
MNServer::MNServer()
{
    moveToThread(this);
    start();
}

MNServer::~MNServer()
{
    instance_->quit();
    if(!instance_->wait(1000))
    {
        qWarning("MNServer couldn't shut down gracefully!");
        instance_->terminate();
    }
}

/**
 * Elvégzi a konstruktor másik szálon végzendő feladatait, majd elindítja a szál
 * event loop-ját. Az event loop kilépése után a destruktor munkáját is elkezdi.
 */
void MNServer::run()
{
    //a socketeket csak ezen a szálon lehet legyártani
    
    tcp_=new MNQTcpServer(this);
    if(!tcp_->listen(QHostAddress::Any,PORT))
    {
        qFatal("TCP Couldn't listen on port %d",PORT);
    }else qDebug("TCP listening on port %d",PORT);

    udp_=new QUdpSocket(this);
    if(!udp_->bind(QHostAddress::Any,PORT))
    {
        qFatal("UDP couldn't bind to port %d",PORT);
    }else qDebug("UDP listening on port %d",PORT);

    connect(tcp_,SIGNAL(newConnection2(qintptr)),this,SLOT(slotNewTcp(qintptr)));
    connect(udp_,SIGNAL(readyRead()),this,SLOT(slotNewUdp()));

    exec();

    //erről a szálról jobb ezeket megölni
    delete tcp_;
    delete udp_;
}

/**
 * A kapott TCP kapcsolatra ráállít egy MNWorker-t.
 */
void MNServer::slotNewTcp(qintptr socketDescriptor)
{
    //qDebug("MNServer: incoming TCP connection");
    (new MNWorker(this,socketDescriptor,MNWorker::TCP))->start();
}

/**
 * A kapott UDP csomagra ráállít egy MNWorker-t.
 */
void MNServer::slotNewUdp()
{
    while(udp_->hasPendingDatagrams())
    {
        //qDebug("MNServer:: incoming UDP datagram");
        QBuffer* dev=new QBuffer;
        qint64 size=udp_->pendingDatagramSize();
        dev->buffer().resize(size);

        QHostAddress addr;
        udp_->readDatagram(dev->buffer().data(),size,&addr,NULL);
        
        dev->open(QIODevice::ReadOnly); //MIÉÉÉÉÉÉÉÉÉÉÉÉÉÉRT?!?!?! :'(
        (new MNWorker(this,addr,dev,MNWorker::UDP))->start();
    }
}
