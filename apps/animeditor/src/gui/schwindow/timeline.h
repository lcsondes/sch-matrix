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

#ifndef TIMELINE_H
#define	TIMELINE_H

#include "global.h"

/**
 * Csúnya, QSlider-alapú idővonal.
 */
class TimeLine : public QWidget
{
Q_OBJECT
    QGridLayout* layout_;
    QLabel* elapsed_;
    QSlider* slider_;
    QLabel* remaining_;
    qint64 end_;

    bool sliderPressed_;
    //bool suppressValueChanged_;
private slots:
    void valueChanged0(int);
    void sliderPressed();
    void sliderReleased();
public:
    static const int MIN_DELAY=20;
    TimeLine(QWidget* parent=NULL);
    int getPosition();
public slots:
    void setRange(qint64);
    void setPos(qint64);
signals:
    void valueChanged(qint64);
};

#endif	/* _TIMELINE_H */

