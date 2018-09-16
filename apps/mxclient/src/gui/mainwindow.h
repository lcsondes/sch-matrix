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

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include "global.h"
#include "playcontrols.h"
#include "timeline.h"
#include "dirview.h"
#include "playlist.h"
#include "serverpane.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
    //main stuff
    QSplitter* splitter_;
    DirView* dirView_;
    PlayList* playlist_;

    //docks
    PlayControls* playControls_;
    QDockWidget* playControlsDock_;

    TimeLine* timeline_;
    QDockWidget* timelineDock_;

    ServerPane* serverPane_;
    QDockWidget* serverPaneDock_;
public:
    MainWindow();
};

#endif
