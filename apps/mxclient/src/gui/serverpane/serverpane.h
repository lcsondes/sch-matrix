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

#ifndef SERVERPANE_H
#define	SERVERPANE_H

#include "global.h"

/**
 * A szerverrel kapcsolatos dolgokra egy widget
 */
class ServerPane : public QWidget
{
Q_OBJECT
    QGridLayout* layout_;
    QLabel* ipLabel_;
    QLineEdit* ip_;
    QPushButton* connect_;
    QLabel* status_;
    QTimer* refreshTick_;
    QTime lastResponse_;
    static const int TIMEOUT=1000;
public:
    static const QString SERVER_IP_QUERY;
    
    ServerPane(QWidget* parent=NULL);
private slots:
    void connectClicked();
    void incomingTime(); //a timesync egyfolytában megy - keepalive is egyben :D
    void refreshStatus();
signals:
    void connected(QHostAddress);
};

#endif
