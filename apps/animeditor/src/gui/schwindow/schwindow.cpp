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

#include "schwindow.h"
#include "clip.h"
#include "schwidget.h"
#include "libmnp4/core/mctime.h"

Frame* SchWindow::clipboard_=NULL;

/**
 * Constructor for SchWindow (Constructs SCH based editor)
 */
SchWindow::SchWindow(SceneTree* scenetree,Palette* palette,Tools* tools,
                     Clip* clip,Mode mode,QWidget* parent/*=NULL*/)
    :QWidget(parent),sceneTree_(scenetree),
    audioPlayer_(mode==SCH_MODE?new AudioPlayer:NULL),clip_(clip),
    palette_(palette),tools_(tools),isToggled_(false),state_(IDLE),
    suppressTimelinePosChange_(false)
{
    timer_=new QTimer(this);
    timer_->setInterval(15);
    timer_->setSingleShot(true);

    layout_=new QGridLayout(this);
    layout_->setContentsMargins(0,0,0,0);

    timeline_=new TimeLine(this);
    layout_->addWidget(timeline_,1,0,1,1);

    playcontrols_=new PlayControls(this);
    layout_->addWidget(playcontrols_,2,0,1,1);

    if(clip->width()!=SchWidget::WIDTH||clip->height()!=SchWidget::HEIGHT)
    {
        mode=EDITOR_MODE;
    }

    if(mode==SCH_MODE)
    {
        schwidget_=new SchWidget(palette,tools_,this);
        layout_->addWidget(schwidget_,0,0,1,1);
    }
    else
    {
        int x=clip->width();
        int y=clip->height();

        schwidget_=new SchWidget(palette,tools_,x,y,this);
        layout_->addWidget(schwidget_,0,0,1,1);
    }

    connect(schwidget_,SIGNAL(clipDropped(int,int,Clip*)),
            this,SLOT(clipDropped(int,int,Clip*)));
    connect(timeline_,SIGNAL(valueChanged(qint64)),
            this,SLOT(positionChanged(qint64)));
    connect(playcontrols_,SIGNAL(lengthChanged(int)),
            this,SLOT(frameLengthChanged(int)));

    connect(playcontrols_,SIGNAL(addFrame()),this,SLOT(addFrame()));
    connect(playcontrols_,SIGNAL(wipeFrame()),this,SLOT(wipeFrame()));
    connect(playcontrols_,SIGNAL(copyFrame()),this,SLOT(copyFrame()));
    connect(playcontrols_,SIGNAL(delFrame()),this,SLOT(delFrame()));
    connect(playcontrols_,SIGNAL(toggleChanged(bool)),
            this,SLOT(toggleChanged(bool)));

    connect(schwidget_,SIGNAL(pixelSet(int,int,QRgb)),
            this,SLOT(pixelSet(int,int,QRgb)));

    connect(timer_,SIGNAL(timeout()),this,SLOT(playTick()));

    connect(playcontrols_,SIGNAL(play()),this,SLOT(play()));
    connect(playcontrols_,SIGNAL(stop()),this,SLOT(stop()));
    connect(playcontrols_,SIGNAL(next()),this,SLOT(next()));
    connect(playcontrols_,SIGNAL(previous()),this,SLOT(previous()));
    connect(playcontrols_,SIGNAL(nextKey()),this,SLOT(nextKey()));
    connect(playcontrols_,SIGNAL(prevKey()),this,SLOT(prevKey()));
    connect(playcontrols_,SIGNAL(first()),this,SLOT(first()));
    connect(playcontrols_,SIGNAL(last()),this,SLOT(last()));

    connect(timeline_,SIGNAL(valueChanged(qint64)),
            this,SLOT(timelinePosChanged(qint64)));

    //Set clip data
    update();
    clip_->setSchWindow(this);
}

SchWindow::~SchWindow()
{
    clip_->setSchWindow(NULL);
}

void SchWindow::setTimelinePos(qint64 pos,bool seek)
{
    timeline_->setPos(pos);
    if(seek)startedAt_=MCTime::getTimeMs()-pos;//lejátszás közben nem árt
    if(!seek)suppressTimelinePosChange_=true;
    if(audioPlayer_&&seek)audioPlayer_->seek(pos);
}

void SchWindow::clipDropped(int x,int y,Clip* clip)
{
    Embed* embed=new Embed;
    embed->clip=clip;
    embed->t=timeline_->getPosition();
    embed->x.setValue(0,x);
    embed->y.setValue(0,y);
    embed->z=-1;

    emit insertClip(embed,clip_);
}

/**
 * Pixel changed on the widget
 */
void SchWindow::pixelSet(int x,int y,QRgb color)
{
    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        Frame& frame=clip_->frame(index);
        frame.setPixel(x,y,color);
    }
    else
    {
        //Nop
    }

    refresh();
}

/**
 * This function may only be called after the clip length changed
 */
void SchWindow::update()
{
    timeline_->setRange(clip_->length());//
    playcontrols_->setFrameCount(clip_->getFrameCount());//

    refresh();
}

/**
 * This function may only be called if the clip length not changed!
 */
