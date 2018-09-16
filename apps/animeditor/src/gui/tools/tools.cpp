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

#include "tools.h"
#include "mainwindow.h"

Tools::Tools(MainWindow* mw,QWidget* parent/*=NULL*/)
    :QWidget(parent),mainWindow_(mw)
{
    layout_=new QGridLayout(this);

    buttongroup_=new QButtonGroup(this);

#define MAKE_TOOL(name,icon,tooltip,enoom)\
do{name##_=new QPushButton(QIcon(icon),QString(),this);\
name##_->setToolTip(tooltip);\
name##_->setCheckable(true);\
buttongroup_->addButton(name##_,int(enoom));}while(0)
    MAKE_TOOL(pencil,":icons/pencil.svg",tr("Pencil tool"),PENCIL);
    //MAKE_TOOL(brush,":icons/brush.svg",tr("Brush tool"),BRUSH);
    MAKE_TOOL(line,":icons/line.svg",tr("Line tool"),LINE);
    MAKE_TOOL(fill,":icons/fill.svg",tr("Fill tool"),FILL);
    MAKE_TOOL(rectangle,":icons/rectangle.svg",tr("Rectangle tool"),RECTANGLE);
    MAKE_TOOL(circle,":icons/ellipse.svg",tr("Circle tool"),CIRCLE);
    MAKE_TOOL(text,":icons/text.svg",tr("Text tool"),TEXT);
#undef MAKE_TOOL

    windowmode_=new QPushButton(QIcon(":icons/pixelmode.svg"),QString(),this);
    isWindowmode_=true;
    windowModeClicked();

    uparrow_=new QPushButton(QIcon(":icons/moveup.svg"),QString(),this);
    uparrow_->setToolTip(tr("Move up"));
    downarrow_=new QPushButton(QIcon(":icons/movedown.svg"),QString(),this);
    downarrow_->setToolTip(tr("Move down"));
    leftarrow_=new QPushButton(QIcon(":icons/moveleft.svg"),QString(),this);
    leftarrow_->setToolTip(tr("Move left"));
    rightarrow_=new QPushButton(QIcon(":icons/moveright.svg"),QString(),this);
    rightarrow_->setToolTip(tr("Move right"));

    addkeyframe_=new QPushButton(QIcon(":icons/addkey.svg"),QString(),this);
    addkeyframe_->setToolTip(tr("Add keyframe"));
    removekeyframe_=new QPushButton(QIcon(":icons/delkey.svg"),QString(),this);
    removekeyframe_->setToolTip(tr("Remove keyframe"));
    moveframe_=new QPushButton(QIcon(":icons/paper.svg"),QString(),this);
    moveframe_->setToolTip(tr("Move drawn frame"));
    movechildren_=new QPushButton(QIcon(":icons/anim.svg"),QString(),this);
    movechildren_->setToolTip(tr("Move embedded clips"));

    windowmode_->setIconSize(QSize(24,24));
    pencil_->setIconSize(QSize(24,24));
    //brush_->setIconSize(QSize(24,24));
    line_->setIconSize(QSize(24,24));
    fill_->setIconSize(QSize(24,24));
    rectangle_->setIconSize(QSize(24,24));
    circle_->setIconSize(QSize(24,24));
    text_->setIconSize(QSize(24,24));

    uparrow_->setIconSize(QSize(24,24));
    downarrow_->setIconSize(QSize(24,24));
    leftarrow_->setIconSize(QSize(24,24));
    rightarrow_->setIconSize(QSize(24,24));

    addkeyframe_->setIconSize(QSize(24,24));
    removekeyframe_->setIconSize(QSize(24,24));
    moveframe_->setIconSize(QSize(24,24));
    movechildren_->setIconSize(QSize(24,24));

    moveframe_->setCheckable(true);
    movechildren_->setCheckable(true);

    pencil_->setChecked(true);
    moveframe_->setChecked(true);
    movechildren_->setChecked(true);

    layout_->addWidget(windowmode_,0,0,2,2);
    layout_->addWidget(pencil_,0,2,2,2);
    layout_->addWidget(line_,0,4,2,2);
    layout_->addWidget(fill_,0,6,2,2);
    layout_->addWidget(rectangle_,0,8,2,2);
    layout_->addWidget(circle_,0,10,2,2);
    layout_->addWidget(text_,0,12,2,2);

    layout_->addWidget(uparrow_,3,6,2,2);
    layout_->addWidget(leftarrow_,4,4,2,2);
    layout_->addWidget(rightarrow_,4,8,2,2);
    layout_->addWidget(downarrow_,5,6,2,2);

    layout_->addWidget(addkeyframe_,3,0,2,2);
    layout_->addWidget(removekeyframe_,5,0,2,2);
    layout_->addWidget(moveframe_,3,12,2,2);
    layout_->addWidget(movechildren_,5,12,2,2);

    connect(windowmode_,SIGNAL(clicked()),this,SLOT(windowModeClicked()));
    connect(moveframe_,SIGNAL(clicked()),this,SLOT(moveFramePressed()));
    connect(movechildren_,SIGNAL(clicked()),this,SLOT(moveChildrenPressed()));
    connect(uparrow_,SIGNAL(clicked()),this,SLOT(moveUp()));
    connect(downarrow_,SIGNAL(clicked()),this,SLOT(moveDown()));
    connect(leftarrow_,SIGNAL(clicked()),this,SLOT(moveLeft()));
    connect(rightarrow_,SIGNAL(clicked()),this,SLOT(moveRight()));
    connect(addkeyframe_,SIGNAL(clicked()),this,SLOT(addKey()));
    connect(removekeyframe_,SIGNAL(clicked()),this,SLOT(removeKey()));

}

Tools::Tool Tools::activeTool()
{
    return(Tool)buttongroup_->checkedId();
}

void Tools::moveFramePressed()
{
    if(!movechildren_->isChecked()&&!moveframe_->isChecked())
    {
        moveframe_->setChecked(true);
    }
}

void Tools::moveChildrenPressed()
{
    if(!movechildren_->isChecked()&&!moveframe_->isChecked())
    {
        movechildren_->setChecked(true);
    }
}

void Tools::move(int dx,int dy)
{
    SchWindow* win=mainWindow_->activeSubWindow();
    if(!win)return;
    Clip* clip=win->clip_;
    int t=win->timeline_->getPosition();
    if(moveframe_->isChecked())
    {
        int frameIdx=clip->frameIndex(t);
        if(frameIdx>=0)clip->frame(frameIdx).shiftPixels(dx,dy);
    }
    if(movechildren_->isChecked())
    {
        for(int i=0;i<clip->getChildCount();++i)
        {
            Embed* e=clip->child(i);
            e->x.setValue(t,e->x.value(t)+dx);
            e->y.setValue(t,e->y.value(t)+dy);
        }
    }
    win->update();
}

void Tools::addKey()
{
    move(0,0); //ez majd megoldja
}

void Tools::removeKey()
{
    SchWindow* win=mainWindow_->activeSubWindow();
    if(!win)return;
    Clip* clip=win->clip_;
    int t=win->timeline_->getPosition();
    for(int i=0;i<clip->getChildCount();++i)
    {
        Embed* e=clip->child(i);
        e->x.remove(t);
        e->y.remove(t);
    }
    win->update();
}

void Tools::moveUp()
{
    move(0,-2);
}

void Tools::moveDown()
{
    move(0,+2);
}

void Tools::moveLeft()
{
    move(-2,0);
}

void Tools::moveRight()
{
    move(+2,0);
}

void Tools::windowModeClicked()
{
    isWindowmode_=!isWindowmode_;
    windowmode_->setIcon(QIcon(isWindowmode_?":icons/windowmode.svg"
                                            :":icons/pixelmode.svg"));
    windowmode_->setToolTip(isWindowmode_?tr("Tools operate on windows")
                                         :tr("Tools operate on pixels"));
}

bool Tools::windowMode() const
{
    return isWindowmode_;
}
