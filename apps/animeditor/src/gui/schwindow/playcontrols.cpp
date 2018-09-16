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

#include "playcontrols.h"

PlayControls::PlayControls(QWidget* parent/*=NULL*/)
    :QWidget(parent),isPlaying_(false),hideChildren_(false),framecount_(-1),
    activeframe_(-1),playIcon_(":icons/play.svg"),stopIcon_(":icons/stop.svg")
{

    editlayout_=new QGridLayout();
    editlayout_->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    layout_=new QGridLayout();
    layout_->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    framecontrollayout_=new QGridLayout();
    framecontrollayout_->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    mainLayout_=new QGridLayout(this);
    mainLayout_->addLayout(editlayout_,0,0,Qt::AlignLeft|Qt::AlignVCenter);
    mainLayout_->addLayout(layout_,0,2,Qt::AlignCenter|Qt::AlignVCenter);
    mainLayout_->addLayout(framecontrollayout_,0,4,
                           Qt::AlignRight|Qt::AlignVCenter);

    toggle_=new QPushButton(QIcon(":icons/show.svg"),QString(),this);
    toggle_->setToolTip(tr("Children are shown"));
    toggle_->setIconSize(QSize(32,32));
    connect(toggle_,SIGNAL(clicked()),this,SLOT(toggleClicked()));
    editlayout_->addWidget(toggle_,0,0,1,1);

    delay_=new QLabel(tr("Delay:"),this);
    timeedit_=new QSpinBox(this);
    timeedit_->setMaximum(INT_MAX);
    timeedit_->setMinimum(20);
    timeedit_->setValue(20);
    timeedit_->setSuffix(tr(" ms"));
    timeedit_->setSingleStep(20);
    connect(timeedit_,SIGNAL(valueChanged(int)),this,SIGNAL(lengthChanged(int)));
    editlayout_->addWidget(delay_,0,1,1,1);
    editlayout_->addWidget(timeedit_,0,2,1,1);

    first_=new QPushButton(QIcon(":icons/first.svg"),"",this);
    first_->setToolTip(tr("Go to first frame"));
    connect(first_,SIGNAL(clicked()),this,SIGNAL(first()));
    layout_->addWidget(first_,0,0,1,1);

    prevKey_=new QPushButton(QIcon(":icons/prevkey.svg"),QString(),this);
    prevKey_->setToolTip(tr("Go to previous keyframe"));
    connect(prevKey_,SIGNAL(clicked()),this,SIGNAL(prevKey()));
    layout_->addWidget(prevKey_,0,1,1,1);

    prev_=new QPushButton(QIcon(":icons/prev.svg"),"",this);
    prev_->setToolTip(tr("Go back 1 frame"));
    connect(prev_,SIGNAL(clicked()),this,SIGNAL(previous()));
    layout_->addWidget(prev_,0,2,1,1);

    play_=new QPushButton(playIcon_,"",this);
    play_->setToolTip(tr("Play animation"));
    play_->setIconSize(QSize(32,32));

    connect(play_,SIGNAL(clicked()),this,SLOT(playClicked()));
    layout_->addWidget(play_,0,3,1,1);

    next_=new QPushButton(QIcon(":icons/next.svg"),"",this);
    next_->setToolTip(tr("Go forward 1 frame"));
    connect(next_,SIGNAL(clicked()),this,SIGNAL(next()));
    layout_->addWidget(next_,0,4,1,1);

    nextKey_=new QPushButton(QIcon(":icons/nextkey.svg"),QString(),this);
    nextKey_->setToolTip(tr("Go to next keyframe"));
    connect(nextKey_,SIGNAL(clicked()),this,SIGNAL(nextKey()));
    layout_->addWidget(nextKey_,0,5,1,1);

    last_=new QPushButton(QIcon(":icons/last.svg"),"",this);
    last_->setToolTip(tr("Go to last frame"));
    connect(last_,SIGNAL(clicked()),this,SIGNAL(last()));
    layout_->addWidget(last_,0,6,1,1);

    framecounter_=new QLabel("Frame: --/--",this);
    framecontrollayout_->addWidget(framecounter_,0,0,1,1);

    delframe_=new QPushButton(QIcon(":icons/delframe.svg"),"",this);
    delframe_->setToolTip(tr("Remove current frame"));
    connect(delframe_,SIGNAL(clicked()),this,SIGNAL(delFrame()));
    framecontrollayout_->addWidget(delframe_,0,1,1,1);

    /*addframe_=new QPushButton(QIcon(":icons/addframe.svg"),"",this);
    addframe_->setToolTip(tr("Add new frame after current"));
    connect(addframe_,SIGNAL(clicked()),this,SIGNAL(addFrame()));
    framecontrollayout_->addWidget(addframe_,0,3,1,1);*/

    wipeframe_=new QPushButton(QIcon(":icons/wipeframe.svg"),"",this);
    wipeframe_->setToolTip(tr("Wipe current frame"));
    connect(wipeframe_,SIGNAL(clicked()),this,SIGNAL(wipeFrame()));
    framecontrollayout_->addWidget(wipeframe_,0,3,1,1);

    copyframe_=new QPushButton(QIcon(":icons/copyframe.svg"),"",this);
    copyframe_->setToolTip(tr("Copy and insert current frame"));
    connect(copyframe_,SIGNAL(clicked()),this,SIGNAL(copyFrame()));
    framecontrollayout_->addWidget(copyframe_,0,2,1,1);

    delframe_->setIconSize(QSize(24,24));
    wipeframe_->setIconSize(QSize(24,24));
    copyframe_->setIconSize(QSize(24,24));

}

void PlayControls::playClicked()
{
    isPlaying_=!isPlaying_;
    play_->setIcon(isPlaying_?stopIcon_:playIcon_);
    emit isPlaying_?play():stop();
}

void PlayControls::toggleClicked()
{
    hideChildren_=!hideChildren_;
    if(hideChildren_)
    {
        toggle_->setToolTip(tr("Children are hidden"));
        toggle_->setIcon(QIcon(":icons/hide.svg"));
        emit toggleChanged(hideChildren_);
    }
    else
    {
        toggle_->setToolTip(tr("Children are shown"));
        toggle_->setIcon(QIcon(":icons/show.svg"));
        emit toggleChanged(hideChildren_);
    }
}

void PlayControls::stopPlay()
{
    isPlaying_=false;
    play_->setIcon(playIcon_);
}

void PlayControls::startPlay()
{
    isPlaying_=true;
    play_->setIcon(stopIcon_);
}

void PlayControls::setFrameCount(int count)
{
    framecount_=count;
    displayFrame();
}

void PlayControls::setActiveFrame(int frame)
{
    activeframe_=frame;
    displayFrame();
}

void PlayControls::displayFrame()
{
    QString label(tr("Frame: %1/%2"));

    if(activeframe_<0||activeframe_>framecount_)
    {
        label=label.arg(tr("--"));
    }
    else
    {
        label=label.arg(QString::number(activeframe_));
    }

    if(framecount_<0)
    {
        label=label.arg(tr("--"));
    }
    else
    {
        label=label.arg(QString::number(framecount_));
    }

    framecounter_->setText(label);
}

void PlayControls::setFrameLength(int length)
{
    timeedit_->setValue(length);
}

