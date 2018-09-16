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

#ifndef TEXTRENDERDIALOG_H
#define	TEXTRENDERDIALOG_H

#include "global.h"

/**
 * Szöveg rajzolására szolgáló párbeszédablak.
 */
class TextRenderDialog : public QDialog
{
Q_OBJECT

    QFont font_;

    QGridLayout* layout_;

    QLabel* fontLabel_;
    QLineEdit* fontName_;
    QPushButton* changeFont_;

    QLineEdit* textEdit_;

    QLabel* timeLabel_;
    QSpinBox* timeEdit_;

    QCheckBox* windowMode_;
    QLabel* tolerance_;
    QSpinBox* tolEdit_;

    QDialogButtonBox* buttons_;

    void setFontName();
private slots:
    void changeFont();
public:
    TextRenderDialog(QWidget* _=NULL);
    QFont font() const;
    QString text() const;
    int time() const;
    bool windowMode() const;
    int tolerance() const;
};

#endif
