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

#include "scenetree.h"
#include "newclipdialog.h"
#include "schwidget.h"

const QString SceneTree::MAIN_NAME("main");

/**
 * Constructor for the scene tree widget
 */
SceneTree::SceneTree(QWidget* parent/*=NULL*/)
    :QTreeWidget(parent),selectedItem_(NULL)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);

    QStringList labels;
    labels<<tr("Name")<<tr("Size")<<tr("Duration");

    setHeaderLabels(labels);

    //Create root clip
    root_=new Embed;
    root_->clip=new Clip(0,0);
    root_->t=0;
    root_->x.setValue(0,0);
    root_->y.setValue(0,0);
    root_->z=-1;

    root_->clip->addTreeItem(invisibleRootItem());

    QByteArray encodedData;
    encodedData.resize(sizeof(void*));
    *(void**)(encodedData.data())=root_;

    invisibleRootItem()->setData(0,Qt::UserRole,QVariant(encodedData));

    //Create main clip
    main_=new Embed;
    main_->clip=new Clip(SchWidget::WIDTH,SchWidget::HEIGHT);
    main_->t=0;
    main_->x.setValue(0,0);
    main_->y.setValue(0,0);
    main_->z=-1;

    main_->clip->setName(MAIN_NAME);
    Frame frame(SchWidget::WIDTH,SchWidget::HEIGHT);
    frame.setDelay(1000);
    main_->clip->addFrame(frame);

    QTreeWidgetItem* mainItem=new QTreeWidgetItem(this);
    mainItem->setText(0,MAIN_NAME);
    mainItem->setText(1,"32x26");
    mainItem->setText(2,"1000");
    QFont font=mainItem->font(0);
    font.setBold(true);
    mainItem->setFont(0,font);

    encodedData.resize(sizeof(void*));
    *(void**)(encodedData.data())=main_;

    mainItem->setData(0,Qt::UserRole,QVariant(encodedData));

    main_->clip->addTreeItem(mainItem);

    root_->clip->addChild(main_);

    addTopLevelItem(mainItem);
}

/**
 * Destructor for SceneTree
 */
SceneTree::~SceneTree()
{
    root_->clip->removeTreeItem(invisibleRootItem());

    delete root_;
}

/**
 * Returns the preferred mime types
 */
QStringList SceneTree::mimeTypes() const
{
    QStringList types;
    types<<"matrix/clip";
    return types;
}

/**
 * Returns the QMime representation of the selected items
 * Note: Returns the pointer of the first selected item in a QByteArray
 */
QMimeData* SceneTree::mimeData(const QList<QTreeWidgetItem*> items) const
{
    if(items.isEmpty())
    {
        return NULL;
    }

    QMimeData* mimeData=new QMimeData();
    QByteArray encodedData;

    encodedData.resize(sizeof(void*));

    Embed* embed=*(Embed**)(items[0]->data(0,Qt::UserRole).
                            toByteArray().constData());

    *(const void**)(encodedData.data())=embed->clip.constData();

    mimeData->setData("matrix/clip",encodedData);
    return mimeData;
}

/**
 * Adds the Embed clip to the dag at the given point
 */
bool SceneTree::addClip(Embed* embed,Clip* parent)
{
    //Add the embed to the dag
    parent->addChild(embed);

    //Check dag data
    if(!checkDag(root_->clip.data()))
    {
        //Remove clip
        parent->removeChild(embed);

        return false;
    }

    //Update ui
    parent->update();

    //Dag ok, generate visual elements
    for(int i=0;i<parent->getParentCount();i++)
    {
        embed->clip->makeTreeItems(parent->treeItem(i),embed);
    }

    return true;
}

/**
 * Inserts the given child under the clip    [SLOT]
 */
void SceneTree::insertClip(Embed* child,Clip* parent)
{
    //Check main
    if(child->clip.data()==main_->clip.data())
    {
        QMessageBox::warning(this,tr("Error"),
                             tr("You can't insert the main clip as a subclip."));
        return;
    }

    //Check
    if(child->clip.data()==parent)
    {
        QMessageBox::warning(this,tr("Error"),
                             tr("You can't insert a clip into itself."));
        return;
    }

    //Try to insert
    if(!addClip(child,parent))
    {
        QMessageBox::warning(this,tr("Error"),tr("Can't add this clip.\n"
                                                 "Possibly you linked a child "
                                                 "into one of its descendants."));
        return;
    }
}

