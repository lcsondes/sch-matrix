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

#include "safebutton.h"

void SafeButton::ctor()
{
    setFrameShape(QFrame::StyledPanel);
    layout_=new QHBoxLayout(this);
    layout_->setContentsMargins(1,1,1,1);
    cb_=new QCheckBox;
    cb_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
    layout_->addWidget(cb_);
    layout_->addWidget(pb_);
    pb_->setEnabled(false);
    //kihasználom, hogy a qpushbutton false értékkel nyomódik meg
    connect(cb_,SIGNAL(clicked(bool)),pb_,SLOT(setEnabled(bool)));
    connect(pb_,SIGNAL(clicked(bool)),cb_,SLOT(setChecked(bool)));
    connect(pb_,SIGNAL(clicked(bool)),pb_,SLOT(setEnabled(bool)));
    connect(pb_,SIGNAL(clicked()),this,SIGNAL(clicked()));
}

SafeButton::SafeButton(QString text,QWidget* parent/*=NULL*/)
    :QFrame(parent)
{
    pb_=new QPushButton(text,this);
    ctor();
}

SafeButton::SafeButton(QIcon icon,QWidget* parent/*=NULL*/)
    :QFrame(parent)
{
    pb_=new QPushButton(icon,QString(),this);
    ctor();
}

void SafeButton::setIconSize(const QSize& size)
{
    pb_->setIconSize(size);
}
