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

#include "libmnp4/core/mctime.h"
#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/packets/mptime.h"
#include "libmnp4/network/mnclient.h"

#include <algorithm>

/*
 * Az időszinkronizációról.
 *
 * A szinkronizációs folyamat méréseket végez, melyekkel megpróbálja
 * megállapítani a szerver és kliens órájának eltolódását, valamint a köztük
 * levő késleltetést.
 *
 * Egy mérés 3 fázisból áll:
 *
 * Első fázis: A kliens elküldi óraállását a szervernek.
 * Második fázis: A szerver elküldi a kapott óraállást és a saját óraállását
 * a kliensnek.
 * Harmadik fázis: A kliens régi óraállásának ismeretében kiszámítja az RTT-t
 * és az órák eltolódását, majd ezt visszaküldi a szervernek.
 *
 * A szerver a kapott mérési eredményeket aggregálja. Tizenegy beérkezett mérési
 * eredmény esetén a leggyorsabb (legalacsonyabb RTT-jű) három értéket
 * átlagolja, majd a kapott értékkel eltolja a saját óráját.
 *
 * Az ok, amiért a szerver állítgatja az óráját: Ha a kép csúszik el pár msec-kel,
 * azt észre sem lehet venni (nem fog bevillanni korábbi frame, így lett megírva
 * az MCAnimPlayer), viszont ha a hangot ugráltatnánk, azt hallani lehetne.
 */

DEFINE_MNP_INITIALIZABLE_STRICT(MCTime);

const int MCTime::MEASUREMENT_COUNT;

MCTime::MCTime()
    :lock_(QReadWriteLock::Recursive),sync_tick_(NULL),meas_ptr_(0)
{
    moveToThread(this);
    MCEventDispatcher::instance()->
            connect<MPTime>(this,MCEVENT_SLOT(packetReceived));
    delta_=-getTimeMsCore(); // 0-ról indítjuk a timert
    start();
}

MCTime::~MCTime()
{
    quit();
    wait();
    //delete sync_tick_; ~QObject() megoldja
}

/**
 * A globális MNClienten keresztül elkezdi az időszinkronizációt a szerverrel.
 * Meghívásakor az MNClientnek már futnia kell.
 */
void MCTime::startSyncing()
{
    Q_ASSERT(instance_);
    Q_ASSERT(!instance_->sync_tick_);
    QMetaObject::invokeMethod(instance_,"startSyncing2",Qt::QueuedConnection);
}

/**
 * A startSyncing() feladatát végzi el - a kettébontás oka, hogy ez a függvény
 * az MCTime szálán fut.
 */
void MCTime::startSyncing2()
{
    Q_ASSERT(!sync_tick_);
    sync_tick_=new QTimer(this);
    connect(sync_tick_,SIGNAL(timeout()),this,SLOT(syncTick()));
    sync_tick_->start(100);
}

/**
 * Elindítja az event loop-ot.
 */
void MCTime::run()
{
    exec();
}

/**
 * Visszaadja az óra értékét. Ez egy 1000 Hz-es számláló, mely 0-tól indul,
 * viszont a szerver átállíthatja a saját óráját a klienséhez. Ekkor akár
 * csökkenhet is az értéke két hívás között.
 */
qint64 MCTime::getTimeMs()
{
    Q_ASSERT(instance_);
    QReadLocker locker(&instance_->lock_);
    return instance_->getTimeMsCore()+instance_->delta_;
}

/**
 * Elindít egy mérést. A timer timeout() signal-jához van kötve.
 */
void MCTime::syncTick()
{
    MPTime* packet=new MPTime;
    packet->phase=1;
    packet->clienttime=getTimeMs();
    MNClient::globalInstance()->send(packet,MNClient::UDP);
}

/**
 * MPTime csomagokat kezel. Ha elég mérési eredmény gyűlt össze, meghívja az
 * evaluateMeasurements() függvényt, mely átállítja az órát. Normális esetben
 * ez csak a szerveroldalon történik meg.
 */
void MCTime::packetReceived(QSharedPointer<const MPacket> packet,
                            QHostAddress replyTo)
{
    const MPTime* p=packet.staticCast<const MPTime>().data();
    //qDebug("Incoming MPTime: %d,%lld,%lld",p->phase,p->field1,p->field2);
    switch(p->phase)
    {
        case 1: //szerver
        {
            //a feladat: a saját óránkat beleírni a csomagba és visszaküldeni
            MPTime response;
            response.phase=2;
            response.clienttime=p->clienttime;
            response.servertime=getTimeMs();
            response.sendViaUdp(replyTo);
        }break;
        case 2: //kliens
        {
            //A feladat: deltát és RTT-t számolni, majd visszaküldeni
            qint64 timeNow=getTimeMs();
            qint64 rtt=timeNow-p->clienttime;
            if(rtt<0)
            {
                qDebug("\t\t\tMCTime: Negative RTT (%d)! Dropping packet",
                       int(rtt));
                return;
            }
            qint64 serverNow=p->servertime+rtt/2; //becsült idő a szervernél
            qint64 delta=timeNow-serverNow;
            MPTime response;
            response.phase=3;
            response.delta=delta;
            response.rtt=rtt;
            response.sendViaUdp(replyTo);
        }break;
        case 3: //szerver
        {
            //érkezett egy mérési eredmény, kiértékeljük jól
            QWriteLocker locker(&lock_);
            measurements_[meas_ptr_++]={p->delta,p->rtt};
            if(meas_ptr_==MEASUREMENT_COUNT)
            {
                evaluateMeasurements();
            }
        }break;
        default:
            qWarning("Invalid MPTime packet got to MCTime!");
            break;
    }
}

/**
 * Kiértékeli a beérkezett mérési eredményeket, és átállítja az órát.
 */
void MCTime::evaluateMeasurements()
{
    QWriteLocker locker(&lock_);
    if(meas_ptr_!=MEASUREMENT_COUNT)return; //valami szálazós móka van

    //összeátlagoljuk a 3 leggyorsabb eredményét oszt jóvan
    static const int N_AVG=3;Q_ASSERT(N_AVG<=MEASUREMENT_COUNT);

    //Note: A qSort() egy qsort, de az std::sortban valami irdatlan mennyiségű
    //magic van. Általában 2-3x gyorsabban lefut.
    std::sort(measurements_,measurements_+MEASUREMENT_COUNT,
              [](const Measurement& a,const Measurement& b)
              {
                  return a.rtt<b.rtt;
              });

    qint64 delta=0;
    for(int i=0;i<N_AVG;++i)delta+=measurements_[i].delta;
    delta/=N_AVG;
    qDebug("MCTime adjusted by %d ms",int(delta));
    delta_+=delta;
    meas_ptr_=0; //új kör
}