/**
 * Removes the emed clip from the dag
 */
void SceneTree::removeClip(Embed* embed,Clip* parent)
{
    //Note: a parent alól kell kiszedni az embed-re mutató twi-ket, majd az embedet

    //Remove visual elements
    for(int i=0;i<parent->getParentCount();i++) //Végigmegyünk a parent TWI-in
    {
        embed->clip->removeTreeItems(parent->treeItem(i),embed);//Kiszedetjük azokat a gyerekeiket, akik az embed-re mutatnak!
    }

    //Remove clip
    //printf("Remove embed of the selected twi-items\n");fflush(stdout);
    parent->removeChild(embed);
    //delete embed;
}

/**
 * Returns main clip
 */
Clip* SceneTree::main()
{
    return main_->clip.data();
}

/**
 * Returns root embed
 */
Embed* SceneTree::root()
{
    return root_;
}

/**
 * Returns metadata
 */
SceneTree::Metadata* SceneTree::metadata()
{
    return &meta_;
}

/**
 * Pop-ups a context menu, if the cursor is above a valid item    [OVERRIDE]
 */
void SceneTree::contextMenuEvent(QContextMenuEvent* event)
{
    QTreeWidgetItem* item=itemAt(event->pos());

    selectedItem_=item;

    QMenu menu(this);
    QAction newItem(tr("New"),this);
    QAction newChildItem(tr("New child"),this);
    QAction copyToRoot(tr("Copy to root"),this);
    QAction copyMain(tr("Clone main clip"),this);
    QAction detachItem(tr("Detach"),this);
    QAction renameItem(tr("Rename"),this);
    QAction removeItem(tr("Remove"),this);
    QAction embedProperties(tr("Embed properties"),this);

    newItem.setToolTip(tr("Creates new clip in the root."));
    newChildItem.setToolTip(tr("Creates new child clip for the "
                               "current selected clip."));
    copyToRoot.setToolTip(tr("Copy selected clip to the root."));
    copyMain.setToolTip(tr("Clones main clip and places it to the root."));
    detachItem.setToolTip(tr("Detaches the clip from its other instances."));
    renameItem.setToolTip(tr("Renames the selected clip."));
    removeItem.setToolTip(tr("Removes selected clip from its parent."));
    embedProperties.setToolTip(tr("Set the properties of the insertion point."));

    if(!item)
    {
        newChildItem.setEnabled(false);
        copyToRoot.setEnabled(false);
        copyMain.setEnabled(false);
        detachItem.setEnabled(false);
        renameItem.setEnabled(false);
        removeItem.setEnabled(false);
        embedProperties.setEnabled(false);
        selectedItem_=NULL;
    }
    else
    {
        if(item->text(0)!=QString("main"))
        {
            copyMain.setEnabled(false);
        }

        if(item->parent()==NULL)
        {
            embedProperties.setEnabled(false);
        }
    }


    connect(&newItem,SIGNAL(triggered()),this,SLOT(newItem()));
    connect(&newChildItem,SIGNAL(triggered()),this,SLOT(newChildItem()));
    connect(&copyToRoot,SIGNAL(triggered()),this,SLOT(copyToRoot()));
    connect(&copyMain,SIGNAL(triggered()),this,SLOT(copyMain()));
    connect(&detachItem,SIGNAL(triggered()),this,SLOT(detachItem()));
    connect(&renameItem,SIGNAL(triggered()),this,SLOT(renameItem()));
    connect(&removeItem,SIGNAL(triggered()),this,SLOT(removeItem()));
    connect(&embedProperties,SIGNAL(triggered()),this,SLOT(embedProperties()));

    menu.addAction(&newItem);
    menu.addAction(&newChildItem);
    menu.addSeparator();
    menu.addAction(&copyToRoot);
    menu.addAction(&copyMain);
    menu.addAction(&detachItem);
    menu.addAction(&renameItem);
    menu.addAction(&removeItem);
    menu.addSeparator();
    menu.addAction(&embedProperties);

    menu.exec(event->globalPos());
}

