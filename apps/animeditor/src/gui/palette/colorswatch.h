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

#ifndef COLORSWATCH_H
#define	COLORSWATCH_H

#include "global.h"

/**
 * Egy színt tartalmazó kis négyzet a palettán.
 */
class ColorSwatch : public QFrame
{
Q_OBJECT
    QBrush background_;
    QRgb color_;
protected:
    void contextMenuEvent(QContextMenuEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
public:
    static const int SIZE=16;
    ColorSwatch(QWidget* parent=NULL);
    QRgb color() const;
public slots:
    void setColor(QRgb);
signals:
    void clicked(QRgb);
};

#endif
