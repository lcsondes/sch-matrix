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

#ifndef CHILDSETTINGSDIALOG_H
#define	CHILDSETTINGSDIALOG_H

#include "global.h"
#include "video/clip.h"

/**
 * Egy beágyazott animáció beállításához használt ablak.
 */
class ChildSettingsDialog : public QDialog
{
Q_OBJECT
    
    Clip::Embed* embed_;

    QGridLayout* layout_;
    QGridLayout* btnlayout_;

    QDialogButtonBox* buttons_;
    QLabel* timelabel_;
    QLabel* xlabel_;
    QLabel* ylabel_;
    QSpinBox* timeedit_;
    QSpinBox* xedit_;
    QSpinBox* yedit_;
    
public:
    ChildSettingsDialog(Clip::Embed* embed,QWidget* parent);

    int getTime();
    int getX();
    int getY();
};

#endif	/* CHILDSETTINGSDIALOG_H */

