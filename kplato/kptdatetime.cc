/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptdatetime.h"

KPTDateTime::KPTDateTime() : QDateTime() {
}

KPTDateTime::KPTDateTime(QDateTime dt) : QDateTime(dt.date(), dt.time()) {
}

KPTDateTime KPTDateTime::add(const KPTDuration &duration) {
    KPTDateTime d(duration.dateTime());
    int days = KPTDuration::zeroDuration.dateTime().daysTo(duration.dateTime());
    QTime t;
    int secs = t.secsTo(duration.dateTime().time());
    setDate(date().addDays(days));
    setTime(time().addSecs(secs));
    //kdDebug()<<k_funcinfo<<"days,secs: "<<days<<", "<<secs<<endl;
    return *this;
}

KPTDateTime KPTDateTime::subtract(const KPTDuration &duration) {
    int days =  KPTDuration::zeroDuration.dateTime().daysTo(duration.dateTime());
    QTime t;
    int secs = t.secsTo(duration.dateTime().time());
    setDate(date().addDays(-days));
    setTime(time().addSecs(-secs));
    //kdDebug()<<k_funcinfo<<"days,secs: "<<days<<", "<<secs<<endl;
    return *this;
}

KPTDuration KPTDateTime::duration(const KPTDateTime &dt) {
    KPTDuration dur;
    int days = daysTo(dt);
    if (days < 0) {
        dur.addDays(-days);
        dur.addSecs(dt.time().secsTo(time()));
    } else {
        dur.addDays(days);
        dur.addSecs(time().secsTo(dt.time()));
    }
    return dur;
}
