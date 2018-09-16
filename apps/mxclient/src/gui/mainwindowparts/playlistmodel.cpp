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

#include "playlistmodel.h"

#include <QtIOCompressor>

#include "libmnp4/core/mcconfig.h"
#include "libmnp4/packets/mpframe.h"

const QString PlayListModel::INITDIR_QUERY("mxclient.initial_dir");

PlayListModel::PlayListModel()
{
    FMOD_System_Create(&fmod_);
    FMOD_System_Init(fmod_,0,FMOD_INIT_NORMAL,NULL);
    QString initial_dir=MCConfig::query(INITDIR_QUERY).toString();

    QStringList filters;
    filters<<"*.q4x";
    QDirIterator iter(initial_dir,filters);
    while(iter.hasNext())
    {
        addFile(iter.next());
    }
}

PlayListModel::~PlayListModel()
{
    FMOD_System_Release(fmod_);
}

int PlayListModel::rowCount(const QModelIndex&) const
{
    return entries_.size();
}

int PlayListModel::columnCount(const QModelIndex&) const
{
    return 4;
}

QVariant PlayListModel::headerData(int section,Qt::Orientation orientation,
                                   int role) const
{
    if(orientation!=Qt::Horizontal)return QVariant();
    if(role!=Qt::DisplayRole)return QVariant();
    switch(section)
    {
        case 0:return QVariant(tr("Filename"));
        case 1:return QVariant(tr("Title"));
        case 2:return QVariant(tr("Length"));
        case 3:return QVariant(tr("Now playing"));
        default:return QVariant();
    }
}

QVariant PlayListModel::data(const QModelIndex& idx,int role) const
{
    if(role!=Qt::DisplayRole)return QVariant();
    if(idx.row()<0||idx.row()>=entries_.size())return QVariant();
    switch(idx.column())
    {
        case 0:return QFileInfo(entries_[idx.row()].filename).fileName();
        case 1:return entries_[idx.row()].title;
        case 2:return ::hms(entries_[idx.row()].length);
        case 3:switch(entries_[idx.row()].flags)
        {
            case Entry::NOTHING:return QString();
            case Entry::NOW_PLAYING:return tr("Now playing");
            case Entry::NEXT:return tr("Next");
            default:return "???";
        }
        default:return QVariant();
    }
}

void PlayListModel::addFile(QString file)
{
    QFile f(file);
    if(!f.exists())return;
    f.open(QIODevice::ReadOnly);
    //Formátum: Q4X1[w:2][h:2][méret:4]Q4Z[méret:4]QPRZ[méret:4]hang
    if(f.read(4)!="Q4X1")
    {
        qDebug()<<file<<" !Q4X1";
        return;
    }
    {
        quint16 w,h;
        f.read((char*)&w,2);
        f.read((char*)&h,2);
        w=qFromBigEndian<quint16>(w);
        h=qFromBigEndian<quint16>(h);
        if(w!=MCConfig::query(MPFrame::WIDTH_QUERY) ||
           h!=MCConfig::query(MPFrame::HEIGHT_QUERY))
        {
            //nem ehhez a felálláshoz készült
            qDebug()<<file<<" !wh";
            return;
        }
    }
    quint64 sound_offset;
    {
        quint32 size;
        f.read((char*)&size,4);
        size=qFromBigEndian<quint32>(size);
        f.seek(f.pos()+size);

        f.read((char*)&size,4);
        size=qFromBigEndian<quint32>(size);
        
        sound_offset=f.pos()+size;
        qDebug()<<"Sound offset="<<sound_offset;
    }
    QString title;
    unsigned int length;
    {
        QtIOCompressor comp(&f);
        comp.open(QIODevice::ReadOnly);
    
        comp.readLine(); //skip "qpr v1"
        title=QString::fromUtf8(comp.readLine()).trimmed();
        comp.readLine(); //skip sound
        length=comp.readLine().trimmed().toUInt();
    }
    f.seek(sound_offset);
    //ha a hang hosszabb, akkor az az animáció hossza
    {
        quint32 sound_length;
        f.read((char*)&sound_length,4);
        sound_length=qFromBigEndian<quint32>(sound_length);
        qDebug()<<"Sound length:"<<sound_length;
        QByteArray sounddata=f.read(sound_length);
        
        FMOD_SOUND* fsound=NULL;
        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo,0,sizeof(exinfo));
        exinfo.cbsize=sizeof(exinfo);
        exinfo.length=sound_length;
        FMOD_System_CreateStream(fmod_,sounddata.constData(),
                                 FMOD_ACCURATETIME|FMOD_OPENMEMORY_POINT|
                                     FMOD_CREATECOMPRESSEDSAMPLE|FMOD_SOFTWARE,
                                 &exinfo,&fsound);
        unsigned int temp=0;
        FMOD_Sound_GetLength(fsound,&temp,FMOD_TIMEUNIT_MS);
        qDebug()<<"Length from qpr"<<length<<"ms ="<<qPrintable(::hms(length))
                <<", from sound"<<temp<<"ms ="<<qPrintable(::hms(temp));
        if(length<temp)length=temp;
        FMOD_Sound_Release(fsound);
    }
    beginInsertRows(QModelIndex(),entries_.size(),entries_.size());
    entries_.append({file,title,length,Entry::NOTHING});
    endInsertRows();
}

