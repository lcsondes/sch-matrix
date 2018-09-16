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

#ifndef SELECTEDCOLORS_H
#define	SELECTEDCOLORS_H

#include "global.h"
#include "colorswatch.h"

class SelectedColors : public QFrame
{
Q_OBJECT
    bool foregroundSelected_;
    ColorSwatch* foreground_;
    ColorSwatch* background_;
private slots:
    void foregroundSelected();
    void backgroundSelected();
protected:
    void resizeEvent(QResizeEvent*);
public:
    SelectedColors(QWidget* parent=NULL);
    bool isForegroundSelected() const;
    QRgb foreground() const;
    QRgb background() const;
    QRgb color(bool fore) const;
    void setForeground(QRgb);
    void setBackground(QRgb);
public slots:
    void setSelected(QRgb);
};

#endif
