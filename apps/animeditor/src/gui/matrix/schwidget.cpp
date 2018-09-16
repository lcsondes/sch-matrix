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

#include "schwidget.h"
#include "frame.h"
#include "schwindow.h"
#include "textrenderdialog.h"
#include "libmnp4/core/mcconfig.h"
#include "libmnp4/packets/mpframe.h"

/*const*/ uint SchWidget::WIDTH=0;
/*const*/ uint SchWidget::HEIGHT=0;

uint qHash(QPoint p)
{
    return(p.x()*p.y())^(p.x()^p.y());
}

void SchWidget::colorWindow(QPoint pos)
{
    QRgb color=palette_->color(dragFore);
    if(color==qRgba(0,0,0,0))return;
    matrix_->getPixel(pos)->setColor(palette_->color(dragFore),true);
    if(tools_->windowMode())
    {
        matrix_->getPixel(QPoint(pos.x()^1,pos.y()))->setColor(palette_->color(dragFore),true);
        matrix_->getPixel(QPoint(pos.x(),pos.y()^1))->setColor(palette_->color(dragFore),true);
        matrix_->getPixel(QPoint(pos.x()^1,pos.y()^1))->setColor(palette_->color(dragFore),true);
    }
}

void SchWidget::highLightWindow(QPoint pos)
{
    matrix_->getPixel(pos)->highLightThis();
    if(tools_->windowMode())
    {
        matrix_->getPixel(QPoint(pos.x()^1,pos.y()))->highLightThis();
        matrix_->getPixel(QPoint(pos.x(),pos.y()^1))->highLightThis();
        matrix_->getPixel(QPoint(pos.x()^1,pos.y()^1))->highLightThis();
    }
}

void SchWidget::unHighLightWindow(QPoint pos)
{
    matrix_->getPixel(pos)->unHighLightThis();
    if(tools_->windowMode())
    {
        matrix_->getPixel(QPoint(pos.x()^1,pos.y()))->unHighLightThis();
        matrix_->getPixel(QPoint(pos.x(),pos.y()^1))->unHighLightThis();
        matrix_->getPixel(QPoint(pos.x()^1,pos.y()^1))->unHighLightThis();
    }
}

void SchWidget::init()
{
    //a main.cpp tömi bele, tuti jó lesz
    WIDTH=MCConfig::query(MPFrame::WIDTH_QUERY).toUInt();
    HEIGHT=MCConfig::query(MPFrame::HEIGHT_QUERY).toUInt();
}

void SchWidget::ctor()
{
    scene_=new QGraphicsScene(this);
    setScene(scene_);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    matrix_=new MatrixItem(w_,h_,!sch_,false);
    connect(matrix_,SIGNAL(pixelChanged(int,int,QRgb)),
            this,SIGNAL(pixelChanged(int,int,QRgb)));
    connect(matrix_,SIGNAL(pixelSet(int,int,QRgb)),
            this,SIGNAL(pixelSet(int,int,QRgb)));
    scene_->addItem(matrix_);

    if(sch_)
    {
        setBackgroundBrush(QColor(24,26,15));//kb. ilyen színű az sch.jpg háttere

        matrix_->setPos(70,171);//sch.jpg magic konstans, ne bántsd

        backgroundItem_=new BackgroundItem(background_);
        scene_->addItem(backgroundItem_);
        backgroundItem_->setZValue(-1);
    }
    else
    {
        setBackgroundBrush(Qt::black);
    }

    connect(matrix_,SIGNAL(clipDropped(int,int,Clip*)),
            this,SIGNAL(clipDropped(int,int,Clip*)));

    connect(this,SIGNAL(clearPixels()),matrix_,SLOT(clear()));

    setDragMode(QGraphicsView::RubberBandDrag);
}

SchWidget::SchWidget(Palette* palette,Tools* tools,SchWindow* parent)
    :QGraphicsView(parent),sch_(true),background_(":/pics/sch"),
    w_(WIDTH),h_(HEIGHT),editable_(true),dragFore(true),palette_(palette),
    tools_(tools),schWindow_(parent),circleBanding_(false)
{
    ctor();
}

SchWidget::SchWidget(Palette* palette,Tools* tools,int x,int y,
                     SchWindow* parent)
    :QGraphicsView(parent),sch_(false),background_(":/pics/sch"),
    w_(x),h_(y),editable_(true),dragFore(true),palette_(palette),
    tools_(tools),schWindow_(parent),circleBanding_(false)
{
    ctor();
}

