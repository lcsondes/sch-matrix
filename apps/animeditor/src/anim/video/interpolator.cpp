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

#include "interpolator.h"

//FIXME KIPUCOLNI MINDEN FLOAT/DOUBLE DOLGOT A KÓDBÓL, MERT PROCIFÜGGŐ

uint qHash(const Interpolator& i)
{
    uint retval=1;
    for(auto iter=i.values_.constBegin();iter!=i.values_.constEnd();++iter)
    {
        retval+=100003;
        retval*=iter.key()+1;
        retval=retval<<13^retval>>13;
    }
    return retval;
}

bool Interpolator::operator==(const Interpolator& o) const
{
    return values_==o.values_;
}

int Interpolator::value(int t) const
{
    Q_ASSERT(!values_.isEmpty());
    auto after=values_.lowerBound(t);
    if(after==values_.constEnd())return (--after).value();
    double b=after.value();
    if(after.key()==t||after==values_.constBegin())return b;
    auto before=after-1;

    double a=before.value();
    int ta=before.key(),tb=after.key();
    double factor=double(t-ta)/(tb-ta);
    int retval=a*(1-factor)+b*factor;
    retval&=~1; //csak ablakra mehet
    return retval;
}

void Interpolator::setValue(int t,int v)
{
    values_[t]=v;
}

int Interpolator::nextKey(int t) const
{
    if(values_.isEmpty())return 0;
    for(auto i=values_.constBegin();i!=values_.constEnd();++i)
    {
        if(i.key()>t)return i.key();
    }
    return (--values_.constEnd()).key();
}

int Interpolator::prevKey(int t) const
{
    if(values_.isEmpty())return 0;
    int lastKey;bool havelastkey=false;
    for(auto i=values_.constBegin();i!=values_.constEnd();++i)
    {
        if(i.key()>=t)break;
        lastKey=i.key();havelastkey=true;
    }
    if(!havelastkey)return 0;
    return lastKey;
}

void Interpolator::deltaShift(int d)
{
    if(values_.isEmpty())return;
    for(auto i=values_.begin();i!=values_.end();++i)
    {
        i.value()=i.value()+d;
    }
}

void Interpolator::remove(int t)
{
    values_.remove(t);
}

void Interpolator::clear()
{
    values_.clear();
}

void Interpolator::debugprint()
{
    qDebug()<<"Interpolator:\n{";
    for(auto i=values_.constBegin();i!=values_.constEnd();++i)
    {
        qDebug()<<i.key()<<"->"<<i.value();
    }
    qDebug()<<"}";
}
