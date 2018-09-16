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

#ifndef QP4HANDLER_H
#define	QP4HANDLER_H

#include "global.h"
#include "scenetree.h"
#include "clip.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define LUA_FUNCTION(name)\
    static int name##static_(lua_State* L)\
        {return((QP4Handler*)(lua_touserdata(L,lua_upvalueindex(1))))->name();}\
    int name();

/**
 * QP4 programokat kezel és futtat.
 */
class QP4Handler : public QObject
{
Q_OBJECT
public:
    static bool replaceRoot(SceneTree*,Clip*,QString* err);
private:
    static const char* const GENERATED_MARK;

    lua_State* L;
    //int function(lua_State*)
    LUA_FUNCTION(debug);
    LUA_FUNCTION(meta);
    LUA_FUNCTION(beginclip);
    LUA_FUNCTION(frame);
    LUA_FUNCTION(embed);
    LUA_FUNCTION(endclip);
    LUA_FUNCTION(rootclip);

    //state
    QString filename;
    QHash<QString,QExplicitlySharedDataPointer<Clip> > repository_;
    QExplicitlySharedDataPointer<Clip> currentClip_;

    static bool save2(Embed* root,QIODevice*);
    static void uncheckTree(Embed*);
public:
    SceneTree::Metadata metadata;
    QString error;
    QExplicitlySharedDataPointer<Clip> root;
    
    QP4Handler();
    ~QP4Handler();
    bool load(SceneTree*,QString,bool compressed);
    static bool save(Embed* root,SceneTree::Metadata*,QIODevice*);
    static bool exportAnim(Embed* root,SceneTree::Metadata*,QIODevice*);
    static bool render(Embed* root,SceneTree::Metadata*,QIODevice*);
signals:
    void debugLine(QString);
};

#endif
