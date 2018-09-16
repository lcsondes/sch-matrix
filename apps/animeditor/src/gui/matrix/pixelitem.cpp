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

#include "pixelitem.h"
#include "palette/palette.h"
//#include "video/clip.h"

const int PixelItem::TYPE;
const int PixelItem::SIZE;

PixelItem::PixelItem(int x,int y,bool t,
                     QGraphicsItem* parent/*=NULL*/)
    :QGraphicsObject(parent),color_(qRgba(0,0,0,0)),x_(x),y_(y),
    h_count_(0),transparent_(t),highlighted_(false),selectmode_(false)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptDrops(true);
}

void PixelItem::setSelectMode(bool val)
{
    selectmode_=val;
    if(selectmode_)
    {
        //Set cursor shape
        setCursor(QCursor(Qt::CrossCursor));
    }
    else
    {
        //Set cursor shape
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

QRectF PixelItem::boundingRect() const
{
    return QRectF(0,0,SIZE,SIZE);
}

void PixelItem::paint(QPainter* p,const QStyleOptionGraphicsItem*,QWidget*)
{
    //a zárójel azért kell, hogy a gcc örüljön, egyébként felesleges
    QRgb color=color_;
    if(highlighted_)color=(color^0xFFFFFFFF)|0xFF000000;
    if(!transparent_)
    {
        //kerekítés az aktuális hardver képességeihez
        color=Palette::roundToHw(qRgba(qRed  (color)*qAlpha(color)/255,
                                       qGreen(color)*qAlpha(color)/255,
                                       qBlue (color)*qAlpha(color)/255,
                                       255));
    }
    //ne fessünk pepitát potyára
    if(transparent_&&qAlpha(color)!=255)
    {
        static const int HSIZE=SIZE/2;
        static const QColor trans1(Qt::black);
        static const QColor trans2(qRgb(0x40,0x40,0x40));
        for(int x=0;x<=1;++x)
        {
            for(int y=0;y<=1;++y)
            {
                p->fillRect(x*HSIZE,y*HSIZE,HSIZE,HSIZE,x^y?trans2:trans1);
            }
        }
    }
    p->fillRect(0,0,SIZE,SIZE,QColor(qRed  (color),
                                     qGreen(color),
                                     qBlue (color),
                                     qAlpha(color)));
}

void PixelItem::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if(event->mimeData()->hasFormat("matrix/clip"))
    {
        event->setAccepted(event->mimeData()->hasFormat("matrix/clip"));

        Clip* clip=*(Clip**)(event->mimeData()->data("matrix/clip").data());

        emit highLightFrame(x_,y_,clip);
    }
}

void PixelItem::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
    if(event->mimeData()->hasFormat("matrix/clip"))
    {
        event->setAccepted(event->mimeData()->hasFormat("matrix/clip"));

        emit unHighLightFrame();
    }
}

void PixelItem::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if(event->mimeData()->hasFormat("matrix/clip"))
    {
        event->setAccepted(event->mimeData()->hasFormat("matrix/clip"));

        Clip* clip=*(Clip**)(event->mimeData()->data("matrix/clip").data());

        emit unHighLightFrame();
        emit clipDropped(x_,y_,clip);
    }
}

//Ez azért kell, mert különben selectionnak veszi, és nem felpasszolja!!
void PixelItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->ignore();
}

int PixelItem::type() const
{
    return TYPE;
}

QRgb PixelItem::color() const
{
    return color_;
}

QPoint PixelItem::getPos()
{
    return QPoint(x_,y_);
}

void PixelItem::setColor(QRgb c,bool persistent/*=false*/)
{
    color_=c;
    emit colorChanged(x_,y_,c);
    if(persistent)emit pixelSet(x_,y_,c);
    update();
}

void PixelItem::highLight(int x,int y,Clip* clip)
{
    int h=clip->height();
    int w=clip->width();
    h_count_++;

    if((x_>=x)&&(x_<x+w)&&(y_>=y)&&(y_<y+h))
    {
        highlighted_=true;
        update();
    }
    else
    {
        highlighted_=false;
        update();
    }
}

void PixelItem::highLight(int x,int y,int w,int h)
{
    h_count_++;

    if((x_>=x)&&(x_<x+w)&&(y_>=y)&&(y_<y+h))
    {
        highlighted_=true;
        update();
    }
    else
    {
        highlighted_=false;
        update();
    }
}

void PixelItem::highLightThis()
{
    highlighted_=true;
    update();
}

void PixelItem::unHighLight()
{
    h_count_--;

    if(((h_count_%2)==0)&&highlighted_)
    {
        highlighted_=false;
        update();
    }
}

void PixelItem::unHighLightThis()
{
    highlighted_=false;
    update();
}

void PixelItem::clear()
{
    color_=qRgba(0,0,0,0);
}
