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

#include "libmnp4/core/mcanimloader.h"

#include <QtIOCompressor>

#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/core/mcconfig.h"
#include "libmnp4/packets/mpload.h"
#include "libmnp4/packets/mpframe.h"

DEFINE_MNP_INITIALIZABLE_STRICT(MCAnimLoader);

const QString MCAnimLoader::PATH_QUERY("libmnp4.animpath");

MCAnimLoader::MCAnimLoader()
{
    MCEventDispatcher::instance()->
            connect<MPLoad>(this,MCEVENT_SLOT(packetReceived));
}

/**
 * Betöltési parancs érkezett
 */
void MCAnimLoader::packetReceived(QSharedPointer<const MPacket> packet,
                                  QHostAddress replyTo)
{
    const MPLoad* p=packet.staticCast<const MPLoad>().data();
    QString basepath=MCConfig::query(PATH_QUERY).toString()+"/";
    QIODevice* dev=NULL;
    QString q4x_name=basepath+p->name+".q4x";
    QString qpr_name=basepath+p->name+".qpr";
	
	//A Q4X töltődik be, ha mindkettő létezik
    if(QFile::exists(q4x_name))
    {
        dev=open_q4x(q4x_name);
    }
    else if(QFile::exists(qpr_name))
    {
        dev=open_qpr(qpr_name);
    }
	
    if(!dev)
    {
        qDebug()<<p->name<<"is not cached";
        return;
    }

    qDebug()<<p->name<<"found in local cache!";
    
    int seq=1;
    for(;;)
    {
        MPFrame* frame=new MPFrame;
        frame->seq=seq++;
        for(int y=0;y<MPFrame::HEIGHT;++y)
        {
            for(int x=0;x<MPFrame::WIDTH;++x)
            {
                quint8 px[3];
                dev->read((char*)px,3);
                //az utolsó szám után lehet még whitespace
                if(dev->atEnd())
                {
                    goto readDone;
                }
                frame->pixels[x][y]=px[0]<<16|px[1]<<8|px[2];
            }
        }
        quint32 delay;
        dev->read((char*)&delay,4);
        delay=qFromBigEndian<quint32>(delay);
        frame->delay=delay;
        MCEventDispatcher::instance()->dispatch(ownership frame,replyTo);
    }
readDone:;
    delete dev;
}

/**
 * Q4X-et nyit meg, és olvassa be az első frame kezdetéig
 * \return Kulcsrakész QIODevice, már csak olvasni kell belőle
 */
ownership QIODevice* MCAnimLoader::open_q4x(QString filename)
{
    //a QtIOCompressor ~QObject()-je majd törli
    QFile* f=new QFile(filename);
    f->open(QIODevice::ReadOnly);
    if(f->read(4)!="Q4X1")
    {
        delete f;
        return NULL;
    }
    {
        quint16 w,h;
        f->read((char*)&w,2);
        f->read((char*)&h,2);
        w=qFromBigEndian<quint16>(w);
        h=qFromBigEndian<quint16>(h);
        if(w!=MPFrame::WIDTH||h!=MPFrame::HEIGHT)
        {
            delete f;
            return NULL;
        }
    }
    quint32 skip;
    f->read((char*)&skip,4);
    skip=qFromBigEndian<quint32>(skip);
    f->seek(f->pos()+skip+4);//qpr méretét is
    QtIOCompressor* comp=new QtIOCompressor(f);
    comp->open(QIODevice::ReadOnly);
    if(comp->readLine()!="qpr v1\n")
    {
        delete comp;
        delete f;
        return NULL;
    }
    comp->readLine();
    comp->readLine();
    comp->readLine();
    f->setParent(comp);//hogy a ~QObject() elintézze
    return comp;
}

/**
 * QPR-t nyit meg, és olvassa be az első frame kezdetéig
 * \return Kulcsrakész QIODevice, már csak olvasni kell belőle
 */
ownership QIODevice* MCAnimLoader::open_qpr(QString filename)
{
    QFile* f=new QFile(filename);
    f->open(QIODevice::ReadOnly);
    if(f->readLine()!="qpr v1\n")
    {
        delete f;
        return NULL;
    }
    f->readLine();
    f->readLine();
    f->readLine();
    return f;
}
