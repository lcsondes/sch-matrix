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

#include "mainwindow.h"

#include <QtIOCompressor>

#include "scenetree.h"
#include "clip.h"
#include "qpaloader.h"
#include "qp4handler.h"
#include "clipmdisubwindow.h"
#include "aboutwindow.h"
#include "metaeditdialog.h"
#include "qp4debug.h"

MainWindow::MainWindow()
{
    setWindowTitle(tr("AnimEditor 2012"));

    epichax_=new QTimer(this);
    epichax_->start(500);
    connect(epichax_,SIGNAL(timeout()),this,SLOT(updateAllWindows()));

    mdi_=new QMdiArea(this);
    setCentralWidget(mdi_);

    fileMenu_=menuBar()->addMenu(tr("&File"));
    newAction_=fileMenu_->addAction(tr("&New animation..."));
    newAction_->setIcon(QIcon(":icons/new.svg"));
    connect(newAction_,SIGNAL(triggered()),this,SLOT(newClicked()));
    openAction_=fileMenu_->addAction(tr("&Open animation..."));
    openAction_->setIcon(QIcon(":icons/open.svg"));
    connect(openAction_,SIGNAL(triggered()),this,SLOT(openClicked()));
    qpaAction_=fileMenu_->addAction(tr("Import &QPA animation..."));
    qpaAction_->setIcon(QIcon(":icons/open.svg"));
    connect(qpaAction_,SIGNAL(triggered()),this,SLOT(openQpaClicked()));
    audioAction_=fileMenu_->addAction(tr("Open a&udio file... "));
    audioAction_->setIcon(QIcon(":icons/audio.svg"));
    connect(audioAction_,SIGNAL(triggered()),this,SLOT(openAudioClicked()));
    saveAction_=fileMenu_->addAction(tr("&Save animation"));
    saveAction_->setIcon(QIcon(":icons/save.svg"));
    connect(saveAction_,SIGNAL(triggered()),this,SLOT(saveClicked()));
    saveAsAction_=fileMenu_->addAction(tr("Save animation &as..."));
    saveAsAction_->setIcon(QIcon(":icons/saveas.svg"));
    connect(saveAsAction_,SIGNAL(triggered()),this,SLOT(saveAsClicked()));
    exportAction_=fileMenu_->addAction(tr("&Export animation..."));
    exportAction_->setIcon(QIcon(":icons/export.svg"));
    connect(exportAction_,SIGNAL(triggered()),this,SLOT(exportClicked()));
    fileMenu_->addSeparator();
    exitAction_=fileMenu_->addAction(tr("E&xit editor"));
    exitAction_->setIcon(QIcon(":icons/exit.svg"));
    connect(exitAction_,SIGNAL(triggered()),this,SLOT(close()));

    editMenu_=menuBar()->addMenu(tr("&Edit"));
    editMetaAction_=editMenu_->addAction(QIcon(":icons/pencil.svg"),
                                         tr("Edit &tags..."));
    connect(editMetaAction_,SIGNAL(triggered()),this,SLOT(editMeta()));
    copyFrame_=editMenu_->addAction(QIcon(":icons/copy.svg"),tr("&Copy frame"));
    connect(copyFrame_,SIGNAL(triggered()),this,SLOT(copyFrame()));
    pasteFrame_=editMenu_->addAction(QIcon(":icons/paste.svg"),
                                     tr("&Paste frame"));
    connect(pasteFrame_,SIGNAL(triggered()),this,SLOT(pasteFrame()));

    helpMenu_=menuBar()->addMenu(tr("&Help"));
    aboutAction_=helpMenu_->addAction(tr("&About"));
    aboutAction_->setIcon(QIcon(":icons/about.svg"));
    connect(aboutAction_,SIGNAL(triggered()),this,SLOT(about()));

    paletteDock_=new QDockWidget(tr("Palette"),this);
    paletteDock_->setFeatures(QDockWidget::DockWidgetMovable|
                              QDockWidget::DockWidgetFloatable);
    palette_=new Palette(paletteDock_);
    paletteDock_->setWidget(palette_);
    addDockWidget(Qt::RightDockWidgetArea,paletteDock_);

    toolsDock_=new QDockWidget(tr("Tools"),this);
    toolsDock_->setFeatures(QDockWidget::DockWidgetMovable|
                            QDockWidget::DockWidgetFloatable);
    tools_=new Tools(this,toolsDock_);
    toolsDock_->setWidget(tools_);
    addDockWidget(Qt::RightDockWidgetArea,toolsDock_);

    sceneTreeDock_=new QDockWidget(tr("Library"),this);
    sceneTreeDock_->setFeatures(QDockWidget::DockWidgetMovable|
                                QDockWidget::DockWidgetFloatable);

    sceneTreePanel_=new SceneTreePanel(this);

    sceneTree_=sceneTreePanel_->getSceneTree();
    sceneTreeDock_->setWidget(sceneTreePanel_);

    addDockWidget(Qt::RightDockWidgetArea,sceneTreeDock_);
    connect(sceneTree_,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(addMdiWindow(QTreeWidgetItem*)));

    main_=new SchWindow(sceneTree_,palette_,tools_,sceneTree_->main(),
                        SchWindow::SCH_MODE,this);
    main_->setWindowTitle(tr("main"));
    //A main bezárhatatlan
    QMdiSubWindow* main=mdi_->addSubWindow(main_,
                                           Qt::Window|Qt::CustomizeWindowHint|
                                           Qt::WindowMinMaxButtonsHint);
    sceneTree_->main()->setMdiWindow(main);
    main->setWindowIcon(QIcon(":icons/anim.svg"));
    main->showMaximized();
    main->setAttribute(Qt::WA_DeleteOnClose);

    connect(main_,SIGNAL(insertClip(Embed*,Clip*)),
            sceneTree_,SLOT(insertClip(Embed*,Clip*)));

    show();
}

