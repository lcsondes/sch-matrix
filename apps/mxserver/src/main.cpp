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
#include "libmnp4/core/mcanimloader.h"
#include "libmnp4/core/mcconfig.h"
#include "muebserver.h"

struct CommandPrompt : public QThread
{
    CommandPrompt(){moveToThread(this);start();}
    void run()
    {
        QThread::msleep(100); //a hálózatnak kell idő, hogy listeneljen
        QTextStream out(stdout,QIODevice::WriteOnly);

        out<<tr("MXServer 2011 v0.1\n");
        out<<tr("Animpath: %1\n").arg
                (MCConfig::query(MCAnimLoader::PATH_QUERY).toString());
        out<<tr("Commands: exit")<<endl;
        
        QTextStream in(stdin,QIODevice::ReadOnly);
        QString line;
        auto prompt=[&in]()
        {
            //out<<"mxserver>";
            return in.readLine();
        };
        while(!(line=prompt()).isNull())
        {
            if(line.toLower()=="exit")
            {
                qDebug("Exiting...");
                MuebServer::shutdown();
                libmnp_shutdown();
                QCoreApplication::quit();
                return;
            }
        }
    }
};

static QSettings globalsettings("libmnp4.ini",QSettings::IniFormat);
static QSettings localsettings("mxserver.ini",QSettings::IniFormat);

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
    QCoreApplication app(argc,argv);
    
    libmnp_init(&readSetting);
    
    if(!MPFrame::WIDTH)
    {
        qCritical("MPFrame couldn't initialize - do you have a libmnp4.ini?");
        return 1;
    }
    
    libmnp_start_server();

    MuebServer::init();

    CommandPrompt p;
    return app.exec();
}