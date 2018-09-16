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

#include "palette.h"
#include "colorswatch.h"
#include "selectedcolors.h"

//TODO palettán felkiáltójel, ha nem roundToHw a szín

static const QRgb predef_colors[]=
{
    //CGA+HTML4 által ihletett paletta. Ha tudsz jobbat, ne habozz, cseréld le!
    //a formátum 0xAARRGGBB
    0xff000000,0xff222222,0xff555555,0xff666666,
    0xff888888,0xffaaaaaa,0xffcccccc,0xffffffff,

    0xff000055,0xff000088,0xff0000aa,0xff0000ff,
    0xff005500,0xff008800,0xff00aa00,0xff00ff00,
    0xff005555,0xff008888,0xff00aaaa,0xff00ffff,
    0xff550000,0xff880000,0xffaa0000,0xffff0000,
    0xff550055,0xff880088,0xffaa00aa,0xffff00ff,
    0xff555500,0xff888800,0xffaa5500,0xffffff00,
    //
    0x7f000000,0x7f555555,0x7faaaaaa,0x7fffffff,

    0x7f000077,0x7f0000ff,
    0x7f007700,0x7f00ff00,
    0x7f007777,0x7f00ffff,
    0x7f770000,0x7fff0000,
    0x7f770077,0x7fff00ff,
    0x7f773300,0x7fffff00,

    //kétféle átlátszó
    0x00000000,0x00ffffff
};

Palette::Palette(QWidget* parent/*=NULL*/)
    :QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    layout_=new QGridLayout(this);
    layout_->setAlignment(Qt::AlignCenter);
    layout_->setSpacing(0);
    colors_=new SelectedColors(this);
    layout_->addWidget(colors_,0,0,SHEIGHT,1,Qt::AlignCenter);
    layout_->setColumnMinimumWidth(1,ColorSwatch::SIZE);
    uint predef=0;
    for(int y=0;y<SHEIGHT;++y)
    {
        for(int x=0;x<SWIDTH;++x)
        {
            ColorSwatch* s=new ColorSwatch(this);
            connect(s,SIGNAL(clicked(QRgb)),
                    colors_,SLOT(setSelected(QRgb)));
            if(predef<ARRAY_SIZE(predef_colors))
            {
                s->setColor(roundToHw(predef_colors[predef++]));
            }
            layout_->addWidget(s,y,x+2,1,1);
        }
    }
}

extern bool noroundtohw;

/**
 * Színkerekítést végez a MUEB-ek aktuális képességéhez mérten.
 */
QRgb Palette::roundToHw(QRgb c)
{
    if(noroundtohw)return c;
    //színkerekítés az aktuális hardver képességeihez

    /*
     * A vidám számok:     R   G   B
     * RGB444 (4096 szín): 17, 17, 17 (255/15)
     * RGB333 (512 szín) : 255.0/7.0
     * RGB222 (64 szín)  : 85, 85, 85 (255/5)
     * RGB221 (32 szín)  : 85, 85,255
     */
    static const double happy_number_r=255.0/7.0;
    static const double happy_number_g=255.0/7.0;
    static const double happy_number_b=255.0/7.0;

    return qRgba(qRound(qRed  (c)/happy_number_r)*happy_number_r,
                 qRound(qGreen(c)/happy_number_g)*happy_number_g,
                 qRound(qBlue (c)/happy_number_b)*happy_number_b,
                 qAlpha(c));
}

QRgb Palette::color(bool fore) const
{
    return colors_->color(fore);
}
