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
#include "schwindow.h"

//spóroljuk meg a tr()-t itt, és minden error legyen magyarul.

#define LUA_ERROR(err) do{lua_pushstring(L,err);lua_error(L);}while(0)

/**
 * debug(string)
 */
int QP4Handler::debug()
{
    const char* msg=luaL_checkstring(L,1);
    emit debugLine(QString(msg).append('\n'));
    return 0;
}

/**
 * meta(table(string->any) data)
 * kulcsok: audio, team, title, year
 */
int QP4Handler::meta()
{
    lua_pushnil(L);
    while(lua_next(L,1)!=0)
    {
        lua_pushvalue(L,-2);//kulcs duplázása, mert a tostring konvertál
        QString idx=lua_tostring(L,-1);
        lua_pop(L,1);

        if(idx=="audio")
        {
            metadata.audio=/*QFileInfo(filename).canonicalPath()+"/"+*/
                           QString::fromUtf8(lua_tostring(L,-1));
        }
        else if(idx=="team")
        {
            metadata.team=QString::fromUtf8(lua_tostring(L,-1));
        }
        else if(idx=="title")
        {
            metadata.title=QString::fromUtf8(lua_tostring(L,-1));
        }
        else if(idx=="year")
        {
            metadata.year=lua_tointeger(L,-1);
        }
        else
        {
            qDebug()<<"meta wtf";
        }
        lua_pop(L,1);
    }
    return 0;
}

/**
 * beginclip(int w,int h,string name)
 */
int QP4Handler::beginclip()
{
    if(currentClip_)LUA_ERROR("Nem lehet beginclip()-eket egymásba ágyazni! "
                              "Hívj endclip()-et előbb!");
    int w,h;
    const char* name;
    w=luaL_checkinteger(L,1);
    h=luaL_checkinteger(L,2);
    name=luaL_checkstring(L,3);
    currentClip_=new Clip(w,h,name);
    return 0;
}

/**
 * endclip()
 */
int QP4Handler::endclip()
{
    if(!currentClip_)LUA_ERROR("endclip()-et beginclip() után kell hívni!");
    if(repository_[currentClip_->name()])
    {
        LUA_ERROR("Ilyen nevű clip már van!");
    }
    repository_[currentClip_->name()]=currentClip_;
    currentClip_=NULL;
    return 0;
}

/**
 * rootclip(name)
 */
int QP4Handler::rootclip()
{
    QString name=luaL_checkstring(L,1);
    if(!repository_[name])LUA_ERROR("Nem létezik ilyen nevű clip még!");
    Embed* e=new Embed;
    e->clip=repository_[name];
    e->x.setValue(0,0);
    e->y.setValue(0,0);
    e->z=-1;
    e->t=0;
    root->addChild(e);
    return 0;
}

/**
 * frame(int[] pixels,int delay)
 */
int QP4Handler::frame()
{
    if(!currentClip_)LUA_ERROR("frame() csak beginclip() és endclip() "
                               "között hívható!");
    int delay=luaL_checkinteger(L,2);
    QMap<int,QRgb> data;
    lua_pushnil(L);
    while(lua_next(L,1)!=0)
    {
        lua_pushvalue(L,-2);//kulcs duplázása, mert a tointeger konvertál
        lua_Integer idx=lua_tointeger(L,-1);
        lua_pop(L,1);

        lua_Integer value=lua_tointeger(L,-1);
        data[idx]=value;
        lua_pop(L,1);
    }
    Frame f(currentClip_->width(),currentClip_->height());
    if(uint(data.size())!=f.width()*f.height())
    {
        LUA_ERROR("Nincs megfelelő számú pixel a frame() hívásban!");
    }
    auto i=data.constBegin();
    for(uint y=0;y<f.height();++y)
    {
        for(uint x=0;x<f.width();++x)
        {
            f.pixelAt(x,y)=*(i++);
        }
    }
    f.setDelay(delay);
    currentClip_->addFrame(f);
    return 0;
}

/**
 * embed(string name,table(int->int) x,table(double->int) y,int z,int t)
 */
int QP4Handler::embed()
{
    if(!currentClip_)LUA_ERROR("embed() csak beginclip() és endclip() "
                               "között hívható!");
    QString name;
    Interpolator x,y;
    int z,t;
    name=luaL_checkstring(L,1);
    if(!repository_[name])LUA_ERROR("Nem létezik ilyen nevű clip még!");
    for(int i=2;i<=3;++i)
    {
        lua_pushnil(L);
        while(lua_next(L,i)!=0)
        {
            lua_pushvalue(L,-2);//kulcs duplázása, mert a tointeger konvertál
            lua_Integer idx=lua_tointeger(L,-1);
            lua_pop(L,1);

            lua_Integer value=lua_tointeger(L,-1);
            (i==2?x:y).setValue(idx,value);
            lua_pop(L,1);
        }
    }
    z=luaL_checkinteger(L,4);
    t=luaL_checkinteger(L,5);

    Embed* embed=new Embed;
    embed->clip=repository_[name];
    embed->x=x;
    embed->y=y;
    embed->z=z;
    embed->t=t;
    currentClip_->addChild(ownership embed);
    
    return 0;
}
