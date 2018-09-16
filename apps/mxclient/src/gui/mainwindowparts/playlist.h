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

#ifndef PLAYLIST_H
#define	PLAYLIST_H

#include "global.h"
#include "playlistmodel.h"
#include "player.h"
#include "playcontrols.h"
#include "timeline.h"

/**
 * A playlist ablaktáblája
 */
class PlayList : public QWidget
{
Q_OBJECT
    PlayControls* playcontrols_;
    Player* player_;

    QGridLayout* layout_;
    PlayListModel model_;
    QTreeView* list_;
    QPushButton* up_;
    QPushButton* remove_;
    QPushButton* down_;
private slots:
    //gombok
    void moveUp();
    void moveDown();
    void remove();
public:
    PlayList(PlayControls* pc,TimeLine* tl,QWidget* parent=NULL);
    ~PlayList();
public slots:
    //serverpane
    void clientConnected();
    
    //dirview
    void addFile(QString);

    //playcontrols
    void prev();
    void play();
    void stop();
    void pauseContNow();
    void next();

    //player
    void currentAnimFinished();
};

#endif
