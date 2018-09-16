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

#include "newclipdialog.h"
#include "schwidget.h"
#include "scenetree.h"

NewClipDialog::NewClipDialog(SceneTree* st,QWidget* parent/*=NULL*/)
: QDialog(parent)
{
    setWindowTitle(tr("New clip"));

    layout_=new QGridLayout(this);
    formLayout_=new QFormLayout;
    QString namebase=tr("New clip %1");
    int i=0;
    while(1)
    {
        ++i;
        if(st->checkName(namebase.arg(i),st->root()->clip.data()))
        {
            break;
        }
    }
    nameEdit_=new QLineEdit(namebase.arg(i),this);
    widthEdit_=new QSpinBox(this);
    heightEdit_=new QSpinBox(this);
    ok_=new QPushButton(tr("OK"),this);
    widthEdit_->setRange(2,1000);
    heightEdit_->setRange(2,1000);
    widthEdit_->setValue(SchWidget::WIDTH);
    heightEdit_->setValue(SchWidget::HEIGHT);

    formLayout_->addRow(tr("Name:"),nameEdit_);
    formLayout_->addRow(tr("Width:"),widthEdit_);
    formLayout_->addRow(tr("Height:"),heightEdit_);

    layout_->addLayout(formLayout_,0,0);
    layout_->addWidget(ok_,1,0,1,1);

    connect(ok_,SIGNAL(clicked()),this,SLOT(accept()));
}

NewClipDialog::~NewClipDialog()
{
    delete formLayout_;
}

QString NewClipDialog::name()
{
    return nameEdit_->text();
}

int NewClipDialog::width()
{
    return widthEdit_->value();
}

int NewClipDialog::height()
{
    return heightEdit_->value();
}
