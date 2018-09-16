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

#include "childsettingsdialog.h"

ChildSettingsDialog::ChildSettingsDialog(Clip::Embed* embed,QWidget* parent):
    QDialog(parent),embed_(embed)
{
    setWindowTitle(tr("Embed settings"));
    
    layout_=new QGridLayout(this);
    layout_->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    btnlayout_=new QGridLayout();
    btnlayout_->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

    buttons_=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                  Qt::Horizontal,this);
    connect(buttons_,SIGNAL(accepted()),this,SLOT(accept()));
    connect(buttons_,SIGNAL(rejected()),this,SLOT(reject()));

    btnlayout_->addWidget(buttons_,0,0,1,1);

    timelabel_=new QLabel(this);
    timelabel_->setText(tr("Start time:"));
    layout_->addWidget(timelabel_,0,0,1,1);

    timeedit_=new QSpinBox(this);
    timeedit_->setMinimum(0);
    timeedit_->setMaximum(INT_MAX);
    timeedit_->setValue(embed_->t);
    layout_->addWidget(timeedit_,1,0,1,1);

    xlabel_=new QLabel(this);
    xlabel_->setText(tr("X coordinate:"));
    layout_->addWidget(xlabel_,2,0,1,1);

    xedit_=new QSpinBox(this);
    xedit_->setMinimum(INT_MIN);
    xedit_->setMaximum(INT_MAX);
    xedit_->setValue(embed_->x.value(0));
    layout_->addWidget(xedit_,3,0,1,1);

    ylabel_=new QLabel(this);
    ylabel_->setText(tr("Y coordinate:"));
    layout_->addWidget(ylabel_,4,0,1,1);

    yedit_=new QSpinBox(this);
    yedit_->setMinimum(INT_MIN);
    yedit_->setMaximum(INT_MAX);
    yedit_->setValue(embed_->y.value(0));
    layout_->addWidget(yedit_,5,0,1,1);

    layout_->addLayout(btnlayout_,6,0,1,1);
}

int ChildSettingsDialog::getTime()
{
    return timeedit_->value();
}

int ChildSettingsDialog::getX()
{
    return xedit_->value();
}

int ChildSettingsDialog::getY()
{
    return yedit_->value();
}
