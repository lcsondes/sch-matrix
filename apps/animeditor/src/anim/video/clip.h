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

#ifndef CLIP_H
#define	CLIP_H

#include "global.h"
#include "frame.h"
#include "interpolator.h"
#include "matrixstack.h"

class SchWindow;

/**
 * Egy klip (frame-ek sorozata)
 */
class Clip : public QSharedData
{
    template<typename T>
    friend class QExplicitlySharedDataPointer;
public:
    struct Embed
    {
        QExplicitlySharedDataPointer<Clip> clip;
        Interpolator x,y;
        int z,t;
        Embed(){z=t=0;}
        void debugprint();
    };
private:
    QList<Frame> frames_;
    QList<Embed*> children_;
    QList<QTreeWidgetItem*> treeItems_;
    QMdiSubWindow* mdiWindow_; //NULL, ha nincs épp
    SchWindow* schWindow_; //NULL, ha nincs
    QString name_;
    uint w_,h_;

    bool checked_; //Used for DAG check

    Clip(const Clip&);

    virtual void render2(Frame*,int,MatrixStack&,bool) const;
public:
    Clip(uint w,uint h,QString name=QString());
    ~Clip();
    void closeWindow();
    void makeTreeItems(QTreeWidgetItem* parent,const Embed*);
    void removeTreeItems(QTreeWidgetItem*,const Embed*);

    QString name() const;
    uint width() const;
    uint height() const;
    uint length() const;
    const Frame& frame(int) const;
    Frame& frame(int);
    int frameIndex(int t) const;
    int frameStart(uint i) const;
    int nextKey(int t) const;
    int prevKey(int t) const;
    Embed* child(int) const;
    int getChildIndex(Embed*) const;
    int getParentCount() const;
    int getChildCount() const;
    int getFrameCount() const;
    QTreeWidgetItem* treeItem(int) const;
    QMdiSubWindow* mdiWindow() const;
    SchWindow* schWindow() const;

    void setName(QString); //widgetitemeket is updateli
    void update(bool updateSch=true);
    void updateTree();

    void clear();

    void addFrame(const Frame&);
    void addFrame(const Frame&,int previous);
    void addChild(Embed*); //embedet kívülről szülnek, a struct public
    void insertChild(Embed*,int);
    void addTreeItem(QTreeWidgetItem*);
    void removeFrame(int);
    void removeChild(int);
    void removeChild(Embed*);
    void removeTreeItem(int);
    void removeTreeItem(QTreeWidgetItem*);
    void setMdiWindow(QMdiSubWindow*);
    void setSchWindow(SchWindow*);

    void swapChildren(int,int);

    void check();
    void unCheck();
    bool isChecked() const;

    void render(Frame*,int,bool) const;

    void debugprint();
};

typedef Clip::Embed Embed;

#endif
