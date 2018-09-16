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

#ifndef MERROR_H
#define MERROR_H

#include "global.h"
#include "libmnp4/packets/mpframe.h"

class MError
{
    QMap<int,QByteArray> errorlist;

    int requestcount;

    //Struktúra tároláshoz - majd egy szofisztikáltabb hibakezeléshez, ha lesz hibakód is
    struct error_t
    {
        //Ablaktáblák
        char upperleft;
        char upperright;
        char lowerdown; //reserved
        char lowerright;//reserved

        int answercount;
    };

public:
    MError();
    ~MError();

    /**
     *  Registers a mueb error
     */
    void addError(QByteArray error,int floor,int door);


    /**
     * Gets the list of the faulted rooms
     */
    QMap<int,QByteArray> getErrorList() {return errorlist;}
};

#endif // MERROR_H
