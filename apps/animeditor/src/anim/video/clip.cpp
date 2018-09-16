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

#include "clip.h"
#include "matrixstack.h"
#include "schwindow.h"
#include "clipmdisubwindow.h"
#include <algorithm>

bool operator==(const Embed& a,const Embed& b)
{
    return(a.clip==b.clip)&&(a.x==b.x)&&(a.y==b.y)&&(a.z==b.z)&&(a.t==b.t);
}

/**
 * Costructor for Clip
 */
Clip::Clip(uint w,uint h,QString name/*=QString()*/)
    :mdiWindow_(NULL),schWindow_(NULL),name_(name),w_(w),h_(h),checked_(false)
{
    if(w_&1)++w_;
    if(h_&1)++h_;
}

/**
 * Copy constructor for Clip [PRIVATE]
 * This constructor may only be used by the QExplicitlySharedDataPointer.
 * This constructor makes a deep copy of the Clip, except the QWidgets.
 */
Clip::Clip(const Clip& o)
    :QSharedData(o),frames_(o.frames_),mdiWindow_(NULL),schWindow_(NULL),
    name_(o.name_),w_(o.w_),h_(o.h_),checked_(o.checked_)
{
    for(int i=0;i<o.children_.count();i++)
    {
        Embed* embed=new Embed;
        embed->clip=o.children_[i]->clip;
        embed->t=o.children_[i]->t;
        embed->x=o.children_[i]->x;
        embed->y=o.children_[i]->y;
        embed->z=o.children_[i]->z;

        children_.append(embed);
    }

}

/**
 * Destructor for clip
 */
Clip::~Clip()
{
    qDebug("(Enter destructor of %s)",name_.toAscii().data());
    for(int i=0;i<children_.count();i++)
    {
        qDebug("Clip <\"%s\"> deletes child no. %d (\"%s\")",
               this->name_.toAscii().data(),i,
               children_[i]->clip->name().toAscii().data());
        delete children_[i];
    }
    children_.clear();

    for(int i=0;i<treeItems_.count();i++)
    {
        qDebug("Clip <\"%s\"> deletes QTWI no. %d",
               this->name_.toAscii().data(),i);
        treeItems_[i]->takeChildren();
        delete treeItems_[i];
    }
    treeItems_.clear();

    qDebug(">Clip \"%s\" deleted.",name_.toAscii().data());
}

/**
 * Closes the windows of the clip (if exist) recursively
 */
void Clip::closeWindow()
{
    if(mdiWindow_!=NULL&&name_!=SceneTree::MAIN_NAME)
    {
        mdiWindow_->close();
    }

    for(int i=0;i<children_.count();i++)
    {
        children_[i]->clip->closeWindow();
    }
}

/**
 * This function makes a TreeWidgetItem under the given parent. The new item
 * will reference to this Clip object through the given embed. Then it's
 * recursively called for all the embed Clips.
 */
void Clip::makeTreeItems(QTreeWidgetItem* parent,const Embed* embed)
{
    //Q_ASSERT(embed.clip==this);
    QTreeWidgetItem* item=new QTreeWidgetItem(parent);
    item->setText(0,name_);
    item->setText(1,QString("%1x%2").arg(w_).arg(h_));
    item->setText(2,QString::number(length())+QObject::tr(" ms"));
    {

        //qDebug()<<name()<<"has "<<getParentCount()<<" parents.";
        //qDebug()<<"mTI called with"<<embed<<", QTWI is (QTWI*)"<<item;
        if(item->text(0)==SceneTree::MAIN_NAME)
        {
            QFont font=item->font(0);
            font.setBold(true);
            item->setFont(0,font);
        }
    }


    QByteArray encodedData;
    encodedData.resize(sizeof(void*));
    *(const void**)(encodedData.data())=embed;

    item->setData(0,Qt::UserRole,QVariant(encodedData));

    embed->clip->addTreeItem(item);
    for(auto i=children_.begin();i!=children_.end();++i)
    {
        (*i)->clip->makeTreeItems(item,*i);
    }
    embed->clip->updateTree();
}

