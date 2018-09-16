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

#include "muebserver.h"
#include "libmnp4/core/mcanimplayer.h"

DEFINE_MNP_INITIALIZABLE_STRICT(MuebServer);

static const quint16 LISTEN_PORTS[]={40000};
//10.255.255.255
static const QHostAddress BROADCAST_ADDR("10.255.255.255");

MuebServer::MuebServer()
{
    moveToThread(this);
    connect(MCAnimPlayer::instance(),SIGNAL(frameAppeared(MPFrame)),
            this,SLOT(sendFrame(MPFrame)));
    start();
}

MuebServer::~MuebServer()
{
    quit();
    wait();
    //a QUdpSocketeket összevadássza a ~QObject()
}

void MuebServer::run()
{
    for(uint i=0;i<ARRAY_SIZE(LISTEN_PORTS);++i)
    {
        quint16 port=LISTEN_PORTS[i];
        if(port)
        {
            QUdpSocket* s=new QUdpSocket(this); //nem leakel, mert van parentje
            connect(s,SIGNAL(readyRead()),this,SLOT(newUdp()));
            s->bind(QHostAddress::Any,port);
        }
    }
    connect(this,SIGNAL(incoming(quint16,QByteArray,int,int)),
            this,SLOT(receiveStatus(quint16,QByteArray,int,int)));
    exec();
}

void MuebServer::newUdp()
{
    QUdpSocket* sock=(QUdpSocket*)sender();
    while(sock->hasPendingDatagrams())
    {
        QByteArray buf;
        buf.resize(sock->pendingDatagramSize());
        QHostAddress addr;
        sock->readDatagram(buf.data(),buf.size(),&addr,NULL);
        quint32 ip=addr.toIPv4Address();

        //A MUEB-ek IP-je 10.1.emelet.ajtó
        emit incoming(sock->localPort(),buf,(ip>>8)&0xFF,ip&0xFF);
    }
}

/**
 * Bejövő státuszüzeneteket dolgozza fel
 */
void MuebServer::receiveStatus(quint16 port,QByteArray payload,
                               int floor,int door)
{
    //Port ellenőrzése
    if(port!=40000)
    {
        //Nem státusz
        return;
    }

    //Csomagméret ellenőrzése
    if(payload.size()!=6)
    {
        //Rosszméret
        return;
    }

    //Hiba eltárolása
    errorlist.addError(payload,floor,door);
}

/**
 * A muebek státuszának lekérdezése
 */
void MuebServer::sendStatusRequest()
{
    //Csomag heggesztés
    QByteArray data;

    //Verzió, ID
    data.append((char)1);
    data.append((char)1);

    //Válasz IP és MAC kitöltése 10 byte
    //(Valaki mesélje már el, hogy ez mire jó???)
    for(int i=0;i<10;i++)
    {
        data.append((char)0);
    }

    //Adat kiküldése
    QUdpSocket sock;

    sock.writeDatagram(data,BROADCAST_ADDR,20000);
}

/**
 * Ezt hívja a szerver többi része, ha képet akar kiküldeni
 */
void MuebServer::sendFrame(MPFrame frame)
{
    qDebug()<<"Sending frame seq"<<frame.seq<<"delay"<<frame.delay;
    //Csomag heggesztése
    QByteArray data;

    for(int y=0;y<MPFrame::HEIGHT/2;++y)
    {
        for(int x=0;x<MPFrame::WIDTH/2;++x)
        {
            //felső 2 ablak
            quint8 a,b,c,d,e,f;
            
            //alsó 4 bitbe kerekíti bele a színt
            auto lo4=[](quint8 color)->quint8
            {
                return quint8(qRound(color/255.0*7.0));
            };
            //felső 4 bitbe kerekíti be a színt
            auto hi4=[lo4](quint8 color)->quint8
            {
                return lo4(color)<<4;
            };
            
            //pixel 1
            a=hi4(frame.pixels[x*2  ][y*2  ]>>16&0xFF)|
              lo4(frame.pixels[x*2  ][y*2  ]>> 8&0xFF);
            b=hi4(frame.pixels[x*2  ][y*2  ]    &0xFF)|
            //pixel 2
              lo4(frame.pixels[x*2+1][y*2  ]>>16&0xFF);
            c=hi4(frame.pixels[x*2+1][y*2  ]>> 8&0xFF)|
              lo4(frame.pixels[x*2+1][y*2  ]    &0xFF);
            //pixel 3
            d=hi4(frame.pixels[x*2  ][y*2+1]>>16&0xFF)|
              lo4(frame.pixels[x*2  ][y*2+1]>> 8&0xFF);
            e=hi4(frame.pixels[x*2  ][y*2+1]    &0xFF)|
            //pixel 4
              lo4(frame.pixels[x*2+1][y*2+1]>>16&0xFF);
            f=hi4(frame.pixels[x*2+1][y*2+1]>> 8&0xFF)|
              lo4(frame.pixels[x*2+1][y*2+1]    &0xFF);
            
            data.append((char)a);
            data.append((char)b);
            data.append((char)c);
            data.append((char)d);
            data.append((char)e);
            data.append((char)f);
        }
    }

    int size=data.size()/4;
    QByteArray p1=data.mid(size*0,size);
    QByteArray p2=data.mid(size*1,size);
    QByteArray p3=data.mid(size*2,size);
    QByteArray p4=data.mid(size*3,size);
    p1.prepend("\x01\x01");
    p2.prepend("\x01\x02");
    p3.prepend("\x01\x03");
    p4.prepend("\x01\x04");


    //Csomag küldése, kétszer, hogy tutira megérkezzen minden muebhez
    QUdpSocket sock;

    sock.writeDatagram(p1,BROADCAST_ADDR,10000);
    sock.writeDatagram(p2,BROADCAST_ADDR,10000);
    sock.writeDatagram(p3,BROADCAST_ADDR,10000);
    sock.writeDatagram(p4,BROADCAST_ADDR,10000);
    sock.writeDatagram(p1,BROADCAST_ADDR,10000);
    sock.writeDatagram(p2,BROADCAST_ADDR,10000);
    sock.writeDatagram(p3,BROADCAST_ADDR,10000);
    sock.writeDatagram(p4,BROADCAST_ADDR,10000);
}
