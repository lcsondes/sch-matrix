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
#include "safebutton.h"

/**
 * A lejátszást vezérlő widget.
 */
class PlayControls : public QWidget
{
Q_OBJECT
    QHBoxLayout* layout_;
    QPushButton* prev_; ///< Előző szám (lesz ez után)
    SafeButton* play_; ///< Lejátszás indítása
    SafeButton* stop_; ///< Lejátszás leállítása azonnal
    SafeButton* pauseNow_; ///< Lejátszás szüneteltetése azonnal
    QPushButton* pauseNext_; ///< Ne lépjen a következő számra
    QPushButton* next_; ///< Következő szám (lesz ez után)
public:
    PlayControls(QWidget* parent=NULL);
    bool isPauseNext();
signals:
    void prev();
    void play();
    void stop();
    void pauseNow();
    void next();
};

#endif
