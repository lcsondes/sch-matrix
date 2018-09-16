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

#include "dirview.h"

DirView::DirView(QWidget* parent/*=NULL*/)
    :QWidget(parent)
{
    layout_=new QHBoxLayout(this);

    model_.setRootPath(""); //windowson sajátgép, unixon /
    {
        QStringList filters;
        filters<<"*.q4x";
        model_.setNameFilters(filters);
        model_.setNameFilterDisables(false);
    }
    view_=new QTreeView(this);
    view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view_->setModel(&model_);
    layout_->addWidget(view_);
    add_=new QPushButton(QIcon(":icons/add.svg"),QString(),this);
    add_->setIconSize(QSize(32,32));
    add_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    layout_->addWidget(add_);

    connect(add_,SIGNAL(clicked()),this,SLOT(addClicked()));
}

void DirView::addClicked()
{
    QModelIndexList list=view_->selectionModel()->selectedIndexes();
    QStringList files;
    for(auto i=list.constBegin();i!=list.constEnd();++i)
    {
        files.append(model_.fileInfo(*i).absoluteFilePath());
    }
    files.removeDuplicates(); //FIXME a Qt doksi szerint nem kéne duplikátumot
                              //szűrni
    for(auto i=files.constBegin();i!=files.constEnd();++i)
    {
        emit fileAdded(*i);
    }
}
