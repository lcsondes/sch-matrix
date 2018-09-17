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

#ifndef LIBMNP_MPCOMMAND_H
#define LIBMNP_MPCOMMAND_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * A lejátszás vezérlésére szolgáló csomag.
 */
class MNP4_EXPORT MPControl : public MPacket
{
DECLARE_MNP_PACKET(MPControl);

protected:
    bool load2(QByteArray) override;
    QByteArray save2() override;
public:
	/**
	 * A használható parancsok.
	 */
    enum Command
    {
        BUFFER=0, ///< Implikált STOP, animációpuffer ürítése.
        PLAY=1, ///< Animáció lejátszása az elejéről.
        STOP=2, ///< Animációlejátszás megállítása, kép elsötétítése.
        PAUSE=3, ///< Animációlejátszás szüneteltetése.
        CONTINUE=4, ///< Animációlejátszás folytatása.
        /**
         * Tekerés az animációban.
         * data1=idő az animáció kezdete óta.
         * data2=1:hard, 0:soft tekerés
         * (hard: beteszi az új frame-et mindenképpen, soft: csak időzítést
         * állít - emiatt softtal nem lehet hátratekerni!)
         */
        SEEK=5,
        LAST__ ///< A beérkező csomag hibaellenőrzésére kell, nem használt
    } cmd; ///< Parancs. quint32-ként szerializálódik.

    //érdekel, minek ez az idős dolog? Az mcanimplayer.cpp elején a válasz!
    qint64 time; ///< A parancs küldésének időpontja.
    qint64 data1; ///< Adatmező. Értelmezése a küldött parancstól függ.
    qint64 data2; ///< Adatmező. Értelmezése a küldött parancstól függ.

    MPControl();
    explicit MPControl(Command);
};

#endif
