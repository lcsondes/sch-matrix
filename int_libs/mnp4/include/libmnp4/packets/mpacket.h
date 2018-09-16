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

#ifndef LIBMNP_MPACKET_H
#define	LIBMNP_MPACKET_H

#include "libmnp4/global.h"

/**
 * Minden hálózati csomag absztrakt ősosztálya. Tud szerializálódni,
 * és deszerializálódni QIODevice-okon keresztül, továbbá kényelmi funkcióként
 * el tudja magát küldeni UDP-csomagként egy megadott címre.
 * A leszármazott osztályok deklarációjában el kell helyezni egy
 * DECLARE_MNP_PACKET makrót, a .cpp fájlban pedig egy DEFINE_MNP_PACKET makrót,
 * továbbá egyedi ID-vel és default konstruktorral kell rendelkezniük.
 */
class MNP4_EXPORT MPacket
{
    ///A leszármazott osztályok létrehozófüggvényei
    static QHash<QByteArray,MPacket*(*)(void)>* creators_;

protected:
    static void register_subclass(QByteArray,MPacket*(*)(void));

    QByteArray id_; ///<A csomag MNP4 azonosítója (pl. PING)
    MPacket(const QByteArray&);
    /**
     * A csomag felépítését végzi a megadott tömbből.
     */
    virtual bool load2(QByteArray)=0;
    /**
     * A csomag adatrészét állítja elő (azaz az MNP4<id><méret> utáni részt).
     */
    virtual QByteArray save2()=0;
public:
    virtual ~MPacket();
    QByteArray id();
    void save(QIODevice*);
    static MPacket* load(QIODevice*);
    void sendViaUdp(QHostAddress);
};

//%c%c%c%c-ként íródik ki, debugoláshoz hasznos
#define DEBUG_MPACKET_cccc(p) (p).id().constData()[0],\
    (p).id().constData()[1],\
    (p).id().constData()[2],\
    (p).id().constData()[3]

/**
 * MNP csomag deklarációja. Szükséges ahhoz, hogy a deszerializáció működjön.
 */
//csinál egy objektumot, lehúzza belőle az id-t és beregisztrálja jól
#define DECLARE_MNP_PACKET(name) \
    static struct _smnp##name{static MPacket* create();\
        _smnp##name(){name* p=new name;\
        MPacket::register_subclass(p->id(),&create);\
        delete p;}} _smnp

/**
 * MNP csomag implementációja. Statikus időben (még a main()) előtt elvégez
 * minden regisztrációt, hogy a deszerializáció felismerje az osztályt.
 */
//példányosítja az _smnp##name segédosztályt, ami elvégzi a regisztrációt
#define DEFINE_MNP_PACKET(name) \
        MPacket* name::_smnp##name::create(){return new name;}\
        name::_smnp##name name::_smnp

#endif
