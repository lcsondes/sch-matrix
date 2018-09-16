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
#include "libmnp4/init.h"
#include "mainwindow.h"

/*
 * Kliens-specifikus MCConfig-beállítások:
 * mxclient.initial_dir - nem kötelező, a playlistet feltölti innen minden induláskor
 * mxcleint.server_ip - nem kötelező, a szerver IP címét lehet előre kitöltetni vele 
 */

#ifdef MTX_QT_IS_STATIC
Q_IMPORT_PLUGIN(qsvg);
#endif

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

void messageHandler(QtMsgType type,const char* msg)
{
    switch(type)
    {
        case QtDebugMsg:
        case QtWarningMsg:
            fprintf(stderr,"%s\n",msg);
            return;
        case QtCriticalMsg:
        case QtFatalMsg:
            fprintf(stderr,"!!!> %s\n",msg);
#ifdef Q_OS_WIN32
            //QMessageBox beszól, mert potenciálisan rossz szálon vagyunk
            MessageBoxA(NULL,msg,"Halalvan==true",MB_OK);
#endif
            QApplication::quit();
    }
}

static QSettings globalsettings("libmnp4.ini",QSettings::IniFormat);
static QSettings localsettings("mxclient.ini",QSettings::IniFormat);

static QVariant readSetting(QString key)
{
    qDebug()<<"Reading setting "<<key;
    key.replace('.','/');
    QVariant retval=localsettings.value(key);
    if(retval.isNull())
    {
        retval=globalsettings.value(key);
    }
    return retval;
}

int main(int argc,char** argv)
{
    QApplication app(argc,argv);
    //qInstallMsgHandler(&messageHandler);
    
    libmnp_init(&readSetting);
    
    MainWindow m;
    
    if(!MPFrame::WIDTH)
    {
        QMessageBox::critical(&m,QObject::tr("libmnp4 error"),
                              QObject::tr("Couldn't initialize MPFrame--"
                                           "do you have a libmnp4.ini?"));
        return 1;
    }


    return app.exec();
}