/**
 * Removes tree items
 */
void Clip::removeTreeItems(QTreeWidgetItem* parentItem,const Embed* embed)
{
    //Meg kell keresni a parent azon gyerekeit (egy lehet csak az adott embed-en keresztül!), akik ránk mutatnak, és rájuk újrahívódunk a saját gyerekeinkkel, majd gyilkolunk!
    for(int i=0;i!=parentItem->childCount();i++) //Igen, nem a saját item-listánkban, mert abban más is lehet!
    {
        Embed* mbd=*(Embed**)(parentItem->child(i)->data(0,Qt::UserRole).
                              toByteArray().data());

        if(mbd==embed)
        {
            //Megtaláltuk a ránk mutató elemet, rekurzívan onnan is töröljük az elemeket
            for(int n=0;n<children_.count();n++)
            {
                children_[n]->clip->removeTreeItems(parentItem->child(i),
                                                    children_[n]);
            }

            //Most a fa alján vagyunk tutira
            //Kiszedjük az itemet magunkból
            removeTreeItem(parentItem->child(i));

            //Kiszedjük az itemet a fából
            QTreeWidgetItem* item=parentItem->child(i);
            parentItem->removeChild(parentItem->child(i));
            delete item;
            return;
        }
    }
    qDebug("Return from remove tree items.");
}

/**
 * Returns the name of the clip
 */
QString Clip::name() const
{
    return name_;
}

/**
 * Returns the width of the clip
 */

uint Clip::width() const
{
    return w_;
}

/**
 * Returns the height of the clip
 */
uint Clip::height() const
{
    return h_;
}

extern bool noendworkaround;

/**
 * Returns the length of the clip (with all its subclips) in ms.
 */
uint Clip::length() const
{
    int retval=0;
    for(auto i=frames_.constBegin();i!=frames_.constEnd();++i)
    {
        retval+=i->delay();
    }
    //Itt állt egy noendworkaroundos if, de elbasztam, ezért végül
    //a továbbnyúlós figura lesz érvényben
    for(auto i=children_.constBegin();i!=children_.constEnd();++i)
    {
        int otherlength=(*i)->t+(*i)->clip->length();
        if(retval<otherlength)retval=otherlength;
    }
    return retval;
}

/**
 * Returns the i-th frame of the clip
 */
const Frame& Clip::frame(int i) const
{
    return frames_[i];
}

/**
 * Returns a reference to the i-th frame of the clip
 */
Frame& Clip::frame(int i)
{
    return frames_[i];
}

int Clip::frameIndex(int t) const
{
    if(t<0)return -1;
    int elapsed=0;
    for(int i=0;i<frames_.size();++i)
    {
        elapsed+=frames_[i].delay();
        if(elapsed>t)return i;
    }
    return -1;
}

int Clip::frameStart(uint idx) const
{
    if(idx>=uint(frames_.size()))
    {
        return -1;
    }
    int retval=0;
    for(uint i=0;i<idx;++i)
    {
        retval+=frames_[i].delay();
    }
    return retval;
}

int Clip::nextKey(int t) const
{
    int retval=t;
    for(auto i=children_.constBegin();i!=children_.constEnd();++i)
    {
        for(int j=0;j<=1;++j)
        {
            int next=(j?(*i)->x:(*i)->y).nextKey(t-((*i)->t));
            //ha a retval kezdeti értéken van, nagyobb kell, egyébként kisebb
            if((next>retval&&retval==t)||(next<retval&&next!=t))
            {
                retval=next;
            }
        }
    }
    return retval;
}