/**
 * Checks the data structure
 */
bool SceneTree::checkDag(Clip* clip)
{
    //Check signing
    if(clip->isChecked())
    {
        return false;
    }

    //Sign clip
    clip->check();

    for(int i=0;i<clip->getChildCount();i++)
    {
        if(!checkDag(clip->child(i)->clip.data()))
        {
            clip->unCheck();
            return false;
        }
    }

    clip->unCheck();

    return true;
}

/**
 * Check whether the name is already used or not (NEEDS DAG!!!)
 */
bool SceneTree::checkName(QString name,Clip* clip)
{
    //Check name
    if(clip->name()==name)
    {
        return false;
    }

    for(int i=0;i<clip->getChildCount();i++)
    {
        if(!checkName(name,clip->child(i)->clip.data()))
        {
            return false;
        }
    }

    return true;
}

/**
 * Creates new empty top-level clip in the library
 */
void SceneTree::newItem()
{
    selectedItem_=NULL;
    newChildItem();
}

/**
 * Creates new empty clip under the selected parent in the library
 */
void SceneTree::newChildItem()
{
    //if(!selectedItem_)return; megyünk tovább és megoldjuk

    QString name;
    int w,h;

    {
        NewClipDialog d(this);
        if(d.exec()!=QDialog::Accepted)return;

        name=d.name();
        w=d.width();
        h=d.height();
        //felhúzzuk párosra
        w+=w&1;
        h+=h&1;
    }

    if(!checkName(name,root_->clip.data()))
    {
        QMessageBox::warning(this,tr("Error"),
                             tr("This name is already in use."));
        return;
    }

    //Create new clip and insert it
    Clip* clip=new Clip(w,h);
    clip->setName(name);
    Frame frame(w,h);
    frame.setDelay(1000);
    clip->addFrame(frame);

    Embed* embed=new Embed;
    embed->clip=clip;
    embed->t=0;
    embed->x.setValue(0,0);
    embed->y.setValue(0,0);
    embed->z=-1;

    //int xxx;
    //qDebug("Stack %p",&xxx);
    //qDebug("Tipus: %d",selectedItem_->data(0,Qt::UserRole).type());
    //qDebug("Hex: %s",selectedItem_->data(0,Qt::UserRole).toByteArray().toHex().constData());

    Clip* parent=selectedItem_?(*(Embed**)(selectedItem_->data(0,Qt::UserRole).
                                           toByteArray().data()))->clip.data()
            :root_->clip.data();
    qDebug("Nev: %s\n",qPrintable(parent->name()));
    if(!addClip(embed,parent))
    {
        QMessageBox::warning(this,tr("Error"),tr("Couldn't add item."));
        return;
    }

    clip->update();
}

/**
 * Copy item to the root
 */
void SceneTree::copyToRoot()
{
    if(selectedItem_)
    {
        //Parent
        QTreeWidgetItem* parentItem=selectedItem_->parent();
        if(parentItem==NULL)
        {
            parentItem=invisibleRootItem();
        }

        //Get data
        Clip* clip=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()))->clip.data();

        //Check
        if(clip==main_->clip.data())
        {
            QMessageBox::information(this,tr("Error"),
                                     tr("You can't duplicate the main clip."));
            return;
        }

        //Create embed
        Embed* embed=new Embed;
        embed->clip=clip;
        embed->t=0;
        embed->x.setValue(0,0);
        embed->y.setValue(0,0);
        embed->z=-1;

        //Insert
        if(!addClip(embed,root_->clip.data()))
        {
            QMessageBox::warning(this,tr("Error"),
                                 tr("Couldn't add new item."));
            return;
        }

        clip->update();
    }
}

/**
 * Detaches the selected item    [SLOT]
 */
