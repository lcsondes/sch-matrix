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

#include "timeline.h"

TimeLine::TimeLine(QWidget* parent/*=NULL*/)
    :QWidget(parent),sliderPressed_(false),suppressValueChanged_(false)
{
    layout_=new QGridLayout(this);
    elapsed_=new QLabel(this);
    layout_->addWidget(elapsed_,0,0,1,1);

    unlocker_=new QCheckBox(this);
    layout_->addWidget(unlocker_,0,1,1,1);

    slider_=new QSlider(Qt::Horizontal,this);
    slider_->setEnabled(false);
    slider_->setTracking(false);
    layout_->addWidget(slider_,0,2,1,1);

    connect(unlocker_,SIGNAL(clicked(bool)),slider_,SLOT(setEnabled(bool)));
    connect(slider_,SIGNAL(valueChanged(int)),this,SLOT(valueChanged0(int)));
    connect(slider_,SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
    connect(slider_,SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));

    remaining_=new QLabel(this);
    layout_->addWidget(remaining_,0,3,1,1);

    status_=new QLabel(this);
    layout_->addWidget(status_,1,0,1,4);
}

void TimeLine::sliderPressed()
{
    sliderPressed_=true;
}

void TimeLine::sliderReleased()
{
    sliderPressed_=false;
}

void TimeLine::valueChanged0(int value)
{
    if(!suppressValueChanged_)
    {
        emit valueChanged(value);
    }
    suppressValueChanged_=false;
}

void TimeLine::setStatus(QString status)
{
    status_->setText(status);
}

void TimeLine::setRange(qint64 end)
{
    end_=end;
    slider_->setRange(0,end);
    setPos(0);
}

void TimeLine::setPos(qint64 pos)
{
    if(!sliderPressed_)
    {
        suppressValueChanged_=true;
        slider_->setValue(pos);
    }
    elapsed_->setText(tr("%1/%2").arg(::hms(pos),::hms(end_)));
    remaining_->setText(tr("-%1").arg(::hms(end_-pos)));
}
