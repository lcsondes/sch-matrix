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

#ifndef LIBMNP_MCEMITTER_H
#define	LIBMNP_MCEMITTER_H

#include "libmnp4/global.h"
#include "libmnp4/packets/mpacket.h"

/**
 * Csomagonként elkülöníti a signalokat. A MCEventDispatcher belső osztályának
 * kéne lennie, de a moc nem támogat olyat.
 */
class MNP4_EXPORT MCEmitter : public QObject
{
Q_OBJECT
friend class MCEventDispatcher;
signals:
    /*
     * Új csomag érkezését jelzi. Közvetlenül nem használható, az
     * MCEventDispatcher::connect-en keresztül lehet feliratkozni rá.
     * \param packet A csomag. Megosztott elérésű is lehet, ezért konstans.
     * \param replyTo A válaszcím.
     */
    void newPacket(QSharedPointer<const MPacket> packet,QHostAddress replyTo);
};

#endif
