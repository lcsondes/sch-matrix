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

#include "selectedcolors.h"
#include "palette.h"

SelectedColors::SelectedColors(QWidget* parent/*=NULL*/)
    :QFrame(parent)
{
    foreground_=new ColorSwatch(this);
    foreground_->setToolTip(tr("Main color"));
    background_=new ColorSwatch(this);
    background_->setToolTip(tr("Background color"));
    background_->stackUnder(foreground_);

    foreground_->setColor(qRgba(255,255,255,255));
    background_->setColor(qRgba(0,0,0,0));

    connect(foreground_,SIGNAL(clicked(QRgb)),
            this,SLOT(foregroundSelected()));
    connect(background_,SIGNAL(clicked(QRgb)),
            this,SLOT(backgroundSelected()));

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    int minsize=ColorSwatch::SIZE*3;
    setMinimumSize(minsize,minsize);
    foregroundSelected();
}

/**
 * \return true, ha az előtér színe van kiválasztva
 */
bool SelectedColors::isForegroundSelected() const
{
    return foregroundSelected_;
}

/**
 * Ki lett választva az előtér-szín
 */
void SelectedColors::foregroundSelected()
{
    foregroundSelected_=true;
    foreground_->setFrameStyle(QFrame::Box);
    background_->setFrameStyle(QFrame::Panel);
    background_->setFrameShadow(QFrame::Sunken);
    update();
}

/**
 * Ki lett választva a háttérszín
 */
void SelectedColors::backgroundSelected()
{
    foregroundSelected_=false;
    foreground_->setFrameStyle(QFrame::Panel);
    foreground_->setFrameShadow(QFrame::Sunken);
    background_->setFrameStyle(QFrame::Box);
    update();
}

/**
 * Négyzet-alakúan tartja magát.
 */
void SelectedColors::resizeEvent(QResizeEvent*)
{
    if(width()!=height())
    {
        int size=std::min(width(),height());
        resize(size,size);
        return;
    }
    foreground_->move(0,0);
    foreground_->resize(size()*2/3);
    QSize bkpos_tmp=size()/3;
    background_->move(bkpos_tmp.width(),bkpos_tmp.height());
    background_->resize(size()*2/3);
}

/**
 * Az épp kiválasztott színt lecseréli a megadottra.
 */
void SelectedColors::setSelected(QRgb c)
{
    if(foregroundSelected_)
    {
        setForeground(c);
    }
    else
    {
        setBackground(c);
    }
}

QRgb SelectedColors::foreground() const
{
    return foreground_->color();
}

QRgb SelectedColors::background() const
{
    return background_->color();
}

QRgb SelectedColors::color(bool fore) const
{
    return fore?foreground_->color():background_->color();
}

void SelectedColors::setForeground(QRgb c)
{
    foreground_->setColor(c);
}

void SelectedColors::setBackground(QRgb c)
{
    background_->setColor(c);
}