int Clip::prevKey(int t) const
{
    int retval=t;
    for(auto i=children_.constBegin();i!=children_.constEnd();++i)
    {
        for(int j=0;j<=1;++j)
        {
            int prev=(j?(*i)->x:(*i)->y).prevKey(t-((*i)->t));
            //ha a retval kezdeti értéken van, kisebb kell, egyébként nagyobb
            if((prev<retval&&retval==t)||(prev>retval&&prev!=t))
            {
                retval=prev;
            }
        }
    }
    return retval;
}

/**
 * Returns the i-th embed of the clip
 */
Embed* Clip::child(int i) const
{
    return children_[i];
}

/**
 * Returns the index of the child in the child_ list, otherwise returns -1
 */
int Clip::getChildIndex(Embed* embed) const
{
    if(!children_.isEmpty())
    {
        for(int i=0;i<children_.size();i++)
        {
            if(children_[i]==embed)
            {
                return i;
            }
        }
    }

    return -1;
}

/**
 * Returns the pointer of the i-th QTreeWidgetItem of the clip
 */
QTreeWidgetItem* Clip::treeItem(int i) const
{
    return treeItems_[i];
}

QMdiSubWindow* Clip::mdiWindow() const
{
    return mdiWindow_;
}

SchWindow* Clip::schWindow() const
{
    return schWindow_;
}

void Clip::clear()
{
    frames_.clear();
    Frame f(w_,h_);
    f.setDelay(1000);
    frames_.append(f);
    for(auto i=children_.begin();i!=children_.end();++i)
    {
        delete *i;
    }
    children_.clear();
}

/**
 * Sets the name of the clip - also refreshes all QTreeWidget items
 */
void Clip::setName(QString s)
{
    name_=s;

    if(!treeItems_.isEmpty())
    {
        for(int i=0;i<treeItems_.size();i++)
        {
            treeItems_.at(i)->setText(0,name_);
        }
    }
}

void Clip::update(bool updateSch/*=true*/)
{
    for(auto i=treeItems_.begin();i!=treeItems_.end();++i)
    {
        (*i)->setText(0,name_);
        {
            QFont font=(*i)->font(0);
            font.setItalic(getParentCount()>=2);
            (*i)->setFont(0,font);
        }
        (*i)->setText(1,QString("%1x%2").arg(w_).arg(h_));
        (*i)->setText(2,QString::number(length())+QObject::tr(" ms"));

        if(updateSch&&schWindow_!=NULL)
        {
            schWindow_->update();
        }

        QTreeWidgetItem* parentItem=(*i)->parent();
        if(parentItem!=NULL)
        {
            Clip* parent=(*(Embed**)(parentItem->data(0,Qt::UserRole).
                                     toByteArray().data()))->clip.data();
            parent->update();
        }
    }

}

/**
 * Updates the tree only
 */
void Clip::updateTree()
{
    update(false);
}

/**
 * Adds a frame to the framelist
 */
void Clip::addFrame(const Frame& f)
{
    frames_.append(f);
    update();
}

void Clip::addFrame(const Frame& f,int previous)
{
    frames_.insert(previous+1,f);
    update();
}

/**
 * Adds an embed to the clip
 */
void Clip::addChild(Embed* child) //embedet kívülről szülnek, a struct public
{
    children_.append(child);
}

/**
 * Adds a QTreeWidgetItem to the clip
 */
void Clip::addTreeItem(QTreeWidgetItem* item)
{
    treeItems_.append(item);
    updateTree();
}

/**
 * Removes the i-th frame of the clip
 */
void Clip::removeFrame(int i)
{
    frames_.removeAt(i);
    update();
}

/**
 * Removes the i-th embedded child of the clip
 */
void Clip::removeChild(int i)
{
    children_.removeAt(i);
}

/**
 * Remoces the given child
 */
void Clip::removeChild(Embed* embed)
{
    int index=children_.indexOf(embed);

    if(index>=0)
    {
        children_.removeAt(index);
    }
}

/**
 * Removes the i-th QTreeWidgetItem of the clip - ????
 */
