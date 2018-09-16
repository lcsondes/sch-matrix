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

#include "libmnp4/network/mnworker.h"
#include "libmnp4/packets/mpacket.h"
#include "libmnp4/core/mceventdispatcher.h"

//direkt van rossz szálon, hogy legyen event loop, ami törölje.

void MNWorker::ctor()
{
    connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}

/**
 * "TCP konstruktor". A QTcpSocket létrehozása már a másik szálban történik.
 */
MNWorker::MNWorker(QObject* parent,qintptr socketDescriptor,ConnectionType type)
    :QThread(parent),socketDescriptor_(socketDescriptor),type_(type)
{
    ctor();
}

/**
 * "UDP konstruktor".
 */
MNWorker::MNWorker(QObject* parent,const QHostAddress& addr,
                   ownership QIODevice* dev,ConnectionType type)
    :QThread(parent),socketDescriptor_(-1),addr_(addr),dev_(dev),type_(type)
{
    ctor();
}

MNWorker::~MNWorker()
{
    delete dev_;
}

/**
 * A feldolgozást végző függvény. Minden feldolgozás saját szálon fut, a rövid
 * válaszidő érdekében.
 * TCP kapcsolat esetén befejezi a konstruktor munkáját (QIODevice-t húz a
 * socketDescriptor_ köré), majd nekiáll csomagokat deszerializálni a
 * QIODevice-ból. Ezeket elküldi az MCEventDispatcher-nek.
 */
void MNWorker::run()
{
    if(socketDescriptor_!=-1)
    {
        dev_=new QTcpSocket;
        ((QTcpSocket*)dev_)->setSocketDescriptor(socketDescriptor_,
                                                 QAbstractSocket::ConnectedState,
                                                 QIODevice::ReadWrite);
    }
    for(;;) //ha több csomag jön egy TCP-streamben, behúzzuk mindet
    {
        MPacket* packet=MPacket::load(dev_);
        if(!packet) //rossz adatnál és stream végénél is ez van
        {
            goto end; //sz*r az egész stream, kuka
        }
        /*qDebug("MNWorker: decoded packet as [%c%c%c%c]",
               packet->id().constData()[0],
               packet->id().constData()[1],
               packet->id().constData()[2],
               packet->id().constData()[3]);//*/
        MCEventDispatcher::instance()->dispatch(ownership packet,addr_);
    }
end:;
    deleteLater();
}