void SceneTree::detachItem()
{
    if(selectedItem_)
    {
        //Parent
        QTreeWidgetItem* parentItem=selectedItem_->parent();
        if(parentItem==NULL)
        {
            parentItem=invisibleRootItem();
        }

        //Get data
        Clip* clip=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).
                               toByteArray().data()))->clip.data();
        Clip* parent=(*(Embed**)(parentItem->data(0,Qt::UserRole).
                                 toByteArray().data()))->clip.data();

        //Check if selected item is the main
        if(clip==main_->clip.data())
        {
            QMessageBox::warning(this,tr("Couldn't detach"),
                                 tr("You can't detach the main clip."));
            return;
        }

        //Check if detach is possible
        if(clip->getParentCount()<=1)
        {
            //silently discard this one
            return;
        }

        //Check if detach is possible
        if(parent->getParentCount()>=2)
        {
            QMessageBox::warning(this,tr("Could not detach"),
                                 tr("The parent of the item you have tried to "
                                    "detach has more than one parent.\nDetach "
                                    "is impossible!"));
            return;
        }

        //Ask for total safety
        QMessageBox::StandardButton result=
                QMessageBox::question(this,tr("Detach clip"),
                                      tr("Do you want to detach clip \"%1\"?\n"
                                         "All instances will be modified!")
                                      .arg(clip->name()),
                                      QMessageBox::Yes|QMessageBox::No);
        if(result!=QMessageBox::Yes)
        {
            return;
        }

        //Close window and remove items
        clip->closeWindow();
        Embed* embed=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).
                                 toByteArray().data()));
        removeClip(embed,parent);

        //Detach!
        embed->clip.detach();

        //Re-insert item
        clip=embed->clip.data();
        addClip(embed,parent);


        /*

                //Remove this tree item from the item list of the clip
                clip->removeTreeItem(selectedItem_);

                //Detach!
                (*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()))->clip.detach();

                //Re-insert item
                clip=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()))->clip.data();
                clip->addTreeItem(selectedItem_);
         */
        //Get new name for the detached clip
        while(true)
        {
            bool ok;

            QString text=QInputDialog::getText(this,tr("Rename detached clip"),
                                               tr("Please enter the new name:"),
                                               QLineEdit::Normal,clip->name(),
                                               &ok);

            if(ok&& !text.isEmpty()&&(clip->name()!=text))
            {
                clip->setName(text);
                return;
            }
        }

    }
}

/**
 * Copies main clip to the root and detaches copy
 */
void SceneTree::copyMain()
{
    QString name;

    bool ok;

    //Read clip name
    name=QInputDialog::getText(this,tr("Give name"),
                               tr("Please enter the name of the copy:"),QLineEdit::Normal,"New name",&ok);

    if(!ok)
    {
        return;
    }

    //Check whether name is unique
    if(!checkName(name,root_->clip.data()))
    {
        QMessageBox::warning(this,tr("Error"),
                             tr("This name is already in use."));
        return;
    }

    //Create new clip and insert int
    Clip* clip=new Clip(main_->clip->width(),main_->clip->height());
    clip->setName(name);

    //Copy main
    for(int i=0;i<main_->clip->getChildCount();i++)
    {
        Embed* embed=new Embed;
        embed->clip=main_->clip->child(i)->clip;
        embed->t=main_->clip->child(i)->t;
        embed->x=main_->clip->child(i)->x;
        embed->y=main_->clip->child(i)->y;
        embed->z=main_->clip->child(i)->z;

        clip->addChild(embed);
    }

    for(int i=0;i<main_->clip->getFrameCount();i++)
    {
        clip->addFrame(main_->clip->frame(i));
    }

    Embed* embed=new Embed;
    embed->clip=clip;
    embed->t=0;
    embed->x.setValue(0,0);
    embed->y.setValue(0,0);
    embed->z=-1;

    if(!addClip(embed,root_->clip.data()))
    {
        QMessageBox::warning(this,tr("Error"),tr("Couldn't add the new item."));
        return;
    }

    clip->update();
}

/**
 * Renames the selected item [SLOT]
 */