void SchWidget::resizeEvent(QResizeEvent*)
{
    QSize bksize;
    if(sch_)
    {
        bksize=background_.size();
    }
    else
    {
        bksize=matrix_->boundingRect2().size().toSize();
    }
    fitInView(0,0,bksize.width(),bksize.height(),Qt::KeepAspectRatio);
}

QRgb SchWidget::pixelAt(int x,int y) const
{
    return matrix_->colorAt(x,y);
}

void SchWidget::setPixel(int x,int y,QRgb c)
{
    matrix_->setColor(x,y,c);
}

void SchWidget::setEditable(bool val)
{
    editable_=val;
}

void SchWidget::clear()
{
    emit clearPixels();
}

void SchWidget::floodFill(int x,int y,QRgb source,QRgb dest,
                          QSet<QPoint>* visitedPixels)
{
    if(matrix_->colorAt(x,y)==dest||visitedPixels->contains(QPoint(x,y)))
    {
        return;
    }

    matrix_->setColor(x,y,dest,true);
    visitedPixels->insert(QPoint(x,y));

    if((x-1)>=0&&matrix_->colorAt(x-1,y)==source)
    {
        floodFill(x-1,y,source,dest,visitedPixels);
    }
    if((x+1)<w_&&matrix_->colorAt(x+1,y)==source)
    {
        floodFill(x+1,y,source,dest,visitedPixels);
    }
    if((y-1)>=0&&matrix_->colorAt(x,y-1)==source)
    {
        floodFill(x,y-1,source,dest,visitedPixels);
    }
    if((y+1)<h_&&matrix_->colorAt(x,y+1)==source)
    {
        floodFill(x,y+1,source,dest,visitedPixels);
    }
}

bool SchWidget::isEditable()
{
    return editable_;
}

void SchWidget::showFrame(Frame* frame)
{
    if(frame->width()!=uint(w_)||frame->height()!=uint(h_))
    {
        return;
    }

    for(int x=0;x<w_;x++)
    {
        for(int y=0;y<h_;y++)
        {
            setPixel(x,y,frame->pixelAt(x,y));
        }
    }
}

void SchWidget::mousePressEvent(QMouseEvent* event)
{
    processMouseButton(event);
}

void SchWidget::mouseMoveEvent(QMouseEvent* event)
{
    processMouse(event);
}

void SchWidget::mouseReleaseEvent(QMouseEvent* event)
{
    processMouseButton(event);
}

void SchWidget::processMouseButton(QMouseEvent* event)
{
    pressedButtons_=event->buttons();
    processMouse(event);
}

