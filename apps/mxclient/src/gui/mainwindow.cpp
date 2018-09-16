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

#include "mainwindow.h"
#include "libmnp4/network/mnclient.h"

MainWindow::MainWindow()
{
    setWindowTitle(tr("Matrix Player 2012 shitty edition"));
    splitter_=new QSplitter(this);
    //splitter_->setStyle(new QPlastiqueStyle); //TODO memleak
    setCentralWidget(splitter_);

    //docks
    playControlsDock_=new QDockWidget(tr("PlayControls"),this);
    playControlsDock_->setFeatures(QDockWidget::DockWidgetMovable|
                                   QDockWidget::DockWidgetFloatable);
    playControls_=new PlayControls(playControlsDock_);
    playControlsDock_->setWidget(playControls_);
    addDockWidget(Qt::TopDockWidgetArea,playControlsDock_);

    timelineDock_=new QDockWidget(tr("Timeline"),this);
    timelineDock_->setFeatures(QDockWidget::DockWidgetMovable|
                               QDockWidget::DockWidgetFloatable);
    timeline_=new TimeLine(timelineDock_);
    timelineDock_->setWidget(timeline_);
    addDockWidget(Qt::BottomDockWidgetArea,timelineDock_);

    serverPaneDock_=new QDockWidget(tr("ServerPane"),this);
    serverPaneDock_->setFeatures(QDockWidget::DockWidgetMovable|
                                 QDockWidget::DockWidgetFloatable);
    serverPane_=new ServerPane(serverPaneDock_);
    serverPaneDock_->setWidget(serverPane_);
    addDockWidget(Qt::TopDockWidgetArea,serverPaneDock_);

    dirView_=new DirView(this);
    splitter_->addWidget(dirView_);

    playlist_=new PlayList(playControls_,timeline_,this);
    splitter_->addWidget(playlist_);

    connect(dirView_,SIGNAL(fileAdded(QString)),
            playlist_,SLOT(addFile(QString)));

    connect(serverPane_,SIGNAL(connected(QHostAddress)),
            playlist_,SLOT(clientConnected()));

    show();
}
