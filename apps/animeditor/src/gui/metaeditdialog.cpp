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

#include "metaeditdialog.h"

MetaEditDialog::MetaEditDialog(SceneTree* st,QWidget* parent/*=NULL*/)
    :QDialog(parent)
{
    setWindowTitle(tr("Animation tags"));
    
    layout_=new QGridLayout(this);
    formLayout_=new QFormLayout;

    teamEdit_=new QLineEdit(st->metadata()->team,this);
    titleEdit_=new QLineEdit(st->metadata()->title,this);
    yearEdit_=new QSpinBox(this);
    yearEdit_->setMinimum(1972);
    yearEdit_->setMaximum(INT_MAX);
    yearEdit_->setValue(st->metadata()->year);

    ok_=new QPushButton(tr("OK"),this);
    connect(ok_,SIGNAL(clicked()),this,SLOT(accept()));

    formLayout_->addRow(tr("Team:"),teamEdit_);
    formLayout_->addRow(tr("Title:"),titleEdit_);
    formLayout_->addRow(tr("Year:"),yearEdit_);

    layout_->addLayout(formLayout_,0,0);
    layout_->addWidget(ok_,1,0,1,1);
}

MetaEditDialog::~MetaEditDialog()
{
    delete formLayout_;
}

QString MetaEditDialog::team()
{
    return teamEdit_->text();
}

QString MetaEditDialog::title()
{
    return titleEdit_->text();
}

int MetaEditDialog::year()
{
    return yearEdit_->value();
}
