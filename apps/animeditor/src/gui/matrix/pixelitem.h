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

#ifndef PIXELITEM_H
#define	PIXELITEM_H

#include "global.h"
#include "palette.h"
#include "clip.h"

class PixelItem : public QGraphicsObject
{
Q_OBJECT
    QRgb color_; //a setteren keresztül bántsd
    int x_,y_;
    int h_count_; //Hax. ez kell, mert keveri a drop eventek sorrendjét!!!
    bool transparent_;
    bool highlighted_;
    bool selectmode_;
    
public:
    static const int SIZE=8; //legyen páros

    void paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent*) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent*) override;
    void dropEvent(QGraphicsSceneDragDropEvent*) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    static const int TYPE=QGraphicsItem::UserType+42; //azért, mert csak™
    int type() const override;
    PixelItem(int,int,bool transparent,QGraphicsItem* parent=NULL);
    QRgb color() const;
    void setColor(QRgb,bool persistent=false);
    QPoint getPos();

    void highLightThis();
    void unHighLightThis();


public slots:
    void highLight(int,int,Clip*);
    void highLight(int,int,int,int);
    void unHighLight();
    void setSelectMode(bool);
    void clear();

signals:
    void colorChanged(int,int,QRgb);
    void pixelSet(int,int,QRgb);
    void clipDropped(int,int,Clip*);
    void highLightFrame(int,int,Clip*);
    void unHighLightFrame();
    void startSelect(int,int);
    void stopSelect(int,int);
};

#endif
