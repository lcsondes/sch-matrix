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

#include "libmnp4/core/mctime.h"
#ifdef Q_OS_WIN

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

static bool gotFreq=false;
static LARGE_INTEGER freq;

/*
 * Egyszerű getTimeMsCore() implementáció performance counterekkel.
 *
 * Note: a performance countereket sokan szidják, mert állítólag össze-vissza
 * ugrálnak, viszont az én tapasztalataim szerint ultrapontosak.
 */
qint64 MCTime::getTimeMsCore()
{
    if(!gotFreq)
    {
        QueryPerformanceFrequency(&freq);
        gotFreq=true;
    }
    LARGE_INTEGER a;
    QueryPerformanceCounter(&a);
    return a.QuadPart*1000/freq.QuadPart;
}

#endif
