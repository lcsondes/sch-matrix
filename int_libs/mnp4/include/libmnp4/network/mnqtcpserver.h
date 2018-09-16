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

#ifndef LIBMNP_MNQTCPSERVER_H
#define	LIBMNP_MNQTCPSERVER_H

#include "libmnp4/global.h"

/**
 * QTcpServer wrapper, melynek incomingConnection függvénye a newConnection2
 * signal-ra van kötve. Ez az egyetlen működőképes módja TCP socketek szálak
 * közti mozgatásának Qt-ben sajnos.
 */
class MNQTcpServer : public QTcpServer
{
Q_OBJECT
    override void incomingConnection(int);
public:
    MNQTcpServer(QObject* parent=NULL);
signals:
    void newConnection2(/*ownership*/ int); //a moc nem szereti a makrót
};

#endif