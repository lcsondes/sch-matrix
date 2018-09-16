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

#ifndef SCENETREE_H
#define	SCENETREE_H

#include "global.h"
#include "video/clip.h"
#include "video/frame.h"
#include "childsettingsdialog.h"

/**
 * Az animáció klipjeit megjelenítő widget.
 */
class SceneTree : public QTreeWidget
{
    Q_OBJECT
    friend class QP4Handler;

    QTreeWidgetItem* selectedItem_;
public:
    Embed* root_;
    Embed* main_;

public:
    struct Metadata
    {
        QString title;
        QString team;
        QString audio;
        uint year;
        bool generated;
        Metadata():year(QDate::currentDate().year()),generated(false){}
    };
private:
    Metadata meta_;
public:
    
    SceneTree(QWidget* _=NULL);
    ~SceneTree();

    QStringList mimeTypes() const;
    QMimeData* mimeData(const QList<QTreeWidgetItem*>) const;
    bool addClip(Embed*,Clip*);
    void removeClip(Embed*,Clip*);
    Clip* main();
    Embed* root();
    Metadata* metadata();

    static const QString MAIN_NAME;

    bool checkName(QString,Clip*);
protected:
    void contextMenuEvent(QContextMenuEvent*);
    bool checkDag(Clip* clip);

    void move(int delta);
public slots:
    void insertClip(Embed*,Clip*);
    void newItem();
    void newChildItem();
    void detachItem();
    void renameItem();
    void removeItem();
    void copyToRoot();
    void copyMain();
    void embedProperties();
    void newChildItemBtn();
    void removeItemBtn();
    void moveUp();
    void moveDown();
};

#endif