void SchWindow::refresh()
{
    int index=clip_->frameIndex(timeline_->getPosition());
    playcontrols_->setActiveFrame(index+(index>=0));
    //az új index, hátha ugrik
    index=clip_->frameIndex(timeline_->getPosition());

    if(index>=0)
    {
        playcontrols_->setFrameLength(clip_->frame(index).delay());
    }
    else
    {
        playcontrols_->setFrameLength(0);
    }

    Frame workFrame(clip_->width(),clip_->height());
    clip_->render(&workFrame,timeline_->getPosition(),!isToggled_);
    schwidget_->clear();
    schwidget_->showFrame(&workFrame);
}

void SchWindow::positionChanged(qint64)
{
    update();
}

void SchWindow::frameLengthChanged(int length)
{
    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        clip_->frame(index).setDelay(length);
        clip_->updateTree();
        update();
    }
    else
    {
        update();
    }

    int nextindex=clip_->frameIndex(timeline_->getPosition());

    if(index!=nextindex)
    {
        setTimelinePos(clip_->frameStart(index),true);
    }
}

void SchWindow::addFrame()
{
    Frame frame(clip_->width(),clip_->height());
    frame.setDelay(1000);

    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        clip_->addFrame(frame,index);
    }
    else
    {
        clip_->addFrame(frame);
    }

    update();
}

void SchWindow::copyFrame()
{
    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        clip_->addFrame(clip_->frame(index),index);
    }

    update();
}

void SchWindow::wipeFrame()
{
    int index=clip_->frameIndex(timeline_->getPosition());
    Frame& f=clip_->frame(index);
    f.shiftPixels(INT_MAX,INT_MAX);
}

void SchWindow::delFrame()
{
    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        clip_->removeFrame(index);
        if(clip_->getFrameCount()==0)
        {
            addFrame();
        }
    }
    else
    {
        //Nop
    }

    update();
}

void SchWindow::toggleChanged(bool toggle)
{
    isToggled_=toggle;

    refresh();
}

void SchWindow::timelinePosChanged(qint64 pos)
{
    if(!suppressTimelinePosChange_&&audioPlayer_)audioPlayer_->seek(pos);
    suppressTimelinePosChange_=false;
}

void SchWindow::play()
{
    int tpos=timeline_->getPosition();
    startedAt_=MCTime::getTimeMs()-tpos;
    if(audioPlayer_)
    {
        audioPlayer_->play(sceneTree_->metadata()->audio,false);
        audioPlayer_->seek(tpos);
        audioPlayer_->cont();
    }
    timer_->start();
    refresh();
}

void SchWindow::playTick()
{
    qint64 now=MCTime::getTimeMs();
    qint64 pos=now-startedAt_;
    if(audioPlayer_)startedAt_=now-audioPlayer_->pos();
    if(pos>=clip_->length())
    {
        setTimelinePos(clip_->length(),false);
        playcontrols_->stopPlay();
        stop();
    }
    else
    {
        setTimelinePos(pos,false);
        timer_->start();
    }
}

void SchWindow::stop()
{
    timer_->stop();
    if(audioPlayer_)audioPlayer_->stop();
    state_=IDLE;
}

void SchWindow::previous()
{
    int start=clip_->frameStart(clip_->frameIndex(timeline_->getPosition())-1);

    if(start==-1)
    {
        setTimelinePos(0,true);
    }
    else
    {
        setTimelinePos(start,true);
    }
}

void SchWindow::next()
{
    int start=clip_->frameStart(clip_->frameIndex(timeline_->getPosition())+1);

    if(start==-1)
    {
        setTimelinePos(clip_->frameStart(clip_->getFrameCount()-1)+
                       clip_->frame(clip_->getFrameCount()-1).delay()-1,true);
    }
    else
    {
        setTimelinePos(start,true);
    }
}

void SchWindow::prevKey()
{
    qDebug()<<clip_->prevKey(timeline_->getPosition());
    setTimelinePos(clip_->prevKey(timeline_->getPosition()),true);
}

void SchWindow::nextKey()
{
    qDebug()<<clip_->nextKey(timeline_->getPosition());
    setTimelinePos(clip_->nextKey(timeline_->getPosition()),true);
}

void SchWindow::first()
{
    setTimelinePos(0,true);
}

void SchWindow::last()
{
    setTimelinePos(clip_->frameStart(clip_->getFrameCount()-1)+
                   clip_->frame(clip_->getFrameCount()-1).delay()-1,true);
}

void SchWindow::copyToClipboard()
{
    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        if(clipboard_!=NULL)
        {
            delete clipboard_;
            clipboard_=NULL;
        }

        clipboard_=new Frame(clip_->frame(index));
    }
}

void SchWindow::pasteFromClipboard()
{
    int index=clip_->frameIndex(timeline_->getPosition());
    if(index>=0)
    {
        if(clipboard_==NULL)
        {
            QMessageBox::warning(this,tr("Error"),tr("Clipboard is empty."));
            return;
        }
        else if(clipboard_->width()!=clip_->width()||
                clipboard_->height()!=clip_->height())
        {
            QMessageBox::warning(this,tr("Error"),
                                 tr("Could not paste frame.\n"
                                    "The size of the frame on clipboard "
                                    "differs from the target clip's frame "
                                    "size."));
            return;
        }
        else
        {
            clip_->addFrame(Frame(*clipboard_),index);
            timeline_->setPos(clip_->frameStart(index+1));
        }
    }

    update();
}