void Clip::removeTreeItem(int i)
{
    treeItems_.removeAt(i);
    updateTree();
}

/**
 * Removes the given tree widget item from the list
 */
void Clip::removeTreeItem(QTreeWidgetItem* item)
{
    int index=treeItems_.indexOf(item);

    if(index>=0)
    {
        treeItems_.removeAt(index);
    }
    updateTree();
}

void Clip::setMdiWindow(QMdiSubWindow* w)
{
    //Q_ASSERT(!mdiWindow_);
    mdiWindow_=w;
}

void Clip::setSchWindow(SchWindow* w)
{
    //Q_ASSERT(!mdiWindow_);
    schWindow_=w;
}

/**
 * Swaps children a and b
 */
void Clip::swapChildren(int a,int b)
{
    int c1=a<b?a:b;
    int c2=a<b?b:a;

    Embed* e2=children_.takeAt(c2);
    Embed* e1=children_.takeAt(c1);

    children_.insert(c1,e2);
    children_.insert(c2,e1);

    update();
}

/**
 * Returns the count of parents (count of items in list)
 */
int Clip::getParentCount() const
{
    return treeItems_.count();
}

/**
 * Returns the count of children
 */
int Clip::getChildCount() const
{
    return children_.count();
}

/**
 * Gets the count of frames
 */
int Clip::getFrameCount() const
{
    return frames_.count();
}

/**
 * Sets the clip in checked state
 */
void Clip::check()
{
    checked_=true;
}

/**
 * Unchecks the clip
 */
void Clip::unCheck()
{
    checked_=false;
}

/**
 * Returns the state of the clip
 */
bool Clip::isChecked() const
{
    return checked_;
}

void Clip::render(Frame* tgt,int t,bool renderChildren) const
{
    MatrixStack stk;
    render2(tgt,t,stk,renderChildren);
}

void Clip::render2(Frame* tgt,int t,MatrixStack& stk,bool renderChildren) const
{
    int frameIdx=frameIndex(t);
    auto renderFrame=[&](){if(frameIdx>=0)frames_[frameIdx].render(tgt,stk);};
    if(!renderChildren)
    {
        renderFrame();
        return;
    }
    QList<Embed*> children_z=children_;
    //gyorsan reverse
    for(int i=0;i<children_z.size()/2;++i)
    {
        std::swap(children_z[i],children_z[children_z.size()-i-1]);
    }
    //TODO: ha lesz rendes Z-support, akkor ide menjen std::sort
    bool hadpositive=false;
    for(auto i=children_z.constBegin();i!=children_z.constEnd();++i)
    {
        if(!hadpositive&&(*i)->z>0)
        {
            renderFrame();
            hadpositive=true;
        }
        int t2=t-((*i)->t);
        stk.push();
        stk.translate((*i)->x.value(t2),(*i)->y.value(t2));
        (*i)->clip->render2(tgt,t2,stk,renderChildren/*true*/);
        stk.pop();
    }
    if(!hadpositive)renderFrame();
}

void Clip::Embed::debugprint()
{
    qDebug()<<"Embed for "<<clip->name()<<"\nx:\n{";
    x.debugprint();
    qDebug()<<"}\ny:\n{";
    y.debugprint();
    qDebug()<<"}\nz:"<<z<<",t:"<<t<<"\n{";
    clip->debugprint();
    qDebug()<<"}";
}

void Clip::debugprint()
{
    qDebug()<<"Clip name="<<name_<<", w:"<<w_<<", h:"<<h_;
    qDebug()<<getFrameCount()<<" frames";
    qDebug()<<"Frame w:"<<frames_[0].width()<<", h:"<<frames_[0].height();
    qDebug()<<"Children:\n{";
    for(auto i=children_.constBegin();i!=children_.constEnd();++i)
    {
        (*i)->debugprint();
    }
    qDebug()<<"}";
}
