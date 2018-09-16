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

#include "textrenderdialog.h"

TextRenderDialog::TextRenderDialog(QWidget* parent/*=NULL*/)
    :QDialog(parent)
{
    setWindowTitle(tr("Insert text"));
    layout_=new QGridLayout(this);

    fontLabel_=new QLabel(tr("Font:"),this);
    fontName_=new QLineEdit(this);
    fontName_->setEnabled(false);
    changeFont_=new QPushButton(tr("Change..."),this);
    textEdit_=new QLineEdit(tr("Enter text here"),this);
    timeLabel_=new QLabel(tr("Length:"),this);
    timeEdit_=new QSpinBox(this);
    timeEdit_->setMinimum(1);
    timeEdit_->setMaximum(INT_MAX);
    timeEdit_->setValue(5000);
    timeEdit_->setSuffix(tr(" ms"));
    windowMode_=new QCheckBox(tr("Draw to windows, not pixels"),this);
    tolerance_=new QLabel(tr("Tolerance:"),this);
    tolEdit_=new QSpinBox(this);
    tolEdit_->setMinimum(0);
    tolEdit_->setMaximum(4); //ennyi pixel van egy ablakban
    tolEdit_->setValue(1);
    tolerance_->setEnabled(false);
    tolEdit_->setEnabled(false);
    buttons_=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                  Qt::Horizontal,this);

    connect(changeFont_,SIGNAL(clicked()),this,SLOT(changeFont()));
    connect(buttons_,SIGNAL(accepted()),this,SLOT(accept()));
    connect(buttons_,SIGNAL(rejected()),this,SLOT(reject()));

    connect(windowMode_,SIGNAL(clicked(bool)),tolerance_,SLOT(setEnabled(bool)));
    connect(windowMode_,SIGNAL(clicked(bool)),tolEdit_,SLOT(setEnabled(bool)));

    layout_->addWidget(fontLabel_,0,0,1,1);
    layout_->addWidget(fontName_,0,1,1,1);
    layout_->addWidget(changeFont_,0,2,1,1);
    layout_->addWidget(textEdit_,1,0,1,3);
    layout_->addWidget(timeLabel_,2,0,1,1);
    layout_->addWidget(timeEdit_,2,1,1,2);
    layout_->addWidget(windowMode_,3,0,1,3);
    layout_->addWidget(tolerance_,4,0,1,1);
    layout_->addWidget(tolEdit_,4,1,1,2);
    layout_->addWidget(buttons_,5,0,1,3);

    font_.setPointSize(16);
    font_.setBold(true);
    setFontName();
}

void TextRenderDialog::setFontName()
{
    fontName_->setText(tr("%1 %2 pt").arg(font_.family()).
                                      arg(font_.pointSize()));
}

void TextRenderDialog::changeFont()
{
    font_=QFontDialog::getFont(NULL,font_,this,tr("Choose a font"));
    setFontName();
}

QFont TextRenderDialog::font() const
{
    return font_;
}

QString TextRenderDialog::text() const
{
    return textEdit_->text();
}

int TextRenderDialog::time() const
{
    return timeEdit_->value();
}

bool TextRenderDialog::windowMode() const
{
    return windowMode_->checkState()==Qt::Checked;
}

int TextRenderDialog::tolerance() const
{
    return 4-tolEdit_->value();
}
