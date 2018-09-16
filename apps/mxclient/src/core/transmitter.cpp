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

#include "transmitter.h"
#include "libmnp4/network/mnserver.h"
#include "libmnp4/network/mnclient.h"

void Transmitter::ctor(QHostAddress tgt)
{
    moveToThread(this);
    if(tgt.isNull())
    {
        tgt=MNClient::globalInstance()->serverAddress();
    }
    tgt_=tgt;
}

Transmitter::Transmitter(QHostAddress tgt/*=QHostAddress()*/)
    :sock_(NULL)
{
    ctor(tgt);
}

Transmitter::Transmitter(const QList<MPFrame>& frames,
                         QHostAddress tgt/*=QHostAddress()*/)
    :sock_(NULL)
{
    ctor(tgt);
    transmit(frames);
}

Transmitter::~Transmitter()
{
    quit();
    wait();
}

void Transmitter::transmit(const QList<MPFrame>& frames)
{
    Q_ASSERT(!sock_);
    frames_=frames;
    
    start();
}

void Transmitter::run()
{
    sock_=new QTcpSocket(this);
    sock_->connectToHost(tgt_,MNServer::PORT);
	sock_->waitForConnected(); // Qt 5 óta ezt muszáj kivárni, nem bufferel előre
    for(int i=0;i<frames_.size();++i)
    {
        frames_[i].save(sock_);
    }
    exec(); //és most várunk

	//erről a szálról érdemes megölni
	delete sock_;
}