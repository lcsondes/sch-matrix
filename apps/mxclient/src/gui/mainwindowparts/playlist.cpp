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

#include "playlist.h"

PlayList::PlayList(PlayControls* pc,TimeLine* tl,QWidget* parent/*=NULL*/)
    :QWidget(parent),playcontrols_(pc),player_(NULL)
{
    connect(pc,SIGNAL(prev())    ,this,SLOT(prev()));
    connect(pc,SIGNAL(play())    ,this,SLOT(play()));
    connect(pc,SIGNAL(stop())    ,this,SLOT(stop()));
    connect(pc,SIGNAL(pauseNow()),this,SLOT(pauseContNow()));
    connect(pc,SIGNAL(next())    ,this,SLOT(next()));

    layout_=new QGridLayout(this);

    list_=new QTreeView(this);
    list_->setModel(&model_);
    layout_->addWidget(list_,0,0,7,1);

    connect(list_,SIGNAL(doubleClicked(const QModelIndex&)),
            &model_,SLOT(setNext(const QModelIndex&)));

    up_=new QPushButton(QIcon(":icons/up.svg"),QString(),this);
    up_->setIconSize(QSize(32,32));
    layout_->addWidget(up_,1,1,1,1);

    remove_=new QPushButton(QIcon(":icons/remove.svg"),QString(),this);
    remove_->setIconSize(QSize(32,32));
    layout_->addWidget(remove_,3,1,1,1);

    down_=new QPushButton(QIcon(":icons/down.svg"),QString(),this);
    down_->setIconSize(QSize(32,32));
    layout_->addWidget(down_,5,1,1,1);

    connect(up_    ,SIGNAL(clicked()),this,SLOT(moveUp()));
    connect(down_  ,SIGNAL(clicked()),this,SLOT(moveDown()));
    connect(remove_,SIGNAL(clicked()),this,SLOT(remove()));

    player_=new Player;

    connect(player_,SIGNAL(animFinished()),this,SLOT(currentAnimFinished()));

    connect(player_,SIGNAL(setStatus(QString)),tl,SLOT(setStatus(QString)));
    connect(player_,SIGNAL(setRange(qint64))  ,tl,SLOT(setRange(qint64)));
    connect(player_,SIGNAL(setPos(qint64))    ,tl,SLOT(setPos(qint64)));

    connect(tl,SIGNAL(valueChanged(qint64)),player_,SLOT(seek(qint64)));
}

void PlayList::clientConnected()
{
    player_->start();
}

PlayList::~PlayList()
{
    delete player_;
}

void PlayList::addFile(QString file)
{
    model_.addFile(file);
}

void PlayList::moveUp()
{
    QModelIndexList selected=list_->selectionModel()->selectedIndexes();
    if(selected.isEmpty())return;
    model_.moveUp(selected[0].row());
}

void PlayList::moveDown()
{
    QModelIndexList selected=list_->selectionModel()->selectedIndexes();
    if(selected.isEmpty())return;
    model_.moveDown(selected[0].row());
}

void PlayList::remove()
{
    QModelIndexList selected=list_->selectionModel()->selectedIndexes();
    if(selected.isEmpty())return;
    model_.remove(selected[0].row());
}

void PlayList::prev()
{
    model_.moveNextBackward();
}

void PlayList::next()
{
    model_.moveNextForward();
}

void PlayList::play()
{
    model_.currentAnimEnded();
    model_.selectNextAnim();
    //sokáig tart, másik szálon hívjuk meg
    Q_ASSERT(player_);
    QMetaObject::invokeMethod(player_,"play",Qt::QueuedConnection,
                              Q_ARG(QString,model_.currentFile()),
                              Q_ARG(quint32,model_.currentLength()));
}

void PlayList::stop()
{
    player_->stop();
    model_.rewindCurrentAnim();
}

void PlayList::pauseContNow()
{
    player_->pauseCont();
}

void PlayList::currentAnimFinished()
{
    if(!playcontrols_->isPauseNext())
    {
        play();
    }
}
