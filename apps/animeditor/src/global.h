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

#ifndef GLOBAL_H
#define	GLOBAL_H

#include "pch.h"

#define ownership
#define in
#define out
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

//időt konvertálnak stringbe, nem itt lenne a helyük

inline QString hms(int ms)
{
    QTime t(0,0);
    t=t.addMSecs(ms);
    return t.toString(QObject::tr("m:ss"));
}

inline QString hmsm(int ms)
{
    QTime t(0,0);
    t=t.addMSecs(ms);
    return t.toString(QObject::tr("m:ss.zzz"));
}

#endif
