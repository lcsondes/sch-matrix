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

#ifndef INTERPOLATOR_H
#define	INTERPOLATOR_H

#include "global.h"

/**
 * Két érték között lineáris interpolál.
 */
class Interpolator
{
    friend class QP4Handler;
    friend uint qHash(const Interpolator&);
    QMap<int,int> values_;
public:
    bool operator==(const Interpolator&) const;

    int value(int t) const;
    void setValue(int t,int v);
    void remove(int t);
    int nextKey(int t) const;
    int prevKey(int t) const;
    void deltaShift(int d);
    void clear();

    void debugprint();
};

uint qHash(const Interpolator&);

#endif