void SchWidget::processMouse(QMouseEvent* event)
{
    if(!editable_)return;
    QGraphicsItem* tgt;

    //Ha valahogy változna a tool körhúzás közben, kicsit undi de így tisztább
    if(circleBanding_&&
       ((tools_->activeTool()!=Tools::CIRCLE&&tools_->activeTool()!=Tools::LINE
         &&tools_->activeTool()!=Tools::RECTANGLE)))
    {
        circleUnHighLight();
        drawSCHObject(event);
    }
    switch(tools_->activeTool())
    {
        case Tools::PENCIL:
        {
            if(tools_->windowMode())goto ilovesem;
            tgt=itemAt(event->pos());
            if(tgt&&(tgt->type()==PixelItem::TYPE))
            {
                //ez pixelitem lesz
                if(pressedButtons_&Qt::LeftButton)
                    ((PixelItem*)tgt)->setColor(palette_->color(true),true);
                else if(pressedButtons_&Qt::RightButton)
                    ((PixelItem*)tgt)->setColor(palette_->color(false),true);
            }
        }break;

ilovesem:;
            //case Tools::BRUSH:
        {
            tgt=itemAt(event->pos());
            if(tgt&&(tgt->type()==PixelItem::TYPE))
            {
                //ez pixelitem lesz
                bool fore;
                QPoint pos;
                if(pressedButtons_&Qt::LeftButton)fore=true;
                else if(pressedButtons_&Qt::RightButton)fore=false;
                else return;
                pos=((PixelItem*)tgt)->getPos();
                matrix_->getPixel(pos)->setColor(palette_->color(fore),true);
                matrix_->getPixel(QPoint(pos.x()^1,pos.y()))->setColor(palette_->color(fore),true);
                matrix_->getPixel(QPoint(pos.x(),pos.y()^1))->setColor(palette_->color(fore),true);
                matrix_->getPixel(QPoint(pos.x()^1,pos.y()^1))->setColor(palette_->color(fore),true);
            }
        }break;

        case Tools::FILL:
        {
            tgt=itemAt(event->pos());
            if(tgt&&(tgt->type()==PixelItem::TYPE))
            {
                bool fore;
                if(pressedButtons_&Qt::LeftButton)fore=true;
                else if(pressedButtons_&Qt::RightButton)fore=false;
                else return;

                PixelItem* pixel=(PixelItem*)tgt;

                //quick&dirty fix
                QSet<QPoint> visitedPixels;
                floodFill(pixel->getPos().x(),pixel->getPos().y(),
                          pixel->color(),palette_->color(fore),
                          &visitedPixels);

            }
        }break;

        case Tools::LINE:
        case Tools::RECTANGLE:
        case Tools::CIRCLE:
        {

            switch(event->type())
            {
                case QEvent::MouseButtonPress:
                {
                    if(circleBanding_)
                    {
                        if(tools_->activeTool()==Tools::LINE)
                            lineUnHighLight();
                        else
                            circleUnHighLight();
                        drawSCHObject(event);
                    }
                    mousePressViewPoint_=event->pos();
                    mousePressPoint_=event->pos();
                    if(pressedButtons_&Qt::LeftButton)dragFore=true;
                    else dragFore=false;
                    circleBanding_=true;
                    circleBandRect_=QRect();
                    selectionList.clear();
                }
                    break;
                case QEvent::MouseButtonRelease:
                {
                    if(circleBanding_)
                    {
                        if(tools_->activeTool()==Tools::LINE)
                            lineUnHighLight();
                        else
                            circleUnHighLight();
                        drawSCHObject(event);
                    }
                    if(pressedButtons_)
                    {
                        mousePressViewPoint_=event->pos();
                        if(pressedButtons_&Qt::LeftButton)dragFore=true;
                        else dragFore=false;
                        circleBanding_=true;
                        circleBandRect_=QRect();
                        selectionList.clear();
                    }
                }
                    break;
                case QEvent::MouseMove:
                {
                    // Check for enough drag distance
                    if(circleBanding_)
                    {
                        if((mousePressViewPoint_-event->pos()).manhattanLength()
                           <QApplication::startDragDistance())
                        {
                            return;
                        }

                        if(tools_->activeTool()==Tools::LINE)
                            lineUnHighLight();
                        else
                            circleUnHighLight();

                        // Stop rubber banding if the user has let go of all buttons (even
                        // if we didn't get the release events).
                        if(!event->buttons())
                        {
                            circleBanding_=false;
                            circleBandRect_=QRect();
                            return;
                        }

                        // Update rubberband position
                        const QPoint &mp=mousePressViewPoint_;
                        QPoint ep=event->pos();
                        circleBandRect_=QRect(qMin(mp.x(),ep.x()),qMin(mp.y(),ep.y()),
                                              qAbs(mp.x()-ep.x())+1,qAbs(mp.y()-ep.y())+1);

                        // Set the new selection area
                        QPainterPath selectionArea;
                        switch(tools_->activeTool())
                        {
                            case Tools::CIRCLE:
                            {
                                selectionArea.addEllipse(QRectF(mapToScene(circleBandRect_.topLeft()),
                                                                mapToScene(circleBandRect_.bottomRight())));
                            }
                                break;
                            case Tools::RECTANGLE:
                            {
                                selectionArea.addRect(QRectF(mapToScene(circleBandRect_.topLeft()),
                                                             mapToScene(circleBandRect_.bottomRight())));
                            }
                                break;
                            case Tools::LINE:
                            {
                                selectionArea.moveTo(mapToScene(mp));
                                selectionArea.lineTo(mapToScene(ep));
                            }
                                break;
                            default:;
                        }
                        selectionArea.closeSubpath();
                        if(scene())
                            scene()->setSelectionArea(selectionArea,Qt::IntersectsItemShape,
                                                      viewportTransform());

                        selectionList=scene()->selectedItems();

                        if(tools_->activeTool()==Tools::LINE)
                            lineHighLight();
                        else
                            circleHighLight();

                    }
                }
                    break;
                default:;
            }
        }break;

        case Tools::TEXT:
        {
            doText();
        }break;

        default:;
    }
}

