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

#include "global.h"
#include "mainwindow.h"
#include "schwidget.h"
#include "libmnp4/init.h"
#include "libmnp4/core/mctime.h"

#ifdef MTX_QT_IS_STATIC
Q_IMPORT_PLUGIN(qsvg);
#endif

bool noroundtohw=false;
bool noendworkaround=false;
bool sideaudio=false;
static const int SCH_WIDTH_PX=32;
static const int SCH_HEIGHT_PX=26;

//az animeditor nem kap ini fájlt, mert 1 exébe akarjuk összenyomni

static QVariant readSetting(QString key)
{
    if(key=="libmnp4.frame.width")
    {
        return SCH_WIDTH_PX;
    }
    else if(key=="libmnp4.frame.height")
    {
        return SCH_HEIGHT_PX;
    }
    else return QVariant();
}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    {
    bool english=false;
    for(int i=1;i<argc;++i)
    {
        if(QString(argv[i])=="-noendworkaround")
        {
            noendworkaround=true;
        }
        if(QString(argv[i])=="-noroundtohw")
        {
            noroundtohw=true;
        }
        if(QString(argv[i])=="-sideaudio")
        {
            sideaudio=true;
        }
        if(QString(argv[i])=="-english")
        {
            english=true;
        }
    }

    if(!english)
    {
        QTranslator qtTrans;
        qtTrans.load(":/qt_hu");
        app.installTranslator(&qtTrans);
        
        QTranslator myTrans;
        myTrans.load(":/animeditor_hu");
        app.installTranslator(&myTrans);
    }
    }

    libmnp_init(&readSetting);
    MCTime::init();
    SchWidget::init();
    MainWindow m;
    return app.exec();
}
