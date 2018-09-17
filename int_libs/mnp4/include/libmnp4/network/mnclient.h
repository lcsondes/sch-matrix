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

#ifndef LIBMNP_MNCLIENT_H
#define	LIBMNP_MNCLIENT_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Kényelmi osztály a szerver irányába történő adatküldésre.
 * Nem a szokásos singleton makrókat használja a szükséges
 * paraméter miatt. Singletonként és sima objektumként is használható,
 * akár több is egyszere.
 */
class MNP4_EXPORT MNClient : public QThread
{
Q_OBJECT

public:
    /**
     * TCP és UDP magától értetődik.
     * TCP_RECONNECT a kapcsolatot újraindítja. Érdemes hívogatni egy nagy batch
     * után (pl. animáció). Packet nem kell.
     * Note: Jelenleg az MPacket::load 1-2 másodperces timeouttal dolgozik,
     * ezért muszáj TCP_RECONNECT-et hívni minden burst után!
     * QUIT befejezi a futást. Csak belső használatra!
     */
    enum JobType{UDP,TCP,TCP_RECONNECT,QUIT};
private:
    struct Job
    {
        //ne adj a Job-nak destruktort, mert az első értékadásnál megölné a packetet.
        ownership MPacket* packet; ///< Az MNClient szabadítja fel.
        JobType type;
    };

    static MNClient* globalInstance_; ///< Az osztály nagy közös objektuma.

    friend void MNP4_EXPORT libmnp_shutdown();
    static void shutdown();

    QHostAddress tgt_; ///< A szerver címét tárolja.
    QTcpSocket* tcp_; ///< A szerverhez kapcsolódó TCP socket.
    union
    {
        //Az inicializációhoz kell, lásd nagy komment a .cpp elején
        void* volatile initHelper_;
        QUdpSocket* udp_; ///< A szerver irányába UDP-csomagokat küldő socket.
    };
    //Bármit akarunk csinálni a jobs_-szal, úgyis mutexelni kell
    QMutex jobs_lock_; ///< A jobs_ szálbiztos használatára való mutex.
    QWaitCondition jobs_wc_; ///< A jobs_-ba történő írást jelző esemény.
    QQueue<Job> jobs_; ///< A feldolgozásra váró csomagok.

    void run() override;
public:
    //Bár csinálhatsz magadnak példányokat belőle, egy globális példány is
    //rendelkezésre áll. Használd inkább azt!

    MNClient(const QHostAddress&);
    ~MNClient();

    void send(ownership MPacket*,JobType); //threadsafe
    void sendOneTcp(ownership MPacket*); //threadsafe
    void sendOneUdp(ownership MPacket*); //threadsafe
    void tcpReconnect(); //threadsafe

    QHostAddress serverAddress() const;

    static void init(const QHostAddress&);
    static MNClient* globalInstance();
};

#endif
