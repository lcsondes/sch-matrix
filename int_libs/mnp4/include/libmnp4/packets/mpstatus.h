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

#ifndef MPSTATUS_H
#define	MPSTATUS_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Státuszlekérő csomag a szerver távoli felügyeletéhez.
 * A lejátszás is használja, főleg a #validFrames mezőt.
 */
class MNP4_EXPORT MPStatus : public MPacket
{
DECLARE_MNP_PACKET(MPStatus);

protected:
    bool load2(QByteArray) override;
    QByteArray save2() override;
public:
    enum Type
    {
        REQUEST=0,
        RESPONSE=1,
        INVALID
    } type; ///< quint32-ként szerializálódik
    //ezek csak akkor szerializálódnak, ha a type RESPONSE
    qint32 validFrames; ///< Az utolsó érvényes tárolt képkocka sorszáma. -1, ha nincs ilyen
    qint64 time; ///< A szerver órája
    //..., amit csak akarsz :)

    MPStatus(Type type=INVALID);
    struct AutoResponse{};
    explicit MPStatus(AutoResponse);
};

#endif
