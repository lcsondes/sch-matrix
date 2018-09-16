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

#ifndef TOOLS_H
#define	TOOLS_H

#include "global.h"

//TODO del keyframe gomb csak keyframe-en legyen aktív

class MainWindow;

/**
 * Az "eszköztár"
 */
class Tools : public QWidget
{
    Q_OBJECT
public:
    enum Tool
    {
        PENCIL=1,
        //BRUSH,
        LINE,
        FILL,
        RECTANGLE,
        CIRCLE,
        TEXT
    };
private:

    MainWindow* mainWindow_;

    QGridLayout* layout_;
    QButtonGroup* buttongroup_;
    bool isWindowmode_;
    QPushButton* windowmode_;
    QPushButton* pencil_;
    //QPushButton* brush_;
    QPushButton* line_;
    QPushButton* fill_;
    QPushButton* rectangle_;
    QPushButton* circle_;
    QPushButton* text_;

    QPushButton* uparrow_;
    QPushButton* downarrow_;
    QPushButton* leftarrow_;
    QPushButton* rightarrow_;

    QPushButton* addkeyframe_;
    QPushButton* removekeyframe_;
    QPushButton* moveframe_;
    QPushButton* movechildren_;

    void move(int,int);
private slots:
    void moveFramePressed();
    void moveChildrenPressed();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    void addKey();
    void removeKey();

    void windowModeClicked();

public:
    Tool activeTool();
    Tools(MainWindow*,QWidget* _=NULL);
    bool windowMode() const;
};

#endif
