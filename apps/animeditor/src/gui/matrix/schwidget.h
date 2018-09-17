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

#ifndef SCHWIDGET_H
#define	SCHWIDGET_H

#include "global.h"
#include "matrixitem.h"
#include "backgrounditem.h"
#include "tools.h"

uint qHash(QPoint);

class SchWidget : public QGraphicsView
{
Q_OBJECT
    bool sch_;

    QPixmap background_;
    BackgroundItem* backgroundItem_;

    int w_;
    int h_;

    bool editable_;
    bool dragFore;

    Palette* palette_;
    Tools* tools_;
    SchWindow* schWindow_;

    QGraphicsScene* scene_;

    Qt::MouseButtons pressedButtons_;
    void processMouseButton(QMouseEvent*);
    void processMouse(QMouseEvent*);
    void doText();

    //Rectanglehez meg Circlehez
    bool circleBanding_;
    QRect circleBandRect_;

    QPoint mousePressViewPoint_;
    QPoint mousePressPoint_;
    QPoint mouseReleasePoint_;
    QList<QGraphicsItem*> selectionList;
    void circleUnHighLight();
    void circleHighLight();
    void lineUnHighLight();
    void lineHighLight();
    void drawSCHObject(QMouseEvent*);
    void colorWindow(QPoint);
    void highLightWindow(QPoint);
    void unHighLightWindow(QPoint);

    void ctor();

public:
    void resizeEvent(QResizeEvent*) override;

    static /*const*/ uint WIDTH/*=0*/;
    static /*const*/ uint HEIGHT/*=0*/;
    
    static void init();

    MatrixItem* matrix_;

    SchWidget(Palette*,Tools*,SchWindow*);
    SchWidget(Palette*,Tools*,int x,int y,SchWindow*);
    QRgb pixelAt(int,int) const;
    void setPixel(int,int,QRgb);

    void showFrame(Frame* frame);

    bool isEditable();
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

public slots:
    void setEditable(bool);
    void clear();
    void floodFill(int x,int y,QRgb source,QRgb dest,QSet<QPoint>*);
signals:
    void clipDropped(int,int,Clip*);
    void pixelSet(int,int,QRgb);
    void pixelChanged(int,int,QRgb);
    void clearPixels();
};

#endif
