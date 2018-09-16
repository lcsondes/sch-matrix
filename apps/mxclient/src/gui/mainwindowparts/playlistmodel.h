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

#ifndef PLAYLISTMODEL_H
#define	PLAYLISTMODEL_H

#include "global.h"

/**
 * A playlist. Egyben QAbstractListModel is, szóval mehet Q*View-ba.
 */
class PlayListModel : public QAbstractListModel
{
Q_OBJECT
    FMOD_SYSTEM* fmod_;
public:
    struct Entry
    {
        QString filename;
        QString title;
        unsigned int length; //ms
        enum
        {
            NOTHING,
            NOW_PLAYING,
            NEXT
        } flags;
    };
private:
    QList<Entry> entries_; //a settereken keresztül piszkáld
    void move(int,int);
    void moveNext(int);
    
    const Entry* currentEntry() const;
public:
    static const QString INITDIR_QUERY;
    
    PlayListModel();
    ~PlayListModel();
    QString currentFile() const;
    quint32 currentLength() const;

    override int rowCount(const QModelIndex&) const;
    override int columnCount(const QModelIndex&) const;
    override QVariant headerData(int,Qt::Orientation,int role) const;
    override QVariant data(const QModelIndex&,int) const;
public slots:
    void addFile(QString);
    void moveUp(int);
    void moveDown(int);
    void remove(int);
    void setNext(const QModelIndex&);

    void currentAnimEnded();
    void rewindCurrentAnim();
    void selectNextAnim();
    void moveNextForward();
    void moveNextBackward();
};

#endif