MainWindow::~MainWindow()
{
    //~QObject elintézi a gyerekeket
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    closeCurrent()?event->accept():event->ignore();
}

SchWindow* MainWindow::activeSubWindow()
{
    return(SchWindow*)(((ClipMdiSubWindow*)mdi_->activeSubWindow())->widget());
}

void MainWindow::updateAllWindows()
{
    auto list=mdi_->subWindowList();
    for(auto i=list.constBegin();i!=list.constEnd();++i)
    {
        ((SchWindow*)((ClipMdiSubWindow*)((*i)->widget())))->update();
    }
}

void MainWindow::about()
{
    AboutWindow w;
    w.exec();
}

void MainWindow::editMeta()
{
    MetaEditDialog m(sceneTree_,this);
    if(m.exec()!=QDialog::Accepted)return;
    SceneTree::Metadata* meta=sceneTree_->metadata();
    meta->team=m.team();
    meta->title=m.title();
    meta->year=m.year();
}

void MainWindow::copyFrame()
{
    SchWindow* win=activeSubWindow();
    if(!win)return;

    win->copyToClipboard();
}

void MainWindow::pasteFrame()
{
    SchWindow* win=activeSubWindow();
    if(!win)return;

    win->pasteFromClipboard();
}

bool MainWindow::closeCurrent()
{
    QMessageBox::StandardButton result=QMessageBox::question
            (this,tr("Closing current animation"),
             tr("Do you want to save changes?"),
             QMessageBox::Save|QMessageBox::Cancel|QMessageBox::Discard,
             QMessageBox::Cancel);
    switch(result)
    {
        case QMessageBox::Cancel:return false;
        case QMessageBox::Save:saveAsClicked();
            //fallthrough
        case QMessageBox::Discard:return true;
        default:qDebug()<<"Wtf";
    }
    return true;
}

void MainWindow::newClicked()
{
    if(!closeCurrent())return;
    Clip* newroot=new Clip(0,0);
    Embed* newmain=new Embed;
    newmain->clip=new Clip(SchWidget::WIDTH,SchWidget::HEIGHT,
                           SceneTree::MAIN_NAME);
    Frame f(SchWidget::WIDTH,SchWidget::HEIGHT);
    f.setDelay(1000);
    newmain->clip->addFrame(f);
    newmain->z=0;
    newmain->t=0;
    newroot->addChild(newmain);
    QString error;
    if(!QP4Handler::replaceRoot(sceneTree_,newroot,&error))
    {
        QMessageBox::critical(this,tr("Error"),
                              tr("An internal error occurred:\n%1").arg(error));
        return;
    }
    delete newroot;
	currentQp4_=QFileInfo();
	*sceneTree_->metadata()=SceneTree::Metadata(); //hax :(
}

void MainWindow::openClicked()
{
    if(!closeCurrent())return;
    QString file=QFileDialog::getOpenFileName(this,tr("Open QP4"),QString(),
                                              tr("QP4 files (*.qp4 *.q4z)"));
    QFileInfo fileinfo(file);
    if(fileinfo.isFile())
    {
        QDialog d(this);
        d.setWindowTitle(tr("AnimEditor"));
        QGridLayout layout(&d);
        QLabel label(tr("Loading, please wait..."),&d);
        layout.addWidget(&label,0,0,1,1);
        d.open();
        QP4Handler h;
        QP4Debug* debugger=new QP4Debug;
        connect(&h,SIGNAL(debugLine(QString)),debugger,SLOT(debugLine(QString)));

        QApplication::processEvents();
        if(!h.load(sceneTree_,file,fileinfo.suffix().toLower()=="q4z"))
        {
            QMessageBox::critical(this,tr("Couldn't open file"),
                                  tr("There was an error while loading the "
                                     "file:\n%1").arg(h.error));
            goto end;
        }
        currentQp4_=file;
end:
        ;
        debugger->show2();
        debugger->deleteIfEmpty();
    }
}

void MainWindow::openQpaClicked()
{
    if(!closeCurrent())return;
    QString file=QFileDialog::getOpenFileName(this,tr("Open QPA"),QString(),
                                              tr("QPA files (*.qpa)"));
    if(QFileInfo(file).isFile())
    {
        QString error;
        //nem kell dialog, ez qrva gyors
        if(!QpaLoader::load(sceneTree_,file,&error))
        {
            QMessageBox::critical(this,tr("Couldn't open file"),
                                  tr("There was an error while loading the "
                                     "file:\n%1").arg(error));
            return;
        }
        currentQp4_=QFileInfo();
    }
}

