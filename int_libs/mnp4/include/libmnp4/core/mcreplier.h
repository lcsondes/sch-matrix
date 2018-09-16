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

#ifndef LIBMNP_MCPINGREPLIER_H
#define	LIBMNP_MCPINGREPLIER_H

#include "libmnp4/global.h"
#include "libmnp4/init.h"
#include "libmnp4/packets/mpping.h"
#include "libmnp4/packets/mpstatus.h"

/**
 * MPPing és MPStatus csomagokat intéz: válaszol rájuk, illetve az érkezett
 * válaszokra biztosít egy signal-t.
 */
class MCReplier : public QObject
{
Q_OBJECT
DECLARE_MNP_INITIALIZABLE(MCReplier);

    MCReplier();
    ~MCReplier();
public slots:
    void incomingPing(QSharedPointer<const MPacket>,QHostAddress);
    void incomingStat(QSharedPointer<const MPacket>,QHostAddress);
signals:
    void pingResponse(QHostAddress); ///< Érkezett pingválaszokkor sül el.
    /// Érkezett státuszválaszokkor sül el.
    void statusResponse(MPStatus,QHostAddress);
};

#endif
