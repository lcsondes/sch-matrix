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

#ifndef SCHWINDOW_H
#define	SCHWINDOW_H

#include "global.h"
#include "schwidget.h"
#include "playcontrols.h"
#include "timeline.h"
#include "clip.h"
#include "frame.h"
#include "audioplayer.h"
#include "scenetree.h"

class SchWindow : public QWidget
{
Q_OBJECT
	
    friend class QP4Handler;
    friend class SchWidget;
    friend class Tools;

    SceneTree* sceneTree_;
    AudioPlayer* audioPlayer_;
    Clip* clip_;

    QGridLayout* layout_;
    Palette* palette_;
    Tools* tools_;
    TimeLine* timeline_;
    PlayControls* playcontrols_;
    SchWidget* schwidget_;

    bool isToggled_;

    QTimer* timer_;
    qint64 startedAt_;

    enum State
    {
        IDLE,
        PLAYING,
        PAUSED
    } state_;

    bool suppressTimelinePosChange_;
    void setTimelinePos(qint64,bool seek);
private slots:
    void timelinePosChanged(qint64);
public:
    enum Mode
    {
        SCH_MODE=0,
        EDITOR_MODE=1
    };

    static Frame* clipboard_;

    SchWindow(SceneTree*,Palette*,Tools*,Clip* clip,Mode,QWidget* _=NULL);
    ~SchWindow();

public slots:
    void clipDropped(int,int,Clip*);
    void update();
    void refresh();
    void pixelSet(int,int,QRgb);
    void positionChanged(qint64);
    void frameLengthChanged(int);
    void addFrame();
    void copyFrame();
    void wipeFrame();
    void delFrame();
    void toggleChanged(bool);
    void play();
    void playTick();
    void stop();
    void previous();
    void next();
    void prevKey();
    void nextKey();
    void first();
    void last();
    void copyToClipboard();
    void pasteFromClipboard();
signals:
    void insertClip(Embed*,Clip*);
};

#endif
