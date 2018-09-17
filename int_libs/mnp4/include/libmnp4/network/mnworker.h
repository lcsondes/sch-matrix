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

#ifndef LIBMNP_MNWORKER_H
#define	LIBMNP_MNWORKER_H

#include "libmnp4/global.h"

/**
 * Egy bejövő kapcsolat vagy üzenet feldolgozását végzi. Ez főként a csomagok
 * deszerializálásából áll. A csomagok az MCEventDispatcher-be kerülve érik el
 * úticéljukat.
 */
class MNP4_EXPORT MNWorker : public QThread
{
Q_OBJECT
    void ctor();
public:
    enum ConnectionType
    {
        //Note: jelenleg minden válasz UDP-ben megy ki, a TCP-re is.
        //Ez később jöhet jól.
        TCP, //lehet válaszolni a kapott QIODevice-ba
        UDP //kapcsolódni kell a kapott címre
    };
private:
	qintptr socketDescriptor_; ///< A socket leírója. TCP esetén kell.
    QHostAddress addr_; ///< Válaszcím.
    /**
     * A bejövő csomagot szolgáltató QIODevice. UDP esetén egy QBuffer, TCP
     * esetén egy QTcpSocket, melyet a konstruktor hoz létre a
     * socketDescriptor_-ból.
     */
    QIODevice* dev_;
    ConnectionType type_; ///< A kapcsolat típusa.
public:
    MNWorker(QObject*,ownership qintptr,ConnectionType);
    MNWorker(QObject*,const QHostAddress&,ownership QIODevice*,ConnectionType);
    ~MNWorker();
    void run() override;
};

#endif
