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

#include "qpaloader.h"
#include "qp4handler.h"

#define QPAWIDTH 16
#define QPAHEIGHT 13

//3 szín megadása guin
//TODO low priority: file->new

bool QpaLoader::load(SceneTree* st,QString fileName,QString* error,
                     QRgb a,QRgb b,QRgb c)
{
    SceneTree::Metadata meta;
    //alap Clip létrehozás(full-SCH)
    QExplicitlySharedDataPointer<Clip> retval(new Clip(32,26));
    retval->setName(SceneTree::MAIN_NAME);

    QFile qpaFile(fileName);
    qDebug()<<qpaFile.fileName();
    qpaFile.open(QIODevice::ReadOnly);
    QTextStream input(&qpaFile);
    input.setCodec(QTextCodec::codecForName("UTF-8"));

    QString line;

    //innentől fájl kiolvasása:
    line=input.readLine();
    qDebug()<<line;
    while((line=input.readLine())!="-- o --"&&!input.atEnd())
    {
        QStringList data=line.split('=');
        if(data[0]=="cim")meta.title=data[1];
        if(data[0]=="csapat")meta.team=data[1];
        if(data[0]=="qpa")meta.year=data[1].split('.')[0].toInt()+1971;
        if(data[0]=="audio")meta.audio=QFileInfo(QFileInfo(fileName).
                                                     canonicalPath()+"/"+data[1]
                                                ).canonicalFilePath();
    }
    for(;;)
    {
        Frame newFrame(32,26);
        for(int y=0;y<QPAHEIGHT;y++)
        {
            line=input.readLine();
            if(line.isNull())goto err;
            if(line=="-- o --")goto end;
            line.append("................");

            //meg kell négyszerezni minden pixelt

            for(int x=0;x<QPAWIDTH;x++)
            {
                QRgb value;
                switch(line[x].toAscii())
                {
                    case '.':
                        value=a;
                        break;
                    case 'x':
                        value=b;
                        break;
                    case 'X':
                        value=c;
                        break;
                    default:
                        goto err;
                }
                newFrame.pixelAt(x*2  ,y*2  )=value;
                newFrame.pixelAt(x*2  ,y*2+1)=value;
                newFrame.pixelAt(x*2+1,y*2  )=value;
                newFrame.pixelAt(x*2+1,y*2+1)=value;
            }
        }
        int delay;
        input>>delay;
        input.readLine();
        newFrame.setDelay(delay);
        retval->addFrame(newFrame);
    }
end:;
    {
        Embed* mainembed=new Embed;
        mainembed->clip=retval;

        Clip* root=new Clip(0,0);
        root->addChild(mainembed);
        if(!QP4Handler::replaceRoot(st,root,error))
        {
            delete mainembed;
            return false;
        }
        st->metadata()->operator=(meta);
        delete mainembed;
        return true;
    }
err:;
    if(error)*error=QObject::tr("Unspecified error in QPA importer.");
    return false;
}
