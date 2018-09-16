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

const int TimeLine::MIN_DELAY;

TimeLine::TimeLine(QWidget* parent/*=NULL*/)
    :QWidget(parent),sliderPressed_(false)/*,suppressValueChanged_(false)*/
{
    //timeline 20 ms-es beosztása
    layout_=new QGridLayout(this);
    elapsed_=new QLabel(tr("0:00.000/?:??"),this);
    layout_->addWidget(elapsed_,0,0,1,1);

    //TODO kattintásra lehessen ugrani a timeline-on = WONTFIX
    slider_=new QSlider(Qt::Horizontal,this);
    slider_->setEnabled(true);
    slider_->setPageStep(10000/MIN_DELAY); //ha már nem megy oda, 10 mp-vel ugráljon
    layout_->addWidget(slider_,0,1,1,1);

    remaining_=new QLabel(tr("-?:??"));
    layout_->addWidget(remaining_,0,2,1,1);

    connect(slider_,SIGNAL(valueChanged(int)),this,SLOT(valueChanged0(int)));
    connect(slider_,SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
    connect(slider_,SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));
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
    /*if(!suppressValueChanged_)
    {
        emit valueChanged(value);
    }
    suppressValueChanged_=false;*/
    emit valueChanged(value*MIN_DELAY);
}

extern bool noendworkaround;

void TimeLine::setRange(qint64 end)
{
    end_=end/MIN_DELAY-!noendworkaround;
    if(slider_->value()>end_)
    {
        setPos(0);
    }
    else
    {
        setPos(getPosition());
    }
    slider_->setRange(0,end_);
}

void TimeLine::setPos(qint64 pos)
{
    pos/=MIN_DELAY;
    slider_->setValue(pos);
    elapsed_->setText(tr("%1/%2").arg(::hmsm(pos*MIN_DELAY),
                                      ::hms(end_*MIN_DELAY)));
    remaining_->setText(tr("-%1").arg(::hms((end_-pos)*MIN_DELAY)));
}

int TimeLine::getPosition()
{
    return slider_->value()*MIN_DELAY;
}
