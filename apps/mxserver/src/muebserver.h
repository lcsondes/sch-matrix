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

#ifndef MUEBSERVER_H
#define	MUEBSERVER_H

#include "global.h"
#include "merror.h"
#include "libmnp4/init.h"
#include "libmnp4/packets/mpframe.h"

/**
 * Szerver<->MUEB kommunikáció
 */
class MuebServer : public QThread
{
Q_OBJECT

    DECLARE_MNP_INITIALIZABLE(MuebServer);
    MuebServer();
    ~MuebServer();
    void run();

    MError errorlist;

public:
    /**
     * Visszaadja a hibás szobák listáját
     */
    QMap<int,QByteArray> getErrorList() {return errorlist.getErrorList();}

public slots:
    /**
     * Ezt hívja a szerver többi része, ha képet akar kiküldeni
     */
    void sendFrame(MPFrame frame);

    /**
     * Lekérdezi a muebek státuszinfrmációit
     */
    void sendStatusRequest();

protected slots:
    /**
     * Bejövő UDP csomagot fogad
     */
    void newUdp();

    /**
     * Bejövő státuszüzeneteket dolgozza fel
     */
    void receiveStatus(quint16 port,QByteArray payload,int floor,int door);

signals:
    /**
     * Csomag érkezését jelzi egy MUEB-től.
     * A paraméterek:
     * port,adat,emelet,ajtó
     */
    void incoming(quint16,QByteArray,int,int);
};

#endif