void SchWidget::doText()
{
    TextRenderDialog d;
    if(d.exec()!=QDialog::Accepted)return;
    QFontMetrics fm(d.font());
    QRect rect=fm.boundingRect(d.text());
    if(rect.width()&1)rect.adjust(0,0,1,0);
    if(rect.height()&1)rect.adjust(0,0,0,1);

    qDebug()<<rect;

    QImage img(rect.size(),QImage::Format_ARGB32);
    QRgb color;
    img.fill(0x00000000);
    {
        QPainter p(&img);
        bool fore;
        if(pressedButtons_&Qt::LeftButton)fore=true;
        else if(pressedButtons_&Qt::RightButton)fore=false;
        else return;
        color=palette_->color(fore);
        p.setPen(QColor(qRed(color),
                        qGreen(color),
                        qBlue(color),
                        qAlpha(color)));
        QFont font=d.font();
        if(d.windowMode())font.setStyleStrategy(QFont::NoAntialias);
        p.setFont(font);
        qDebug()<<rect;
        p.drawText(-rect.topLeft(),d.text());
    }

    Embed* text=new Embed;
    text->x.setValue(0,SchWidget::WIDTH);
    text->x.setValue(d.time(),-rect.width());
    text->y.setValue(0,0);
    text->z=0;
    text->t=0;
    text->clip=new Clip(rect.width(),rect.height());

    QString basetext=tr("text_%1_%2").arg(d.text());
    QString basewrap=tr("wrap_%1_%2").arg(d.text());
    int nameidx=0;
    while(1)
    {
        ++nameidx;
        if(schWindow_->sceneTree_->checkName(basetext.arg(nameidx),schWindow_->
                                             sceneTree_->root()->clip.data())
           &&
           schWindow_->sceneTree_->checkName(basewrap.arg(nameidx),schWindow_->
                                             sceneTree_->root()->clip.data()))
        {
            break;
        }
    }
    text->clip->setName(basetext.arg(nameidx));
    {
        Frame f(rect.width(),rect.height());
        for(int y=0;y<rect.height();++y)
        {
            QRgb* scanline=(QRgb*)img.scanLine(y);
            for(int x=0;x<rect.width();++x)
            {
                f.setPixel(x,y,scanline[x]);
            }
        }
        if(d.windowMode())
        {
            for(uint x=0;x<f.width();x+=2)
            {
                for(uint y=0;y<f.height();y+=2)
                {
                    int c=0;
                    c+=f.pixelAt(x,y)!=qRgba(0,0,0,0);
                    c+=f.pixelAt(x+1,y)!=qRgba(0,0,0,0);
                    c+=f.pixelAt(x,y+1)!=qRgba(0,0,0,0);
                    c+=f.pixelAt(x+1,y+1)!=qRgba(0,0,0,0);
                    QRgb c2=(c>=(d.tolerance()))?color:qRgba(0,0,0,0);
                    for(uint xx=0;xx<=1;++xx)
                    {
                        for(uint yy=0;yy<=1;++yy)
                        {
                            f.pixelAt(x+xx,y+yy)=c2;
                        }
                    }
                }
            }
        }
        f.setDelay(d.time());
        text->clip->addFrame(f);
    }
    Embed* wrap=new Embed;
    wrap->clip=new Clip(SchWidget::WIDTH,SchWidget::HEIGHT);
    wrap->clip->clear();//ez jól tesz bele 1 frame-et
    wrap->clip->setName(basewrap.arg(nameidx));
    wrap->clip->addChild(text);
    wrap->x.setValue(0,0);
    wrap->y.setValue(0,0);
    wrap->t=0;
    wrap->z=-1;
    schWindow_->emit insertClip(wrap,schWindow_->clip_);
}

void SchWidget::circleUnHighLight()
{
    // Update old rubberband
    for(auto i=selectionList.constBegin();
        i!=selectionList.constEnd();i++)
    {
        if(*i&&((*i)->type()==PixelItem::TYPE))
        {
            unHighLightWindow(((PixelItem*)(*i))->getPos());
        }
    }
}

void SchWidget::circleHighLight()
{
    // Update new rubberband
    for(auto i=selectionList.constBegin();
        i!=selectionList.constEnd();i++)
    {
        if(*i&&((*i)->type()==PixelItem::TYPE))
        {
            highLightWindow(((PixelItem*)(*i))->getPos());
        }
    }
}

