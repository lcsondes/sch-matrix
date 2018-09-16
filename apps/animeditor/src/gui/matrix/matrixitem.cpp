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

#include "matrixitem.h"

MatrixItem::MatrixItem(int w,int h,bool transparent,bool doubleY)
    :width_(w),height_(h),transparent_(transparent),doubleY_(doubleY)
{
    items_=new PixelItem**[width_];
    for(int x=0;x<width_;++x)
    {
        items_[x]=new PixelItem*[height_];
        for(int y=0;y<height_;++y)
        {
            PixelItem* item=new PixelItem(x,y,transparent,this);
            item->setPos((transparent?translatePixelCoordsTrans
                                     :translatePixelCoordsSch)(x,y,doubleY_));
            item->setFlag(ItemStacksBehindParent,true);
            connect(item,SIGNAL(colorChanged(int,int,QRgb)),
                    this,SIGNAL(pixelChanged(int,int,QRgb)));
            connect(item,SIGNAL(pixelSet(int,int,QRgb)),
                    this,SIGNAL(pixelSet(int,int,QRgb)));

            connect(item,SIGNAL(clipDropped(int,int,Clip*)),
                    this,SIGNAL(clipDropped(int,int,Clip*)));
            connect(this,SIGNAL(highLightFrame(int,int,int,int)),
                    item,SLOT(highLight(int,int,int,int)));

            connect(item,SIGNAL(highLightFrame(int,int,Clip*)),
                    this,SIGNAL(highLightFrame(int,int,Clip*)));
            connect(this,SIGNAL(highLightFrame(int,int,Clip*)),
                    item,SLOT(highLight(int,int,Clip*)));
            connect(item,SIGNAL(unHighLightFrame()),
                    this,SIGNAL(unHighLightFrame()));
            connect(this,SIGNAL(unHighLightFrame()),
                    item,SLOT(unHighLight()));

            connect(this,SIGNAL(clearPixels()),item,SLOT(clear()));
            items_[x][y]=item;
        }
    }
    PixelItem* tl=items_[0][0];
    PixelItem* br=items_[width_-1][height_-1];
    boundingRect_=tl->boundingRect().translated(tl->pos()).
           united(br->boundingRect().translated(br->pos()));
}

MatrixItem::~MatrixItem()
{
    //a gyerekeket szétpusztítja a ~QGraphicsItem()
    for(int x=0;x<width_;++x)
    {
        delete items_[x];
    }
    delete items_;
}

PixelItem* MatrixItem::getPixel(QPoint pos)
{
    return items_[pos.x()][pos.y()];
}

QRectF MatrixItem::boundingRect() const
{
    //át kell baszni a Qt-t, különban az itemAt ezt adja vissza és nem a pixel-
    //Itemeket
    return QRectF(1,1,0,0);
}

QRectF MatrixItem::boundingRect2() const
{
    return boundingRect_;
}

void MatrixItem::paint(QPainter* p,const QStyleOptionGraphicsItem*,QWidget*)
{
    if(transparent_)
    {
        p->setPen(Qt::white);
        for(int x=0;x<width_/2;++x)
        {
            for(int y=0;y<height_/2;++y)
            {
                QRect rect;
                rect.setTopLeft(translatePixelCoordsTrans(x*2,y*2,doubleY_));
                rect.setSize(QSize(PixelItem::SIZE*2,PixelItem::SIZE*2));
                p->drawRect(rect);
            }
        }
    }
}

QPoint MatrixItem::translatePixelCoordsSch(int x,int y,bool doubleY)
{
    //ezek a konstansok az sch.jpg-hez vannak belőve - ne cseréld őket
    //a nevesített változatokra (pl. PixelItem::SIZE)!
    double dy=doubleY?11.2:22.4;
    return ((x&~1)*QPointF(14.0,0 )).toPoint()+(x&1)*QPoint(8,0)+
           ((y&~1)*QPointF(0   ,dy)).toPoint()+(y&1)*QPoint(0,8);
}

//az új pixelitem-festés miatt ez nem kell

QPoint MatrixItem::translatePixelCoordsTrans(int x,int y,bool doubleY)
{
    return translatePixelCoordsSch(x,y,doubleY);
    /*
    int xx=(x&~1)*PixelItem::SIZE*1.5            +(x&1)*PixelItem::SIZE;
    int yy=(y&~1)*PixelItem::SIZE*(doubleY?1.5:3)+(y&1)*PixelItem::SIZE;
    return QPoint(xx,yy);
    */
}

QRgb MatrixItem::colorAt(int x,int y) const
{
    Q_ASSERT(x>=0&&x<width_);
    Q_ASSERT(y>=0&&y<height_);
    return items_[x][y]->color();
}

void MatrixItem::setColor(int x,int y,QRgb c,bool persistent/*=false*/)
{
    Q_ASSERT(x>=0&&x<width_);
    Q_ASSERT(y>=0&&y<height_);
    items_[x][y]->setColor(c,persistent);
}

void MatrixItem::clear()
{
    emit clearPixels();
}

void MatrixItem::highLight(int x,int y,int w,int h)
{
    emit highLightFrame(x,y,w,h);
}

void MatrixItem::highLightRect(int x1,int y1,int x2,int y2)
{
    emit highLightFrame(x1<x2?x1:x2,y1<y2?y1:y2,((x2-x1)>=0?(x2-x1):(x1-x2))+1,
            ((y2-y1)>=0?(y2-y1):(y1-y2))+1);
}

void MatrixItem::unHighLight()
{
    emit unHighLightFrame();
}
