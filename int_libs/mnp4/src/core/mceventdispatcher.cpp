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

#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/core/mcemitter.h"
#include "libmnp4/packets/mpacket.h"

MCEventDispatcher* MCEventDispatcher::instance_=NULL;

void MCEventDispatcher::shutdown()
{
    delete instance_;
}

MCEventDispatcher* MCEventDispatcher::instance()
{
    if(!instance_)instance_=new MCEventDispatcher;
    return instance_;
}

MCEventDispatcher::MCEventDispatcher()
{
}

MCEventDispatcher::~MCEventDispatcher()
{
    for(auto i=emitters_.begin();i!=emitters_.end();++i)
    {
        delete i.value();
    }
}

/**
 * Az adott csomagtípusra feliratkozott objektumokat értesíti a csomag
 * érkezéséről.
 * \param packet A csomag.
 * \param replyTo A válaszcím.
 */
void MCEventDispatcher::dispatch(ownership MPacket* packet,
                                 const QHostAddress& replyTo)
{
    MCEmitter* emitter=emitters_[packet->id()];
    /*qDebug("Dispatching [%c%c%c%c] using %p",
           packet->id().constData()[0],
           packet->id().constData()[1],
           packet->id().constData()[2],
           packet->id().constData()[3],emitter);//*/
    if(emitter)emitter->emit newPacket(QSharedPointer<const MPacket>(packet),
                                       replyTo);
}