void SchWidget::lineUnHighLight()
{
    int xmin=INT_MAX;
    int ymin=INT_MAX;
    int xmax=INT_MIN;
    int ymax=INT_MIN;
    for(auto i=selectionList.constBegin();
        i!=selectionList.constEnd();i++)
    {
        if(*i&&((*i)->type()==PixelItem::TYPE))
        {
            if(((PixelItem*)(*i))->getPos().x()<xmin)
            {
                xmin=((PixelItem*)(*i))->getPos().x();
            }
            if(((PixelItem*)(*i))->getPos().x()>xmax)
            {
                xmax=((PixelItem*)(*i))->getPos().x();
            }
            if(((PixelItem*)(*i))->getPos().y()<ymin)
            {
                ymin=((PixelItem*)(*i))->getPos().y();
            }
            if(((PixelItem*)(*i))->getPos().y()>ymax)
            {
                ymax=((PixelItem*)(*i))->getPos().y();
            }
        }
    }
    //megkeressük a "sarokpont" okat
    QPoint veg[5];
    int vegek=0;
    for(auto i=selectionList.constBegin();
        i!=selectionList.constEnd();i++)
    {
        if(*i&&((*i)->type()==PixelItem::TYPE))
        {
            if(((((PixelItem*)(*i))->getPos().y()==ymax)||
                (((PixelItem*)(*i))->getPos().y()==ymin))&&
               ((((PixelItem*)(*i))->getPos().x()==xmax)||
                (((PixelItem*)(*i))->getPos().x()==xmin)))
            {
                veg[vegek++]=((PixelItem*)(*i))->getPos();
            }

        }
    }
    //Ha túl sok van, eliminálunk, legnagyobb távot keresünk
    if(vegek>2)
    {
        int tav=0;
        QPoint pos1,pos2;
        for(int i=0;i<vegek;++i)
            for(int j=i+1;j<vegek;++j)
            {
                if(qAbs(veg[i].x()-veg[j].x())+qAbs(veg[i].y()-veg[j].y())>tav)
                {
                    tav=qAbs(veg[i].x()-veg[j].x())+qAbs(veg[i].y()-veg[j].y());
                    pos1=veg[i];
                    pos2=veg[j];
                }
            }
        veg[0]=pos1;
        veg[1]=pos2;
    }

    if(vegek==1)
        unHighLightWindow(veg[0]);
        //És aztán vonalat interpolálunk a két végpont (veg[0] és veg[1]) közé.
    else if(vegek>1)
    {
        int x0=veg[0].x();
        int y0=veg[0].y();
        int x1=veg[1].x();
        int y1=veg[1].y();
        int dx=x1-x0;
        int dy=y1-y0;

        unHighLightWindow(veg[0]);
        if(qAbs(dx)>qAbs(dy))
        { // slope < 1
            float m=(float)dy/(float)dx;// compute slope
            float b=y0-m*x0;
            dx=(dx<0)?-1:1;
            while(x0!=x1)
            {
                x0+=dx;
                unHighLightWindow(QPoint(x0,qRound(m*x0+b)));
            }
        }
        else
            if(dy!=0)
        { // slope >= 1
            float m=(float)dx/(float)dy;// compute slope
            float b=x0-m*y0;
            dy=(dy<0)?-1:1;
            while(y0!=y1)
            {
                y0+=dy;
                unHighLightWindow(QPoint(qRound(m*y0+b),y0));
            }
        }

    }
}

