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

#ifndef FRAME_H
#define	FRAME_H

#include "global.h"

/**
 * Egy frame.
 */
class Frame
{
    uint width_,height_;
    uint delay_;
    QRgb** pixels_;
    void ctor(bool black=false);
    void dtor();
public:
    static QRgb alphaBlend(QRgb source,QRgb target);

    Frame(uint,uint);
    struct Black{};
    explicit Frame(uint,uint,Black);
    Frame(const Frame&);
    Frame(Frame&&);
    ~Frame();
    bool operator==(const Frame&) const; //nem néz delayt!
    QRgb pixelAt(uint,uint) const;
    QRgb& pixelAt(uint,uint);
    void setPixel(uint,uint,QRgb);
    uint delay() const;
    uint width() const;
    uint height() const;
    void setDelay(uint);
    void shiftPixels(int,int);

    virtual void render(Frame*,const QTransform&) const;
};

#endif
