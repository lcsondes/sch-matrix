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

#ifndef SAFEBUTTON_H
#define	SAFEBUTTON_H

#include "global.h"

/**
 * Véletlenül csak nehezen lenyomható gomb.
 */
class SafeButton : public QFrame
{
Q_OBJECT
    QHBoxLayout* layout_;
    QCheckBox* cb_;
    QPushButton* pb_;
    void ctor();
public:
    SafeButton(QString text,QWidget* parent=NULL);
    SafeButton(QIcon icon,QWidget* parent=NULL);
    void setIconSize(const QSize&);
signals:
    void clicked();
};

#endif
