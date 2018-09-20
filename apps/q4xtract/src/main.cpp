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

#include <QtCore>
#include "qtiocompressor.h"

bool extract(const char* filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	if(f.read(4)!="Q4X2")return false;
	f.read(4); //width, height
	quint32 size;
	for(int i=0;i<=1;++i)
	{
		f.read((char*)&size,4);
		size=qFromBigEndian<quint32>(size);
		{
			QFile out(QString(filename).replace(QRegExp("q4x$"),i?"qpr":"qp4"));
			out.open(QIODevice::WriteOnly);
            QByteArray data=f.read(size);
			QBuffer b(&data);
			b.open(QIODevice::ReadOnly);
			QtIOCompressor comp(&b);
			comp.open(QIODevice::ReadOnly);
			out.write(comp.readAll());
		}
	}
	f.read((char*)&size,4);
	size=qFromBigEndian<quint32>(size);
    //TODO extension - az FMOD autodetectel
	QFile out(QString(filename).replace(QRegExp("q4x$"),"mp3"));	out.open(QIODevice::WriteOnly);
	out.write(f.read(size));
    return true;
}

int main(int argc,char** argv)
{
	for(int i=1;i<argc;++i)
	{
		extract(argv[i]);
	}
	return 0;
}