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

#ifndef LIBMNP_MNSERVER_H
#define	LIBMNP_MNSERVER_H

#include "libmnp4/global.h"
#include "libmnp4/init.h"
#include "libmnp4/network/mnqtcpserver.h"

/**
 * TCP kapcsolatok és UDP csomagok fogadását végző osztály. A feldolgozást is
 * elindítja, de azt az #MNWorker végzi. Singleton.
 */
class MNP4_EXPORT MNServer : public QThread
{
Q_OBJECT
DECLARE_MNP_INITIALIZABLE(MNServer);

    MNServer();
    ~MNServer();

    MNQTcpServer* tcp_; ///< TCP szerver socket
    QUdpSocket* udp_; ///< UDP szerver socket

    void run() override;
public:
    static const int PORT=3172; ///< A szerver portja. Találd ki, miért ez! ;)
private slots:
    void slotNewTcp(qintptr);
    void slotNewUdp();
};

#endif
