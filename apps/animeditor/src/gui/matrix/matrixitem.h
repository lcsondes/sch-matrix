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

#ifndef MATRIXITEM_H
#define	MATRIXITEM_H

#include "global.h"
#include "pixelitem.h"
#include "clip.h"

class MatrixItem : public QGraphicsObject
{
Q_OBJECT
    PixelItem*** items_;
    QRectF boundingRect_;
    int width_;
    int height_;
    bool transparent_;
    bool doubleY_;

    MatrixItem(const MatrixItem&)=delete;
public:
    override QRectF boundingRect() const;
    QRectF boundingRect2() const;
    override void paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*);
    static QPoint translatePixelCoordsSch(int,int,bool doubleY);
    static QPoint translatePixelCoordsTrans(int,int,bool doubleY);

    MatrixItem(int,int,bool trans,bool dY);
    ~MatrixItem();
    PixelItem* getPixel(QPoint);
    QRgb colorAt(int,int) const;
    void setColor(int,int,QRgb,bool persistent=false);
public slots:
    void clear();
    void highLight(int,int,int,int);
    void highLightRect(int,int,int,int);
    void unHighLight();

signals:
    void pixelChanged(int,int,QRgb);
    void pixelSet(int,int,QRgb);
    void clipDropped(int,int,Clip*);
    void highLightFrame(int,int,Clip*);
    void highLightFrame(int,int,int,int);
    void unHighLightFrame();
    void clearPixels();
};

#endif
