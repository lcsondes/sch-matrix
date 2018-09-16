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

#ifndef LIBMNP_INIT_H
#define	LIBMNP_INIT_H

#include "libmnp4/global.h"

void MNP4_EXPORT libmnp_init(QVariant(*)(QString));
void MNP4_EXPORT libmnp_shutdown();
void MNP4_EXPORT libmnp_start_server();
void MNP4_EXPORT libmnp_start_client(const QHostAddress&);

//megírják helyetted a singleton részt

/**
 * Singletont deklarál. Jó, ha egy osztályból csak egyetlen globális példányt szeretnél.
 * A megírd statikus függvények:
 * void init() - létrehozza a globális objektumot, ha még nem létezik
 * void shutdown() - törli a globális objektumot
 * name* instance() - visszaadja a globális objektumot
 */
#define DECLARE_MNP_INITIALIZABLE(name) private:static name* instance_;\
    public:static void init();static void shutdown();static name* instance();\
    private:

/**
 * Megírja a singletont, szigorú inicializációval.
 * Ez annyit tesz, hogy az instance() nem hozza létre az objektumot, ha nincs még létrehozva.
 */
#define DEFINE_MNP_INITIALIZABLE_STRICT(name) name* name::instance_=NULL;\
    void name::shutdown(){delete instance_;}\
    void name::init(){Q_ASSERT(!instance_);instance_=new name;}\
    name* name::instance(){Q_ASSERT(instance_);return instance_;}

/**
 * Megírja a singletont, automatikus inicializációval.
 * Az init() működik, de az instance() is automatikusan elvégzi ezt, ha előszörre hívod.
 */
//jobb a libmnp_* függvényekbe tenni explicit inicializációt, kerüld ezt
/*#define DEFINE_MNP_INITIALIZABLE(name) name* name::instance_=NULL;\
    void name::shutdown(){delete instance_;}\
    void name::init(){Q_ASSERT(!instance_);instance_=new name;}\
    name* name::instance(){if(!instance_)init();return instance_;}*/

#endif
