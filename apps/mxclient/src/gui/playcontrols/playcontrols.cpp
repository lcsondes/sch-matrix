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
    :QWidget(parent)
{
    layout_=new QHBoxLayout(this);
    layout_->setContentsMargins(0,0,0,0);
    prev_=new QPushButton(QIcon(":icons/prev.svg"),QString(),this);
    prev_->setIconSize(QSize(32,32));
    layout_->addWidget(prev_);

    play_=new SafeButton(QIcon(":icons/play.svg"),this);
    play_->setIconSize(QSize(32,32));
    layout_->addWidget(play_);

    stop_=new SafeButton(QIcon(":icons/stop.svg"),this);
    stop_->setIconSize(QSize(32,32));
    layout_->addWidget(stop_);

    pauseNow_=new SafeButton(QIcon(":icons/pausenow.svg"),this);
    pauseNow_->setIconSize(QSize(32,32));
    layout_->addWidget(pauseNow_);

    pauseNext_=new QPushButton(QIcon(":icons/pause.svg"),QString(),this);
    pauseNext_->setIconSize(QSize(32,32));
    pauseNext_->setCheckable(true);
    layout_->addWidget(pauseNext_);
    
    next_=new QPushButton(QIcon(":icons/next.svg"),QString(),this);
    next_->setIconSize(QSize(32,32));
    layout_->addWidget(next_);

    connect(prev_    ,SIGNAL(clicked()),this,SIGNAL(prev()));
    connect(play_    ,SIGNAL(clicked()),this,SIGNAL(play()));
    connect(stop_    ,SIGNAL(clicked()),this,SIGNAL(stop()));
    connect(pauseNow_,SIGNAL(clicked()),this,SIGNAL(pauseNow()));
    connect(next_    ,SIGNAL(clicked()),this,SIGNAL(next()));
}

bool PlayControls::isPauseNext()
{
    return pauseNext_->isChecked();
}
