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

#include "colorswatch.h"
#include "palette.h"

const int ColorSwatch::SIZE;

/**
 * Az "átlátszó" pepita háttér xpm formátumban.
 */
static const char* const transbg_xpm[]=
{
    "2 2 2 1",
    "a c #ffffff",
    "b c #c0c0c0",
    "ab",
    "ba"
};

ColorSwatch::ColorSwatch(QWidget* parent/*=NULL*/)
    :QFrame(parent),color_(qRgba(0,0,0,0))
{
    background_=QBrush(QPixmap(transbg_xpm).scaled(16,16));
    setFrameStyle(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    setMinimumSize(SIZE,SIZE);
    setLineWidth(2);
}

void ColorSwatch::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu m;
    m.addAction(tr("Change color..."));
    QAction* result=m.exec(event->globalPos());
    if(result)
    {
        QColor c=QColorDialog::getColor(QColor(),this,QString(),
                                        QColorDialog::ShowAlphaChannel);
        if(c.isValid())setColor(c.rgba());
        update();
    }
}

void ColorSwatch::resizeEvent(QResizeEvent*)
{
    if(width()!=height())
    {
        int rect=std::max(width(),height());
        resize(rect,rect);
    }
}

void ColorSwatch::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    painter.setBrush(background_);
    painter.setPen(Qt::NoPen);
    painter.drawRect(frameWidth(),frameWidth(),
                     width()-frameWidth()*2,height()-frameWidth()*2);
    painter.setBrush(QColor::fromRgba(color_));
    painter.drawRect(frameWidth(),frameWidth(),
                     width()-frameWidth()*2,height()-frameWidth()*2);
}

void ColorSwatch::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton)
    {
        emit clicked(color_);
    }
}

QRgb ColorSwatch::color() const
{
    return color_;
}

void ColorSwatch::setColor(QRgb c)
{
    color_=Palette::roundToHw(c);
    update();
}
