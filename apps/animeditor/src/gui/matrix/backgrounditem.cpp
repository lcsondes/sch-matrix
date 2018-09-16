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

#include "backgrounditem.h"
#include "pixelitem.h"

BackgroundItem::BackgroundItem(const QPixmap& pixmap,
                               QGraphicsItem* parent/*=NULL*/)
    :QGraphicsItem(parent)
{
    bg_=new QGraphicsPixmapItem(pixmap,this);
}

//void BackgroundItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
//{
//    //az az item kap move-ot, amire kattintottak - delegáljuk oszt jóvan
//    //mousemove garantáltan mousepress után hívódik meg - see Qt doksi
////    event->setButton(pressedButton_); //ezt fixelni kell

////    QGraphicsItem* tgt=scene()->itemAt(event->scenePos());
////    if(tgt&&tgt->type()==PixelItem::TYPE)
////    {
////        //ez pixelitem lesz
////        static_cast<PixelItem*>(tgt)->mousePressEvent(event);
////        static_cast<PixelItem*>(tgt)->mouseReleaseEvent(event);
////    }
//}

//void BackgroundItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
//{
//    pressedButton_=event->button();
//}

QRectF BackgroundItem::boundingRect() const
{
    return bg_->boundingRect();
}

void BackgroundItem::paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*)
{
}