void MainWindow::openAudioClicked()
{
    QString file=QFileDialog::getOpenFileName(this,tr("Open audio file"),
                                              QString(),
                                              tr("Audio files (*.mp3 *.ogg)"));
    QFileInfo f(file);
    if(!f.isFile())return;
    if(f.canonicalPath()!=currentQp4_.canonicalPath())
    {
        //itt felmásoltuk volna az audiot a qp4 mellé, de le van szarva.
    }
    sceneTree_->metadata()->audio=f.canonicalFilePath();
}

void MainWindow::saveCore(QString name)
{
    if(name.isEmpty())return;
    QDialog d(this);
    d.setWindowTitle(tr("AnimEditor"));
    QGridLayout layout(&d);
    QLabel label(tr("Saving, please wait..."),&d);
    layout.addWidget(&label,0,0,1,1);
    d.open();
    QApplication::processEvents();
    QFile f(name);
    f.open(QIODevice::WriteOnly);
    QSharedPointer<QtIOCompressor> compressor;
    bool compress=QFileInfo(name).suffix().toLower()=="q4z";
    if(compress)
    {
        compressor=QSharedPointer<QtIOCompressor>(new QtIOCompressor(&f,9,
                                                                     1048576));
        compressor->open(QIODevice::WriteOnly);
    }
    QP4Handler::save(sceneTree_->root(),sceneTree_->metadata(),
                     compress?(QIODevice*)compressor.data():(QIODevice*)&f);
}

void MainWindow::saveClicked()
{
    if(!currentQp4_.isFile()||!sceneTree_->metadata()->generated)
    {
        saveAsClicked();
    }
    else
    {
        saveCore(currentQp4_.canonicalFilePath());
    }
}

void MainWindow::saveAsClicked()
{
    QString s=QFileDialog::getSaveFileName(this,tr("Save"),QString(),
                                           tr("QP4 file (*.qp4);;"
                                              "Q4Z file (*.q4z)"));
    saveCore(s);
}

void MainWindow::exportClicked()
{
    //jól elküldjük a francba azt, aki nem MP3/OGG-t ad
    
    //hang nélkül is lehet valid (lehet, hogy nem létező hangot kapunk beküldött
    //qp4-ből)
    if(QFile::exists(sceneTree_->metadata()->audio))
    {
        QString format=AudioPlayer::tellFormat(sceneTree_->metadata()->audio);
        if(format!="MPEG"&&format!="OGG")
        {
            QMessageBox::warning(this,tr("Invalid audio format"),
                                 tr("Invalid sound file format!\nPlease "
                                    "convert the audio to MP3 or OGG before "
                                    "exporting your animation."));
            return;//mwhahaha nincs több wav
        }
    }
    else
    {
        //de azért rákérdezünk, hogy tuti hang nélkül akarta-e
        QMessageBox::StandardButton answer=QMessageBox::question
                (this,tr("No audio"),
                 tr("You have no audio loaded.\nAre you sure you want to "
                    "export your animation without sound?"),
                 QMessageBox::Yes|QMessageBox::No);
        if(answer!=QMessageBox::Yes)return;
    }
    QString name=QFileDialog::getSaveFileName(this,tr("Export"),QString(),
                                              tr("Q4X file (*.q4x)"));
    if(name.isEmpty())return;
    QDialog d(this);
    d.setWindowTitle(tr("AnimEditor"));
    QGridLayout layout(&d);
    QLabel label(tr("Exporting, please wait..."),&d);
    layout.addWidget(&label,0,0,1,1);
    d.open();
    QApplication::processEvents();
    QFile f(name);
    f.open(QIODevice::WriteOnly);
    QP4Handler::exportAnim(sceneTree_->root(),sceneTree_->metadata(),&f);
}

void MainWindow::addMdiWindow(Clip* clip)
{
    if(clip->mdiWindow())
    {
        mdi_->setActiveSubWindow(clip->mdiWindow());
        return;
    }
    SchWindow* w=new SchWindow(sceneTree_,palette_,tools_,clip,
                               SchWindow::EDITOR_MODE,this);
    connect(w,SIGNAL(insertClip(Embed*,Clip*)),
            sceneTree_,SLOT(insertClip(Embed*,Clip*)));
    w->setWindowTitle(clip->name());
    ClipMdiSubWindow* s=new ClipMdiSubWindow(clip);
    s->setWidget(w);
    mdi_->addSubWindow(s);
    s->setWindowIcon(QIcon(":icons/anim.svg"));
    s->setAttribute(Qt::WA_DeleteOnClose);
    s->showNormal();
    clip->setMdiWindow(s);
}

void MainWindow::addMdiWindow(QTreeWidgetItem* item)
{
    Embed* embed=*(Embed**)(item->data(0,Qt::UserRole).toByteArray().constData());
    addMdiWindow(embed->clip.data());
}
