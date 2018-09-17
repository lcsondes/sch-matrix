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
#include "schwidget.h"
#include "schwindow.h"
#include "timeline.h"
#include "playcontrols.h"
#include "palette.h"
#include "scenetree.h"
#include "scenetreepanel.h"
#include "tools.h"
#include "clipmdisubwindow.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
    QFileInfo currentQp4_;
    QTimer* epichax_;

    QMdiArea* mdi_;
    SchWindow* main_;
    QMenu* fileMenu_;
    QAction* newAction_;
    QAction* openAction_;
    QAction* qpaAction_;
    QAction* audioAction_;
    QAction* saveAction_;
    QAction* saveAsAction_;
    QAction* exportAction_;
    QAction* exitAction_;
    QMenu* editMenu_;
    QAction* editMetaAction_;
    QAction* copyFrame_;
    QAction* pasteFrame_;
    QMenu* helpMenu_;
    QAction* aboutAction_;
    Palette* palette_;
    QDockWidget* paletteDock_;
    Tools* tools_;
    QDockWidget* toolsDock_;
    SceneTreePanel* sceneTreePanel_;
    SceneTree* sceneTree_;
    QDockWidget* sceneTreeDock_;

    void closeEvent(QCloseEvent*) override;
    void saveCore(QString);
public:
    MainWindow();
    ~MainWindow();
    SchWindow* activeSubWindow();
public slots:
    void updateAllWindows();
    
    void about();
    void editMeta();
    void copyFrame();
    void pasteFrame();

    bool closeCurrent(); //true: user saved or discarded animation
    void newClicked();
    void openClicked();
    void openQpaClicked();
    void openAudioClicked();

    void saveClicked();
    void saveAsClicked();
    void exportClicked();

    void addMdiWindow(Clip*);
    //a 0. oszlopból szerez pointert - az alapján dolgozik
    void addMdiWindow(QTreeWidgetItem*);
};

#endif
