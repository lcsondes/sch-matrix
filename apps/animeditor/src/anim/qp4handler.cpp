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

#include "qp4handler.h"

#include <QtIOCompressor>

#include "scenetree.h"
#include "schwindow.h"

// generált fájlokra rámentünk, de egyébként save as van, nehogy
// felülírjunk egy programot a kimenetével
const char* const QP4Handler::GENERATED_MARK="-- __animeditor__";
extern bool sideaudio;

static const luaL_Reg loadedlibs2[] = {
  {"_G", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_BITLIBNAME, luaopen_bit32},
  {LUA_MATHLIBNAME, luaopen_math},
  {NULL, NULL}
};

static void luaL_openlibs2 (lua_State *L) {
  const luaL_Reg *lib;
  /* call open functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs2; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }
}

QP4Handler::QP4Handler()
{
    L=luaL_newstate();
    luaL_openlibs2(L);

#define PUSH_FUNCTION(name) do{lua_pushlightuserdata(L,this);\
                               lua_pushcclosure(L,&name##static_,1);\
                               lua_setglobal(L,#name);}while(0)
    PUSH_FUNCTION(debug);
    PUSH_FUNCTION(meta);
    PUSH_FUNCTION(beginclip);
    PUSH_FUNCTION(frame);
    PUSH_FUNCTION(embed);
    PUSH_FUNCTION(endclip);
    PUSH_FUNCTION(rootclip);
}

QP4Handler::~QP4Handler()
{
    lua_close(L);
}

/**
 * Kimenti a windowpointereket a régi mainből és beteszi az újba.
 */
bool QP4Handler::replaceRoot(SceneTree* s,Clip* myroot,QString* error)
{
    int mainIdx=-1;
    for(int i=0;i<myroot->getChildCount();++i)
    {
        if(myroot->child(i)->clip->name()==SceneTree::MAIN_NAME)
        {
            mainIdx=i;
            break;
        }
    }
    if(mainIdx==-1)
    {
        if(error)*error=tr("The file doesn't contain a 'main' clip.");
        return false;
    }

    Clip* oldmain=s->main_->clip.data();
    oldmain->closeWindow(); //elengedi a Clip-referenciákat
    QMdiSubWindow* msw=oldmain->mdiWindow();
    SchWindow* schw=oldmain->schWindow();
    oldmain->setMdiWindow(NULL);
    oldmain->setSchWindow(NULL);
    s->root_->clip->clear();
    //s->main_; is meghalt

    //gyerekek átpakolása
    for(int i=0;i<myroot->getChildCount();++i)
    {
        Embed* e=new Embed(*myroot->child(i));
        s->root_->clip->addChild(ownership e);
        myroot->child(i)->clip->makeTreeItems(s->invisibleRootItem(),e);
    }
    s->main_=s->root_->clip->child(mainIdx);
    s->main_->clip->setMdiWindow(msw);
    s->main_->clip->setSchWindow(schw);

    schw->clip_=s->main_->clip.data();
    schw->update();

    return true;
}

bool QP4Handler::load(SceneTree* st,QString fn,bool compressed)
{
    filename=fn;
    if(!QFileInfo(filename).isFile())
    {
        error=tr("The specified file doesn't exist.");
        return false;
    }
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QSharedPointer<QtIOCompressor> compressor;
    QTextStream str;
    if(compressed)
    {
        compressor=QSharedPointer<QtIOCompressor>(new QtIOCompressor(&f));
        compressor->open(QIODevice::ReadOnly);
        str.setDevice(compressor.data());
    }
    else
    {
        str.setDevice(&f);
    }
    root=new Clip(0,0);
    str.setCodec("UTF-8");
    QString contents=str.readAll(); //aú.
    if(contents.startsWith(GENERATED_MARK))metadata.generated=true;
    if(luaL_dostring(L,contents.toUtf8().constData())) //aú.
    {
        error=tr("Lua error: [%1]").arg(luaL_checkstring(L,-1));
        qDebug()<<"Lua error:"<<error;
        return false;
    }
    if(!replaceRoot(st,root.data(),&error))return false;
    if(sideaudio)
    {
        static const char* ext[]={"mp3","ogg"};
        QString audiobase=fn;
        audiobase.replace(QRegExp("q(p4|4z)$"),"");
        for(uint i=0;i<ARRAY_SIZE(ext);++i)
        {
            QFileInfo aud(audiobase+ext[i]);
            if(aud.isFile())
            {
                metadata.audio=aud.canonicalFilePath();
            }
        }
    }
    st->meta_=metadata;
    return true;
}

bool QP4Handler::save(Embed* root,SceneTree::Metadata* meta,QIODevice* dev)
{
    {
        QTextStream str(dev);
        str.setCodec("UTF-8");
        str<<GENERATED_MARK<<"\nmeta({\n"<<
             "audio=\""<<meta->audio<<"\",\n"<<
             "team=\"" <<meta->team <<"\",\n"<<
             "title=\""<<meta->title<<"\",\n"<<
             "year="   <<meta->year <<"})\n\n";
    }
    bool retval=save2(root,dev);
    if(retval)
    {
        for(int i=0;i<root->clip->getChildCount();++i)
        {
            QTextStream str(dev);
            str<<"rootclip(\""<<root->clip->child(i)->clip->name()<<"\")\n";
        }
    }
    uncheckTree(root);
    return retval;
}

bool QP4Handler::exportAnim(Embed* root,SceneTree::Metadata* meta,
                            QIODevice* dev)
{
    //Formátum: Q4X1[w:2][h:2][méret:4]Q4Z[méret:4]QPRZ[méret:4]hang
    //minden szám big-endian
    //exportra picit feljebb tekertem a tömörítést
    dev->write("Q4X1");
    //méretek
    {
        quint16 w=qToBigEndian<quint16>(SchWidget::WIDTH);
        quint16 h=qToBigEndian<quint16>(SchWidget::HEIGHT);
        dev->write((char*)&w,2);
        dev->write((char*)&h,2);
    }
    //0=q4z,1="qprz"
    for(int pass=0;pass<=1;++pass)
    {
        QBuffer qz;
        qz.open(QIODevice::WriteOnly);
        {
            QtIOCompressor compressor(&qz,9,16777216);
            compressor.open(QIODevice::WriteOnly);
            (!pass?save:render)(root,meta,&compressor); //uncompressed
        }
        quint32 size=qToBigEndian<quint32>(qz.size());
        dev->write((char*)&size,4);
        dev->write(qz.data());
    }
    //hang. a MainWindow garantálja, hogy MP3 vagy OGG
    if(meta->audio.isEmpty())
    {
        quint32 zero=0;
        dev->write((char*)&zero,4);
    }
    else
    {
        QFile f(meta->audio);
        {
            quint32 size=qToBigEndian<quint32>(QFileInfo(f).size());
            dev->write((char*)&size,4);
        }
        f.open(QIODevice::ReadOnly);
        dev->write(f.readAll());//nincs kedvem rendesen megcsinálni
    }
    return true;
}

void QP4Handler::uncheckTree(Embed* e)
{
    for(int i=0;i<e->clip->getChildCount();++i)
    {
        uncheckTree(e->clip->child(i));
    }
    e->clip->unCheck();
}

bool QP4Handler::save2(Embed* root,QIODevice* dev)
{
    Clip& c=*root->clip;
    if(c.isChecked())return true;
    //save dependencies (children) first
    for(int i=0;i<c.getChildCount();++i)
    {
        if(!save2(c.child(i),dev))/*beállítja az errort*/return false;
    }
    //dependencies satisfied, yay
    if(c.name().isEmpty())
    {
        //don't save root item as clip
        return true;
    }

    QTextStream str(dev);
    str<<"beginclip("<<c.width()<<","<<c.height()<<",\""<<c.name()<<"\")\n";
    for(int i=0;i<c.getFrameCount();++i)
    {
        str<<"frame({\n";
        for(uint y=0;y<c.height();++y)
        {
            for(uint x=0;x<c.width();++x)
            {
                auto to_shorter=[](QRgb c)
                {
                    auto dec=[](QRgb c){return QString::number(c);};
                    auto hex=[](QRgb x)
                    {
                        return QString("0x")+=(QString::number(x,16));
                    };
                    //tipikusan 0xFF?????? számok jönnek, arra optimalizálok
                    //0xhex és dec közül a rövidebb:
                    if(c>=1000000000)return hex(c);
                    else if(c>=0x10000000)return dec(c);
                    else if(c>=100000000)return hex(c);
                    else if(c>=0x1000000)return dec(c);
                    else if(c>=10000000)return hex(c);
                    else if(c>=0x100000)return dec(c);
                    else if(c>=1000000)return hex(c);
                    else return dec(c);
                };
                str<<to_shorter(c.frame(i).pixelAt(x,y))<<",";
            }
            str<<"\n";//az utolsó vessző nem fáj a luának
        }
        str<<dec<<"},"<<c.frame(i).delay()<<")\n";
    }
    for(int i=0;i<c.getChildCount();++i)
    {
        Embed* e=c.child(i);
        str<<"embed(\""<<e->clip->name()<<"\",\n{";
        for(int u=0;u<=1;++u)
        {
            auto& a=(!u?e->x:e->y).values_;
            for(auto i=a.constBegin();i!=a.constEnd();++i)
            {
                str<<"["<<i.key()<<"]="<<i.value()<<",";
            }
            str<<(!u?"},\n{":"},");
        }
        str<<e->z<<","<<e->t<<")\n";
    }
    str<<"endclip()\n\n";
    c.check();
    return true;
}

bool QP4Handler::render(Embed* root,SceneTree::Metadata* meta,QIODevice* dev)
{
    //get main
    Clip* mainClip=NULL;
    for(int i=0;i<root->clip->getChildCount();++i)
    {
        if(root->clip->child(i)->clip->name()==SceneTree::MAIN_NAME)
        {
            mainClip=root->clip->child(i)->clip.data();
            break;
        }
    }
    Q_ASSERT(mainClip);
    dev->write("qpr v1\n");
    dev->write((meta->team+" - "+meta->title+"\n").toUtf8());
    dev->write("\n");//audio
    dev->write(QByteArray::number(mainClip->length())+"\n");

    QList<Frame> frames;
    for(uint i=0;i<mainClip->length();)
    {
        //egyforma képekből álló futamokat keresünk
        Frame first=Frame(SchWidget::WIDTH,SchWidget::HEIGHT,Frame::Black());
        first.setDelay(20);
        mainClip->render(&first,i,true);
        for(;;) //tömködjünk képeket az előzőbe
        {
            i+=20;
            if(i>=mainClip->length())break;

            Frame f=Frame(SchWidget::WIDTH,SchWidget::HEIGHT,Frame::Black());
            mainClip->render(&f,i,true);
            if(f==first)
            {
                first.setDelay(first.delay()+20);
            }
            else
            {
                //valami újdonság
                break;
            }
        }
        //az i most a következő másmilyen frame-re mutat (vagy a cucc vége után)
        QByteArray buf;
        for(uint y=0;y<first.height();++y)
        {
            for(uint x=0;x<first.width();++x)
            {
                buf.append(qRed(first.pixelAt(x,y)));
                buf.append(qGreen(first.pixelAt(x,y)));
                buf.append(qBlue(first.pixelAt(x,y)));
            }
        }
        dev->write(buf);
        quint32 delay=qToBigEndian<quint32>(first.delay());
        dev->write((char*)&delay,4);
        //megy egy kört a nagy ciklus és pont az i-t firsteli előbb
    }
    return true;
}
