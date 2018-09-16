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

#include "qp4debug.h"

QP4Debug::QP4Debug()
{
    setWindowTitle(tr("QP4 Debug"));
    layout_=new QGridLayout(this);
    edit_=new QTextEdit(this);
    layout_->addWidget(edit_,0,0,1,1);
    connect(this,SIGNAL(finished(int)),this,SLOT(deleteLater()));
}

QP4Debug::~QP4Debug()
{
    qDebug()<<"~QP4Debug()";
}

void QP4Debug::show2()
{
    if(!text.isEmpty())
    {
        edit_->setText(text);
        show();
    }
}

void QP4Debug::debugLine(QString string)
{
    text.append(string);
}

void QP4Debug::deleteIfEmpty()
{
    if(text.isEmpty())
    {
        deleteLater();
    }
}
