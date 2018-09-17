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

#ifndef TRANSMITTER_H
#define	TRANSMITTER_H

#include "global.h"
#include "libmnp4/packets/mpframe.h"

/**
 * Egy animáció átstreamelését végzi.
 */
class Transmitter : public QThread
{
Q_OBJECT
    QTcpSocket* sock_;
    QHostAddress tgt_;
    QList<MPFrame> frames_;
    
    void ctor(QHostAddress);
    
    void run() override;
public:
    Transmitter(QHostAddress _=QHostAddress());
    Transmitter(const QList<MPFrame>&,QHostAddress _=QHostAddress());
    ~Transmitter();
    
    void transmit(const QList<MPFrame>&);
};

#endif
