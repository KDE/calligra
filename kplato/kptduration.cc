/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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

#include "kptduration.h"

#include <kdebug.h>

// Set the value of KPTDuration::zeroDuration to zero.
const KPTDuration KPTDuration::zeroDuration( 0, 0 );

KPTDuration::KPTDuration() {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = QDateTime(QDate(0,1,1));
}

KPTDuration::KPTDuration(const KPTDuration &d) {
    zero = QDateTime(QDate(0,1,1));
    set(d.m_theTime);
}

KPTDuration::KPTDuration(int h, int m, int s, int ms) {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = QDateTime(QDate(0,1,1));
    int dur = h*3600+m*60+s;
    m_theTime = m_theTime.addSecs(dur);
    //kdDebug()<<k_funcinfo<<m_theTime.toString()<<endl;
}

KPTDuration::KPTDuration(const QTime time) {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = QDateTime(QDate(0,1,1),time);
}

KPTDuration::KPTDuration(const QDateTime time) {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = time;
}

KPTDuration::KPTDuration(int seconds) {
    zero = QDateTime(QDate(0,1,1));
    m_theTime = QDateTime(QDate(0,1,1));
    m_theTime = m_theTime.addSecs(seconds);
}


KPTDuration::~KPTDuration() {
}

void KPTDuration::add(KPTDuration time) {
    int days = zero.daysTo(time.m_theTime);
    set(m_theTime.addDays(days));
    time.m_theTime = time.m_theTime.addDays(-days);
    set(m_theTime.addSecs(zero.secsTo(time.m_theTime)));
}

void KPTDuration::add(KPTDuration *time) {
    add(*time);
}

void KPTDuration::subtract(KPTDuration time) {
    int days = time.m_theTime.daysTo(zero);
    set(m_theTime.addDays(days));
    time.set(time.m_theTime.addDays(days));
    set(m_theTime.addSecs(time.m_theTime.secsTo(zero)));
}

void KPTDuration::subtract(KPTDuration *time) {
    subtract(*time);
}

void const KPTDuration::set(KPTDuration newTime) {
    set(newTime.m_theTime);
}

void const KPTDuration::set(QDateTime newTime) {
    m_theTime.setDate(newTime.date());
    m_theTime.setTime(newTime.time());
}

//FIXME: is duration in work time??
QString KPTDuration::toString(Format format) const {
    int days = zero.daysTo(m_theTime);
    int secs = zero.time().secsTo(m_theTime.time());
    switch (format) {
        case Format_DateTime:
            return m_theTime.toString();
        case Format_Hour:
            return QString("%1h%2m").arg(days*24 + secs/3600).arg((secs%3600)/60);
            break;
        case Format_Day:
            return QString("%1.%2").arg(days).arg(secs*10/3600);
            break;
        case Format_Week:
            return QString("%1w%2.%3").arg(days/7).arg(days%7).arg(secs*10/3600);
            break;
        default: // avoid warning
            break;
    }
    return m_theTime.toString();
}
