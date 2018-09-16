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

#include "scenetreepanel.h"

SceneTreePanel::SceneTreePanel(QWidget* parent/*=NULL*/)
    :QWidget(parent)
{
    layout_=new QGridLayout(this);

    scenetree_=new SceneTree(this);
    layout_->addWidget(scenetree_,0,0,8,1);

    up_=new QPushButton(QIcon(":icons/up.svg"),QString(),this);
    up_->setIconSize(QSize(32,32));
    up_->setToolTip(tr("Bring clip forward in Z order"));
    layout_->addWidget(up_,1,1,1,1);

    add_=new QPushButton(QIcon(":icons/newanim.svg"),QString(),this);
    add_->setIconSize(QSize(32,32));
    add_->setToolTip(tr("Add new clip to selected clip"));
    layout_->addWidget(add_,3,1,1,1);

    remove_=new QPushButton(QIcon(":icons/delanim.svg"),QString(),this);
    remove_->setIconSize(QSize(32,32));
    remove_->setToolTip(tr("Remove selected clip"));
    layout_->addWidget(remove_,4,1,1,1);

    down_=new QPushButton(QIcon(":icons/down.svg"),QString(),this);
    down_->setIconSize(QSize(32,32));
    down_->setToolTip(tr("Move clip backward in Z order"));
    layout_->addWidget(down_,6,1,1,1);

    connect(up_,SIGNAL(clicked()),scenetree_,SLOT(moveUp()));
    connect(down_,SIGNAL(clicked()),scenetree_,SLOT(moveDown()));
    connect(add_,SIGNAL(clicked()),scenetree_,SLOT(newChildItemBtn()));
    connect(remove_,SIGNAL(clicked()),scenetree_,SLOT(removeItemBtn()));
}

SceneTree* SceneTreePanel::getSceneTree()
{
    return scenetree_;
}
