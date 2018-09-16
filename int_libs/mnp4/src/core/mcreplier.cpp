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

#include "libmnp4/core/mcreplier.h"
#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/packets/mpping.h"
#include "libmnp4/network/mnserver.h"

DEFINE_MNP_INITIALIZABLE_STRICT(MCReplier);

MCReplier::MCReplier()
{
    MCEventDispatcher::instance()->connect<MPPing>(this,
                                                   MCEVENT_SLOT(incomingPing));
    MCEventDispatcher::instance()->connect<MPStatus>(this,
                                                    MCEVENT_SLOT(incomingStat));
}

MCReplier::~MCReplier()
{
}

/**
 * Ha pingkérés érkezett, válaszol rá. Ha pingválasz érkezett, elsüti
 * a pingResponse() signal-t.
 */
void MCReplier::incomingPing(QSharedPointer<const MPacket> packet,
                             QHostAddress replyTo)
{
    qDebug("MCPingReplier::incomingPing()");
    switch(packet.staticCast<const MPPing>()->type)
    {
        case MPPing::REQUEST:
        {
            MPPing response(MPPing::RESPONSE);
            response.sendViaUdp(replyTo);
            qDebug("MCReplier: replied to a PING");
        }break;
        case MPPing::RESPONSE:
        {
            emit pingResponse(replyTo);
        }break;
        default:
        {
            qDebug("Invalid PING packet got to MCReplier");
        }break;
    }
}

/**
 * Állapotkérésre válaszol egy MPStatus::AutoResponse-t.
 */
void MCReplier::incomingStat(QSharedPointer<const MPacket> packet,
                             QHostAddress replyTo)
{
    const MPStatus* p=packet.staticCast<const MPStatus>().data();
    //qDebug()<<"Stat received: type"<<p->type;
    switch(p->type)
    {
        case MPStatus::REQUEST:
        {
            MPStatus response=MPStatus(MPStatus::AutoResponse());
            response.sendViaUdp(replyTo);
            qDebug("MCReplier: replied to a STAT");
        }break;
        case MPStatus::RESPONSE:
        {
            emit statusResponse(*p,replyTo);
        }break;
        default:
        {
            qDebug("Invalid STAT packet got to MCReplier");
        }break;
    }
}
