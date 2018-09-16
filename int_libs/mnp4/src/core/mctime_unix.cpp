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
#ifdef Q_OS_UNIX

#include <sys/time.h>

/*
 * Egyszerű getTimeMsCore() implementáció gettimeofday használatával. Nem
 * találtam ennél pontosabb órát, de ettől még persze lehet, hogy van.
 */
qint64 MCTime::getTimeMsCore()
{
    timeval a;
    gettimeofday(&a,NULL);
    return qint64(a.tv_sec)*1000+a.tv_usec/1000;
}

#endif
