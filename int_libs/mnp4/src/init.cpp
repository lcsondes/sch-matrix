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

#include <stdio.h>

#include "libmnp4/init.h"
#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/network/mnserver.h"
#include "libmnp4/core/mcreplier.h"
#include "libmnp4/network/mnclient.h"
#include "libmnp4/core/mctime.h"
#include "libmnp4/core/mcanimplayer.h"
#include "libmnp4/packets/mpacket.h"
#include "libmnp4/packets/mpframe.h"
#include "libmnp4/packets/mpstatus.h"
#include "libmnp4/core/mcconfig.h"
#include "libmnp4/core/mcanimloader.h"

/**
 * Inicializálja a libraryt. Meg kell hívni, mielőtt bármi mást csinálnál.
 * \param cfg Függvény, ami konfigurációs értékeket szállít a libnek. Lásd #MCConfig
 */
void libmnp_init(QVariant(*cfg)(QString))
{
    //ez az MCEventDispatcher-nek kell
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<QSharedPointer<const MPacket>>("QSharedPointer<const MPacket>");
	//ez az animációnak
	qRegisterMetaType<MPFrame>("MPFrame");
    //ez pedig az MCReplier-nek
    qRegisterMetaType<MPStatus>("MPStatus");

    MCConfig::init(cfg);
    MPFrame::init();
    //MCEventDispatcher::init(); - ő magától elindul
}

/**
 * Leállítja a libraryt. Kilépés előtt érdemes meghívni.
 */
void libmnp_shutdown()
{
    //kliens
    MNClient::shutdown();
    //szerver
    MCAnimLoader::shutdown();
    MCAnimPlayer::shutdown();
    //network
    MPFrame::shutdown();
    //common
    MCTime::shutdown();
    MCReplier::shutdown();
    MNServer::shutdown();
    //core
    MCEventDispatcher::shutdown();
    //config
    MCConfig::shutdown();
}

/**
 * Egy segédfüggvény, mely a szerver és a kliens közös részét inicializálja.
 */
static void libmnp_start_common()
{
    MNServer::init();
    MCReplier::init();
    MCTime::init();
}

/**
 * Elindítja a szerver-üzemmódot.
 */
void libmnp_start_server()
{
    libmnp_start_common();
    MCAnimPlayer::init();
    MCAnimLoader::init();
}

/**
 * Elindítja a kliens-üzemmódot, és csatlakozik a szerverhez.
 * A válaszok fogadására a szervert is elindítja, nem kell libmnp_start_server-t
 * hívni (sőt, nem is szabad).
 * \param addr A szerver IP-címe.
 */
void libmnp_start_client(const QHostAddress& tgt)
{
    libmnp_start_common();
    MNClient::init(tgt);
    MCTime::startSyncing();
}