void SchWidget::lineHighLight()
{
    int xmin=INT_MAX;
    int ymin=INT_MAX;
    int xmax=INT_MIN;
    int ymax=INT_MIN;
    for(auto i=selectionList.constBegin();
        i!=selectionList.constEnd();i++)
    {
        if(*i&&((*i)->type()==PixelItem::TYPE))
        {
            if(((PixelItem*)(*i))->getPos().x()<xmin)
            {
                xmin=((PixelItem*)(*i))->getPos().x();
            }
            if(((PixelItem*)(*i))->getPos().x()>xmax)
            {
                xmax=((PixelItem*)(*i))->getPos().x();
            }
            if(((PixelItem*)(*i))->getPos().y()<ymin)
            {
                ymin=((PixelItem*)(*i))->getPos().y();
            }
            if(((PixelItem*)(*i))->getPos().y()>ymax)
            {
                ymax=((PixelItem*)(*i))->getPos().y();
            }
        }
    }
    //megkeressük a "sarokpont" okat
    QPoint veg[5];
    int vegek=0;
    for(auto i=selectionList.constBegin();
        i!=selectionList.constEnd();i++)
    {
        if(*i&&((*i)->type()==PixelItem::TYPE))
        {
            if(((((PixelItem*)(*i))->getPos().y()==ymax)||
                (((PixelItem*)(*i))->getPos().y()==ymin))&&
               ((((PixelItem*)(*i))->getPos().x()==xmax)||
                (((PixelItem*)(*i))->getPos().x()==xmin)))
            {
                veg[vegek++]=((PixelItem*)(*i))->getPos();
            }

        }
    }
    //Ha túl sok van, eliminálunk, legnagyobb távot keresünk
    if(vegek>2)
    {
        int tav=0;
        QPoint pos1,pos2;
        for(int i=0;i<vegek;++i)
            for(int j=i+1;j<vegek;++j)
            {
                if(qAbs(veg[i].x()-veg[j].x())+qAbs(veg[i].y()-veg[j].y())>tav)
                {
                    tav=qAbs(veg[i].x()-veg[j].x())+qAbs(veg[i].y()-veg[j].y());
                    pos1=veg[i];
                    pos2=veg[j];
                }
            }
        veg[0]=pos1;
        veg[1]=pos2;
    }

    if(vegek==1)
        highLightWindow(veg[0]);
        //És aztán vonalat interpolálunk a két végpont (veg[0] és veg[1]) közé.
    else if(vegek>1)
    {
        int x0=veg[0].x();
        int y0=veg[0].y();
        int x1=veg[1].x();
        int y1=veg[1].y();
        int dx=x1-x0;
        int dy=y1-y0;

        highLightWindow(veg[0]);
        if(qAbs(dx)>qAbs(dy))
        { // slope < 1
            float m=(float)dy/(float)dx;// compute slope
            float b=y0-m*x0;
            dx=(dx<0)?-1:1;
            while(x0!=x1)
            {
                x0+=dx;
                highLightWindow(QPoint(x0,qRound(m*x0+b)));
            }
        }
        else
            if(dy!=0)
        { // slope >= 1
            float m=(float)dx/(float)dy;// compute slope
            float b=x0-m*y0;
            dy=(dy<0)?-1:1;
            while(y0!=y1)
            {
                y0+=dy;
                highLightWindow(QPoint(qRound(m*y0+b),y0));
            }
        }

    }
}

