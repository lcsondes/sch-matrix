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

#ifndef PLAYCONTROLS_H
#define	PLAYCONTROLS_H

#include "global.h"

/**
 * A lejátszásvezérlő elemeket tartalmazó widget.
 */
class PlayControls : public QWidget
{
Q_OBJECT
    bool isPlaying_;
    bool hideChildren_;
    int framecount_;
    int activeframe_;
    QIcon playIcon_;
    QIcon stopIcon_;
    QGridLayout* layout_;
    QGridLayout* mainLayout_;
    QGridLayout* framecontrollayout_;
    QGridLayout* editlayout_;
    QLabel* delay_;
    QSpinBox* timeedit_;
    QLabel* framecounter_;
    QPushButton* first_;
    QPushButton* prev_;
    QPushButton* play_;
    QPushButton* next_;
    QPushButton* last_;
    QPushButton* nextKey_;
    QPushButton* prevKey_;
    //QPushButton* addframe_;
    QPushButton* wipeframe_;
    QPushButton* copyframe_;
    QPushButton* delframe_;
    QPushButton* toggle_;
protected slots:
    void playClicked();
    void toggleClicked();
    void displayFrame();
public:
    PlayControls(QWidget* _=NULL);
public slots:
    void stopPlay();
    void startPlay();
    void setFrameCount(int);
    void setActiveFrame(int);
    void setFrameLength(int);
signals:
    void first();
    void previous();
    void prevKey();
    void play();
    void stop();
    void next();
    void nextKey();
    void last();
    void addFrame();
    void wipeFrame();
    void copyFrame();
    void delFrame();
    void toggleChanged(bool);
    void lengthChanged(int);
};

#endif