void SceneTree::renameItem()
{
    if(selectedItem_)
    {
        Clip* clip=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()))->clip.data();

        //Check if selected item is the main
        if(clip==main_->clip.data())
        {
            QMessageBox::warning(this,tr("Couldn't rename"),
                                 tr("The main clip cannot be renamed."));
            return;
        }

        bool ok;
        QString text=QInputDialog::getText(this,tr("Rename clip"),
                                           tr("Please enter the new name:"),
                                           QLineEdit::Normal,clip->name(),&ok);
        if(ok)
        {
            if(!text.isEmpty())
            {
                if(!checkName(text,root_->clip.data()))
                {
                    QMessageBox::warning(this,tr("Error"),
                                         tr("The name you entered is already in use."));
                    return;
                }
                clip->setName(text);
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
}

/**
 * Removes the selected item
 */
void SceneTree::removeItem()
{
    if(!selectedItem_)
    {
        return;
    }

    QTreeWidgetItem* parentItem=selectedItem_->parent();
    if(parentItem==NULL)
    {
        parentItem=invisibleRootItem();
    }

    Clip* clip=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()))->clip.data();
    Clip* parent=(*(Embed**)(parentItem->data(0,Qt::UserRole).toByteArray().data()))->clip.data();

    //Check if selected item is the main
    if(clip==main_->clip.data())
    {
        QMessageBox::warning(this,tr("Can't remove main clip"),
                             tr("The main clip cannot be removed."));
        return;
    }

    //Safety check
    QMessageBox::StandardButton result=
            QMessageBox::question(this,tr("Remove clip"),
                                  tr("Do you want to remove clip \"%1\" from "
                                     "clip \"%2\"?\n"
                                     "All instances of \"%2\" will be modified!")
                                  .arg(clip->name()).arg(parent->name()),
                                  QMessageBox::Yes|QMessageBox::No);
    if(result!=QMessageBox::Yes)
    {
        return;
    }

    //Remove item
    clip->closeWindow();
    Embed* embed=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()));//New!
    removeClip(*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data()),parent);
    delete embed;//New!

    parent->update();
}

/**
 * Shows dialog to set embed properties
 */
void SceneTree::embedProperties()
{
    if(selectedItem_)
    {
        if(selectedItem_->parent()==NULL)
        {
            return;
        }

        Clip::Embed* embed=(*(Embed**)(selectedItem_->data(0,Qt::UserRole).
                                       toByteArray().data()));

        ChildSettingsDialog dialog(embed,this);

        if(dialog.exec()==QDialog::Accepted)
        {
            embed->t=dialog.getTime();
            embed->x.deltaShift(dialog.getX()-embed->x.value(0));
            embed->y.deltaShift(dialog.getY()-embed->y.value(0));
            embed->clip->update();
        }
    }
}

/**
 * Adds new child
 */
void SceneTree::newChildItemBtn()
{
    if(!selectedItems().isEmpty())
    {
        selectedItem_=selectedItems().at(0);
    }
    else
    {
        selectedItem_=invisibleRootItem();
    }
    newChildItem();
}

/**
 * Removes child
 */
void SceneTree::removeItemBtn()
{
    if(!selectedItems().isEmpty())
    {
        selectedItem_=selectedItems().at(0);
        removeItem();
    }
}

void SceneTree::move(int delta)
{
    if(!selectedItems().isEmpty())
    {
        selectedItem_=selectedItems().at(0);
        QTreeWidgetItem* parentItem=selectedItem_->parent();

        if(parentItem==NULL)
        {
            parentItem=invisibleRootItem();
        }

        if(selectedItem_->text(0)==MAIN_NAME)
        {
            return;
        }

        //Get data
        Embed* clip=*(Embed**)(selectedItem_->data(0,Qt::UserRole).toByteArray().data());
        Clip* parent=(*(Embed**)(parentItem->data(0,Qt::UserRole).toByteArray().data()))->clip.data();

        int index=parent->getChildIndex(clip);

        if(index+delta<0||index+delta>=parent->getChildCount())
        {
            return;
        }

        if(parent->child(index+delta)->clip.data()==main_->clip.data())
        {
            return;
        }

        //Remove tree item from parent and add again
        int newindex=parentItem->indexOfChild(selectedItem_)+delta;
        parentItem->takeChild(parentItem->indexOfChild(selectedItem_));
        parentItem->insertChild(newindex,selectedItem_);
        for(int i=0;i<parentItem->childCount();i++)
        {
            parentItem->child(i)->setSelected(false);
        }
        selectedItem_->setSelected(true);

        //Swap items
        parent->swapChildren(index,index+delta);

        selectedItem_=NULL;
    }
}

/**
 * Moves item up in Z order
 */
void SceneTree::moveUp()
{
    move(-1);
}

/**
 * Moves item down in Z order
 */
void SceneTree::moveDown()
{
    move(1);
}