void SchWidget::drawSCHObject(QMouseEvent* event)
{
    circleBanding_=false;
    circleBandRect_=QRect();
    switch(tools_->activeTool())
    {
        case Tools::CIRCLE:
        {
            QSet<PixelItem*> pixels;
            QSet<PixelItem*> limitpixels;

            // Update old rubberband
            for(auto i=selectionList.constBegin();
                i!=selectionList.constEnd();i++)
            {
                if(*i&&((*i)->type()==PixelItem::TYPE))
                {
                    pixels.insert((PixelItem*)(*i));
                }
            }

            for(auto i=pixels.begin();i!=pixels.end();i++)
            {
                int x=(((PixelItem*)(*i))->getPos().x());
                int y=(((PixelItem*)(*i))->getPos().y());

                if(x-1>0&& !pixels.contains(matrix_->getPixel(QPoint(x-1,y))))
                {
                    limitpixels.insert((PixelItem*)(*i));
                }
                else if(x+1<w_&& !pixels.contains(matrix_->getPixel(QPoint(x+1,y))))
                {
                    limitpixels.insert((PixelItem*)(*i));
                }
                else if(y-1>0&& !pixels.contains(matrix_->getPixel(QPoint(x,y-1))))
                {
                    limitpixels.insert((PixelItem*)(*i));
                }
                else if(y+1<h_&& !pixels.contains(matrix_->getPixel(QPoint(x,y+1))))
                {
                    limitpixels.insert((PixelItem*)(*i));
                }
            }

            if(tools_->windowMode())
            {
                QSet<PixelItem*> limitpixels2;
                for(auto i=limitpixels.begin();i!=limitpixels.end();i++)
                {
                    int x=(((PixelItem*)(*i))->getPos().x());
                    int y=(((PixelItem*)(*i))->getPos().y());

                    //TODO: most nem páros*párosra elhasal

                    limitpixels2.insert(matrix_->getPixel(QPoint(x,y)));
                    limitpixels2.insert(matrix_->getPixel(QPoint(x^1,y)));
                    limitpixels2.insert(matrix_->getPixel(QPoint(x^1,y^1)));
                    limitpixels2.insert(matrix_->getPixel(QPoint(x,y^1)));
                }
                limitpixels=limitpixels2;
            }

            pixels.subtract(limitpixels);

            for(QSet<PixelItem*>::ConstIterator i=pixels.begin();i!=pixels.end();i++)
            {
                QRgb color=palette_->color(!dragFore);
                if(color==qRgba(0,0,0,0))continue;
                (*i)->setColor(palette_->color(!dragFore),true);
            }
            for(QSet<PixelItem*>::ConstIterator i=limitpixels.begin();i!=limitpixels.end();i++)
            {
                QRgb color=palette_->color(dragFore);
                if(color==qRgba(0,0,0,0))continue;
                (*i)->setColor(palette_->color(dragFore),true);
            }
        }
            break;
        case Tools::RECTANGLE:
        {
            int xmin=INT_MAX;
            int ymin=INT_MAX;
            int xmax=INT_MIN;
            int ymax=INT_MIN;
            for(auto i=selectionList.constBegin();
                i!=selectionList.constEnd();i++)
            {
                if(*i&&((*i)->type()==PixelItem::TYPE))
                {
                    if(((PixelItem*)(*i))->getPos().x()<xmin)
                    {
                        xmin=((PixelItem*)(*i))->getPos().x();
                    }
                    if(((PixelItem*)(*i))->getPos().x()>xmax)
                    {
                        xmax=((PixelItem*)(*i))->getPos().x();
                    }
                    if(((PixelItem*)(*i))->getPos().y()<ymin)
                    {
                        ymin=((PixelItem*)(*i))->getPos().y();
                    }
                    if(((PixelItem*)(*i))->getPos().y()>ymax)
                    {
                        ymax=((PixelItem*)(*i))->getPos().y();
                    }
                }
            }
            if(xmin<=xmax&&ymin<=ymax)
            {
                //Számoljunk kicsit:
                //Egy mátrixpixel szélessége
                qreal pix=matrix_->getPixel(QPoint(1,0))->scenePos().x()-
                        matrix_->getPixel(QPoint(0,0))->scenePos().x();

                //Kivarázsoljuk a téglalaphoz az adatokat
                QPoint &mp=mousePressViewPoint_;//Egérkattintás koordinátája
                QPoint ep=event->pos();//Elengedés koordinátája
                QRect crect=QRect(qMin(mp.x(),ep.x()),qMin(mp.y(),ep.y()),
                                  qAbs(mp.x()-ep.x())+1,qAbs(mp.y()-ep.y())+1);

                //A teljes kijelölés téglalapja
                QRectF rectband=QRectF(mapToScene(crect.topLeft()),
                                       mapToScene(crect.bottomRight()));
                //Egy téglalap, ami pont akkora és mint a rajzolandó
                QRectF rect(matrix_->getPixel(QPoint(xmin,ymin))->scenePos().x(),
                            matrix_->getPixel(QPoint(xmin,ymin))->scenePos().y(),
                            matrix_->getPixel(QPoint(xmax,ymax))->scenePos().x()-
                            matrix_->getPixel(QPoint(xmin,ymin))->scenePos().x()+pix,
                            matrix_->getPixel(QPoint(xmax,ymax))->scenePos().y()-
                            matrix_->getPixel(QPoint(xmin,ymin))->scenePos().y()+pix);


                //Két ablak vízszintes közelítő távolsága
                qreal pixh=3*pix;

                //Két pixel függőleges közelítő távolsága
                qreal pixv=5*pix;

                //Akkor nem kell szélet rajzolni a cuccnak, ha az adott
                //pixel a ház szélén van ÉS az adott szélen pixh vagy pixw
                //távolságnál messzebb van a circleBandRectScene_ - től

                //Alkotunk szabályokat, mikor kell margó
                bool hasTop=qAbs(rectband.topLeft().y()-rect.topLeft().y())<pixv;
                bool hasLeft=qAbs(rectband.topLeft().x()-rect.topLeft().x())<pixh;
                bool hasRight=qAbs(rectband.bottomRight().x()-rect.bottomRight().x())<pixh;
                bool hasBottom=qAbs(rectband.bottomRight().y()-rect.bottomRight().y())<pixv;

                for(auto i=selectionList.constBegin();
                    i!=selectionList.constEnd();i++)
                {
                    if(*i&&(*i)->type()==PixelItem::TYPE)
                    {
                        if(!((((PixelItem*)(*i))->getPos().x()==xmin&&hasLeft)||
                             (((PixelItem*)(*i))->getPos().x()==xmax&&hasRight)||
                             (((PixelItem*)(*i))->getPos().y()==ymin&&hasTop)||
                             (((PixelItem*)(*i))->getPos().y()==ymax&&hasBottom)))
                        {
                            QRgb color=palette_->color(!dragFore);
                            if(color==qRgba(0,0,0,0))continue;
                            ((PixelItem*)(*i))->setColor(palette_->color(!dragFore),true);
                        }
                    }

                }

                //TODO: Egyelőre ez is csak páros*párosra
                QPoint pos;
                for(auto i=selectionList.constBegin();
                    i!=selectionList.constEnd();i++)
                {
                    if(*i&&(*i)->type()==PixelItem::TYPE)
                    {
                        if((((PixelItem*)(*i))->getPos().x()==xmin&&hasLeft)||
                           (((PixelItem*)(*i))->getPos().x()==xmax&&hasRight)||
                           (((PixelItem*)(*i))->getPos().y()==ymin&&hasTop)||
                           (((PixelItem*)(*i))->getPos().y()==ymax&&hasBottom))
                        {
                            colorWindow(((PixelItem*)(*i))->getPos());
                        }
                    }

                }
            }
        }
            break;
        case Tools::LINE:
        {
            //Keresünk maxot meg mint
            int xmin=INT_MAX;
            int ymin=INT_MAX;
            int xmax=INT_MIN;
            int ymax=INT_MIN;
            for(auto i=selectionList.constBegin();
                i!=selectionList.constEnd();i++)
            {
                if(*i&&((*i)->type()==PixelItem::TYPE))
                {
                    if(((PixelItem*)(*i))->getPos().x()<xmin)
                    {
                        xmin=((PixelItem*)(*i))->getPos().x();
                    }
                    if(((PixelItem*)(*i))->getPos().x()>xmax)
                    {
                        xmax=((PixelItem*)(*i))->getPos().x();
                    }
                    if(((PixelItem*)(*i))->getPos().y()<ymin)
                    {
                        ymin=((PixelItem*)(*i))->getPos().y();
                    }
                    if(((PixelItem*)(*i))->getPos().y()>ymax)
                    {
                        ymax=((PixelItem*)(*i))->getPos().y();
                    }
                }
            }
            //megkeressük a "sarokpont" okat
            QPoint veg[5];
            int vegek=0;
            for(auto i=selectionList.constBegin();
                i!=selectionList.constEnd();i++)
            {
                if(*i&&((*i)->type()==PixelItem::TYPE))
                {
                    if(((((PixelItem*)(*i))->getPos().y()==ymax)||
                        (((PixelItem*)(*i))->getPos().y()==ymin))&&
                       ((((PixelItem*)(*i))->getPos().x()==xmax)||
                        (((PixelItem*)(*i))->getPos().x()==xmin)))
                    {
                        veg[vegek++]=((PixelItem*)(*i))->getPos();
                    }

                }
            }
            //Ha túl sok van, eliminálunk, legnagyobb távot keresünk
            if(vegek>2)
            {
                int tav=0;
                QPoint pos1,pos2;
                for(int i=0;i<vegek;++i)
                    for(int j=i+1;j<vegek;++j)
                    {
                        if(qAbs(veg[i].x()-veg[j].x())+qAbs(veg[i].y()-veg[j].y())>tav)
                        {
                            tav=qAbs(veg[i].x()-veg[j].x())+qAbs(veg[i].y()-veg[j].y());
                            pos1=veg[i];
                            pos2=veg[j];
                        }
                    }
                veg[0]=pos1;
                veg[1]=pos2;
            }

            if(vegek==1)
                colorWindow(veg[0]);
                //És aztán vonalat interpolálunk a két végpont (veg[0] és veg[1]) közé.
            else if(vegek>1)
            {
                int x0=veg[0].x();
                int y0=veg[0].y();
                int x1=veg[1].x();
                int y1=veg[1].y();
                int dx=x1-x0;
                int dy=y1-y0;

                colorWindow(veg[0]);
                if(qAbs(dx)>qAbs(dy))
                { // slope < 1
                    float m=(float)dy/(float)dx;// compute slope
                    float b=y0-m*x0;
                    dx=(dx<0)?-1:1;
                    while(x0!=x1)
                    {
                        x0+=dx;
                        colorWindow(QPoint(x0,qRound(m*x0+b)));
                    }
                }
                else
                    if(dy!=0)
                { // slope >= 1
                    float m=(float)dx/(float)dy;// compute slope
                    float b=x0-m*y0;
                    dy=(dy<0)?-1:1;
                    while(y0!=y1)
                    {
                        y0+=dy;
                        colorWindow(QPoint(qRound(m*y0+b),y0));
                    }
                }

            }

        }
            break;
        default:;
    }

    return;
}

