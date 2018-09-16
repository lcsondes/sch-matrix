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

#ifndef LIBMNP_GLOBAL_H
#define	LIBMNP_GLOBAL_H

#include <QtCore>
#include <QtNetwork>

//kódszépítő makrók
#define ownership //azt jelzi, hogy a pointerrel a tulajdonjog is megy
#define override //azt jelzi, hogy egy virtuális függvényt definiál felül

#ifdef MNP4_LIBRARY
#define MNP4_EXPORT Q_DECL_EXPORT
#else
#define MNP4_EXPORT Q_DECL_IMPORT
#endif

#endif
