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

#ifndef LIBMNP_MCCONFIG_H
#define	LIBMNP_MCCONFIG_H

#include "libmnp4/global.h"

/**
 * Beállításokat kezelő osztály. Singleton, de a paraméter miatt nem makróval készült.
 * A beállításokat a host-tól kapja, de vannak egyezményes nevek. Ezek:
 * Kötelező?    Beállítás neve          Jelentés
 * Y            libmnp4.frame.width     A Mátrix szélessége pixelben
 * Y            libmnp4.frame.height    A Mátrix magassága pixelben
 *              libmnp4.animpath        A lokális animáció-cache elérési útja
 */
class MCConfig
{
private:
    static MCConfig* instance_; ///<Az egyetlen globális objektum
    QVariant(*cfg_)(QString); ///<A konfigurációt szolgáltató függvény
	
	MCConfig();
public:
    static void init(QVariant(*)(QString));
    static void shutdown();
    static MCConfig* instance();

    static QVariant query(QString);
};

#endif