void PlayListModel::moveUp(int idx)
{
    move(idx,-1);
}

void PlayListModel::moveDown(int idx)
{
    move(idx,+1);
}

void PlayListModel::move(int idx,int dir)
{
    if(idx<0||idx+dir<0||idx>=entries_.size()||idx+dir>=entries_.size())return;
    //felfele -1, lefele +2, mert a moveRows() elemközökben gondolkodik
    beginMoveRows(QModelIndex(),idx,idx,QModelIndex(),idx+dir+(dir>0));
    entries_.swap(idx,idx+dir);
    endMoveRows();
}

void PlayListModel::remove(int idx)
{
    if(idx<0||idx>=entries_.size())return;
    beginRemoveRows(QModelIndex(),idx,idx);
    entries_.removeAt(idx);
    endRemoveRows();
}

void PlayListModel::setNext(const QModelIndex& idx)
{
    int row=idx.row();
    if(row<0||row>=entries_.size())return;

    Entry& e=entries_[row];
    if(e.flags!=Entry::NOW_PLAYING)
    {
        for(int i=0;i<entries_.size();++i)
        {
            Entry& f=entries_[i];
            if(f.flags==Entry::NEXT)
            {
                f.flags=Entry::NOTHING;
                emit dataChanged(createIndex(i,3),
                                 createIndex(i,3));
                break;
            }
        }
        e.flags=Entry::NEXT;
        emit dataChanged(createIndex(row,3),
                         createIndex(row,3));
    }
}

const PlayListModel::Entry* PlayListModel::currentEntry() const
{
    int nowPlayingIdx=-1;
    for(int i=0;i<entries_.size();++i)
    {
        if(entries_[i].flags==Entry::NOW_PLAYING)
        {
            nowPlayingIdx=i;
            break;
        }
    }
    if(nowPlayingIdx>=0)
    {
        return &entries_[nowPlayingIdx];
    }
    return NULL;
}

QString PlayListModel::currentFile() const
{
    const Entry* current=currentEntry();
    return current?current->filename:QString();
}

quint32 PlayListModel::currentLength() const
{
    const Entry* current=currentEntry();
    return current?current->length:0;
}

void PlayListModel::currentAnimEnded()
{
    int nowPlayingIdx=-1;
    for(int i=0;i<entries_.size();++i)
    {
        if(entries_[i].flags==Entry::NOW_PLAYING)
        {
            nowPlayingIdx=i;
            break;
        }
    }
    if(nowPlayingIdx>=0)
    {
        entries_[nowPlayingIdx].flags=Entry::NOTHING;
        emit dataChanged(createIndex(nowPlayingIdx,3),
                         createIndex(nowPlayingIdx,3));
    }
}

void PlayListModel::rewindCurrentAnim()
{
    for(int i=0;i<entries_.size();++i)
    {
        if(entries_[i].flags==Entry::NOW_PLAYING)
        {
            entries_[i].flags=Entry::NEXT;
            emit dataChanged(createIndex(i,3),
                             createIndex(i,3));
        }
        else if(entries_[i].flags==Entry::NEXT)
        {
            entries_[i].flags=Entry::NOTHING;
            emit dataChanged(createIndex(i,3),
                             createIndex(i,3));
        }
    }
}

void PlayListModel::selectNextAnim()
{
    int nextIdx=0;
    for(int i=0;i<entries_.size();++i)
    {
        if(entries_[i].flags==Entry::NEXT)
        {
            nextIdx=i;
            break;
        }
    }
    if(!entries_.isEmpty())
    {
        entries_[nextIdx].flags=Entry::NOW_PLAYING;
        emit dataChanged(createIndex(nextIdx,3),
                         createIndex(nextIdx,3));
        if(nextIdx+1<entries_.size())
        {
            entries_[nextIdx+1].flags=Entry::NEXT;
            emit dataChanged(createIndex(nextIdx+1,3),
                             createIndex(nextIdx+1,3));
        }
    }
}

void PlayListModel::moveNext(int dir)
{
    int nextIdx=-1;
    for(int i=0;i<entries_.size();++i)
    {
        if(entries_[i].flags==Entry::NEXT)
        {
            nextIdx=i;
            break;
        }
    }
    if(nextIdx>=0&&nextIdx<entries_.size())
    {
        entries_[nextIdx].flags=Entry::NOTHING;
        emit dataChanged(createIndex(nextIdx,3),
                         createIndex(nextIdx,3));
    }
retry:;
    nextIdx+=dir;
    if(nextIdx>=0&&nextIdx<entries_.size())
    {
        if(entries_[nextIdx].flags!=Entry::NOW_PLAYING)
        {
            entries_[nextIdx].flags=Entry::NEXT;
            emit dataChanged(createIndex(nextIdx,3),
                             createIndex(nextIdx,3));
        }
        else
        {
            //az épp lejátszás alatt levő számot átugorjuk
            goto retry;
        }
    }
}

void PlayListModel::moveNextForward()
{
    moveNext(+1);
}

void PlayListModel::moveNextBackward()
{
    moveNext(-1);
}
