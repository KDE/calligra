/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTMAP_H
#define KPTMAP_H


#include <qmap.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qpair.h>
#include <qvaluelist.h>

#include <kdebug.h>

namespace KPlato
{

namespace KPTMap {
enum State { None=0, NonWorking=1, Working=2 };
}

typedef QMap<QString, int> KPTDateMapType;
class KPTDateMap : public KPTDateMapType
{
public:
    KPTDateMap() {}
    virtual ~KPTDateMap() {}

    virtual bool contains(QDate date) const { return KPTDateMapType::contains(date.toString(Qt::ISODate)); }

    void insert(QString date, int state=KPTMap::NonWorking) {
        //kdDebug()<<k_funcinfo<<date<<"="<<state<<endl;
        if (state == KPTMap::None)
            KPTDateMapType::remove(date);
        else
            KPTDateMapType::insert(date, state);
    }
    void insert(QDate date, int state=KPTMap::NonWorking) { insert(date.toString(Qt::ISODate), state); }

    void remove(QDate date) {
        //kdDebug()<<k_funcinfo<<date.toString(Qt::ISODate)<<endl;
        KPTDateMapType::remove(date.toString(Qt::ISODate));
    }

    int state(QString date) {
        KPTDateMapType::iterator it = find(date);
        if (it == end()) return 0;
        else return it.data();
    }
    int state(QDate date) { return state(date.toString(Qt::ISODate)); }

    bool operator==(const KPTDateMap &m) const { 
        return keys() == m.keys() && values() == m.values(); 
    }
    bool operator!=(const KPTDateMap &m) const { 
        return keys() != m.keys() || values() != m.values(); 
    }
    
    // boolean use
    void toggle(QString date, int state=KPTMap::NonWorking) {
        //kdDebug()<<k_funcinfo<<date<<"="<<state<<endl;
        if (KPTDateMapType::contains(date))
            KPTDateMapType::remove(date);
        else
            KPTDateMapType::insert(date, state);
    }
    void toggle(QDate date, int state=KPTMap::NonWorking) { return toggle(date.toString(Qt::ISODate)); }
    void toggleClear(QString date, int state=KPTMap::NonWorking) {
        //kdDebug()<<k_funcinfo<<date<<"="<<state<<endl;
        bool s = KPTDateMapType::contains(date);
        clear();
        if (!s) insert(date, state);
    }
    void toggleClear(QDate date, int state=KPTMap::NonWorking) { toggleClear(date.toString(Qt::ISODate)); }
};

typedef QMap<int, int> KPTIntMapType;
class KPTIntMap : public KPTIntMapType
{
public:
    KPTIntMap() {}
    virtual ~KPTIntMap() {}

    void insert(int key, int state=KPTMap::NonWorking) {
        if (state == KPTMap::None)
            KPTIntMapType::remove(key);
        else
            KPTIntMapType::insert(key, state); }

    virtual int state(int key) {
        KPTIntMapType::iterator it = KPTIntMapType::find(key);
        if (it == KPTIntMapType::end()) return 0;
        else return it.data();
    }

    bool operator==(const KPTIntMap &m) const { 
        return keys() == m.keys() && values() == m.values(); 
    }
    bool operator!=(const KPTIntMap &m) const { 
        return keys() != m.keys() || values() != m.values(); 
    }
    
    // boolean use
    void toggle(int key, int state=KPTMap::NonWorking) { KPTIntMapType::contains(key) ? remove(key) : insert(key, state); }
    void toggleClear(int key, int state=KPTMap::NonWorking) {
        bool s =contains(key);
        clear();
        if (!s) insert(key, state);
    }
};

class KPTWeekMap : public KPTIntMap
{
public:
    bool contains(int week, int year) { return KPTIntMap::contains(week*10000 + year); }
    bool contains(QPair<int,int> week) { return contains(week.first,  week.second); }

    void insert(int week, int year, int state=KPTMap::NonWorking) {
        if (week < 1 || week > 53) { kdError()<<k_funcinfo<<"Illegal week number: "<<week<<endl; return; }
        KPTIntMap::insert(week*10000 + year, state);
    }
    void insert(QPair<int,int> week, int state=KPTMap::NonWorking) { insert(week.first, week.second, state); }

    void insert(KPTWeekMap::iterator it, int state) { insert(week(it.key()), state); }

    void remove(QPair<int,int> week) { KPTIntMap::remove(week.first*10000 + week.second); }

    static QPair<int, int> week(int key) { return QPair<int, int>(key/10000, key%10000); }

    int state(QPair<int, int> week) { return KPTIntMap::state(week.first*10000 + week.second); }
    int state(int week, int year) { return state(QPair<int, int>(week, year)); }

    void toggle(QPair<int,int> week, int state=KPTMap::NonWorking) {
        if (week.first < 1 || week.first > 53) { kdError()<<k_funcinfo<<"Illegal week number: "<<week.first<<endl; return; }
        KPTIntMap::toggle(week.first*10000 + week.second, state);
    }
    void toggleClear(QPair<int,int> week, int state=KPTMap::NonWorking) {
        if (week.first < 1 || week.first > 53) { kdError()<<k_funcinfo<<"Illegal week number: "<<week.first<<endl; return; }
        KPTIntMap::toggleClear(week.first*10000 + week.second, state);
    }
};

}  //KPlato namespace

#endif
