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

#ifndef LIBMNP_MPFRAME_H
#define	LIBMNP_MPFRAME_H

#include "libmnp4/global.h"
#include "libmnp4/init.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Animáció-képkockát tartalmazó csomag. Mivel marha nagy, főleg TCP-n
 * keresztül közlekedik.
 *
 * Vagy az #MCAnimPlayer bufferébe kerül, vagy egyből ki a képre, a #seq-től függően.
 */
class MPFrame : public MPacket
{
DECLARE_MNP_PACKET(MPFrame);
private:
    void ctor();
    void copy(const MPFrame&);
    void dtor();

    friend void libmnp_init(QVariant(*)(QString));
    friend void libmnp_shutdown();
    static void init();
    static void shutdown();

protected:
    bool load2(QByteArray);
    QByteArray save2();
public:
    static const QString WIDTH_QUERY;
    static const QString HEIGHT_QUERY;

    static /*const*/ int WIDTH; ///< A Mátrix szélessége
    static /*const*/ int HEIGHT; ///< A Mátrix magassága

    quint32 seq; ///< Sorszám egy animáción belül. 0=immediate
    quint32 delay; ///< Idő, ameddig ez a frame látszik. Immediate-nél ignored.

    //A QRgb nem használható, mert QtGui
    quint32** pixels; ///< A kép pixelei (XRGB8888 formátumban)

    MPFrame();
    struct BlankIt{};
    explicit MPFrame(BlankIt); //ez ki is nullázza
    MPFrame(const MPFrame&);
    MPFrame(MPFrame&&);
    void operator=(const MPFrame&);
    ~MPFrame();
        
    bool isValid() const;
};

#endif