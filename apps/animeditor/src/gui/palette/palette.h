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

#ifndef PALETTE_H
#define	PALETTE_H

#include "global.h"
#include "selectedcolors.h"

/**
 * A paletta. Színt kerekíteni is tud.
 */
class Palette : public QWidget
{
Q_OBJECT
    QGridLayout* layout_;
    SelectedColors* colors_;
public:
    //a színek darabszámát jelzik - 16x4-es grid
    static const int SWIDTH=16;
    static const int SHEIGHT=4;

    static QRgb roundToHw(QRgb);

    Palette(QWidget* parent=NULL);
    QRgb color(bool fore) const;
};

#endif
