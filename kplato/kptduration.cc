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
#include "kptdatetime.h"

#include <kdebug.h>
#include <qregexp.h>

// Set the value of KPTDuration::zeroDuration to zero.
const KPTDuration KPTDuration::zeroDuration( 0, 0, 0 );

KPTDuration::KPTDuration() {
    m_ms = 0;
}

KPTDuration::KPTDuration(const KPTDuration &d) {
    m_ms = d.m_ms;
}

KPTDuration::KPTDuration(unsigned d, unsigned h, unsigned m, unsigned s, unsigned ms) {
    m_ms = ms;
    m_ms += static_cast<Q_INT64>(s) * 1000; // cast to avoid potential overflow problem
    m_ms += static_cast<Q_INT64>(m) * 60 * 1000;
    m_ms += static_cast<Q_INT64>(h) * 60 * 60 * 1000;
    m_ms += static_cast<Q_INT64>(d) * 24 * 60 * 60;
    kdDebug()<<k_funcinfo<<toString(Format_Hour)<<endl;
}

KPTDuration::KPTDuration(unsigned seconds) {
    m_ms = seconds * 1000;
}

KPTDuration::~KPTDuration() {
}

void KPTDuration::add(const KPTDuration &delta) {
    m_ms += delta.m_ms;
}

void KPTDuration::add(Q_INT64 delta) {
    Q_INT64 tmp = m_ms + delta;
    if (tmp < 0) {
        kdDebug()<<k_funcinfo<<"Underflow"<<(long int)delta<<" from "<<this->toString()<<endl;
        m_ms = 0;
        return;
    }
    m_ms = tmp;
}

void KPTDuration::subtract(const KPTDuration &delta) {
    if (m_ms < delta.m_ms) {
        kdDebug()<<k_funcinfo<<"Underflow"<<delta.toString()<<" from "<<this->toString()<<endl;
        m_ms = 0;
        return;
    }
    m_ms -= delta.m_ms;
}

KPTDuration KPTDuration::operator*(int unit) const {
    KPTDuration dur;
    if (unit < 0) {
        kdDebug()<<k_funcinfo<<"Underflow"<<unit<<" from "<<this->toString()<<endl;
    }
    else {
        dur.m_ms = m_ms * unit; //FIXME
    }
    return dur;
}

KPTDuration KPTDuration::operator/(int unit) const {
    KPTDuration dur;
    if (unit <= 0) {
        kdDebug()<<k_funcinfo<<"Underflow"<<unit<<" from "<<this->toString()<<endl;
    }
    else {
        dur.m_ms = m_ms / unit; //FIXME
    }
    return dur;
}

bool KPTDuration::isCloseTo(const KPTDuration &d) const {
    Q_INT64 limit = 30000;
    Q_INT64 delta = m_ms - d.m_ms;
    return (delta >= 0) ? (delta < limit) : (-delta < limit);
}

QString KPTDuration::toString(Format format) const {
    Q_INT64 ms;
    double days;
    unsigned hours;
    unsigned minutes;
    unsigned seconds;
    QString result;

    switch (format) {
        case Format_Hour:
            ms = m_ms;
            hours = ms / (1000 * 60 * 60);
            ms -= (Q_INT64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            result = QString("%1h%2m").arg(hours).arg(minutes);
            break;
        case Format_Day:
            days = m_ms / (1000 * 60 * 60 * 24.0);
            result = QString("%1d").arg(QString::number(days, 'f', 4));
            break;
        case Format_DayTime:
            ms = m_ms;
            days = m_ms / (1000 * 60 * 60 * 24);
            ms -= (Q_INT64)days * (1000 * 60 * 60 * 24);
            hours = ms / (1000 * 60 * 60);
            ms -= (Q_INT64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            ms -= minutes * (1000 * 60);
            seconds = ms / (1000);
            ms -= seconds * (1000);
            result.sprintf("%u %02u:%02u:%02u.%u", (unsigned)days, hours, minutes, seconds, (unsigned)ms);
            break;
        default:
            kdFatal()<<k_funcinfo<<"Unknown format"<<endl;
            break;
    }
    return result;
}

KPTDuration::KPTDuration KPTDuration::fromString(const QString &s, Format format) {
    // FIXME: Older versions of this code saved durations as QDateTime's. To avoid
    // making all test files instantly obsolete, we detect this case here. Before
    // we ship, this should be removed!
    if (s[1].isLetter())
    {
        QDateTime zero(QDate(0, 1, 1));
        int seconds = zero.secsTo(QDateTime::fromString(s));
        return KPTDuration(seconds);
    }
    QRegExp matcher;
    KPTDuration tmp;
    switch (format) {
        case Format_Hour:
            matcher.setPattern("^(\\d*)h(\\d*)m$" );
            matcher.search(s);
            tmp.addHours(matcher.cap(1).toUInt());
            tmp.addMinutes(matcher.cap(2).toUInt());
            break;
        case Format_DayTime:
            matcher.setPattern("^(\\d*) (\\d*):(\\d*):(\\d*)\\.(\\d*)$" );
            matcher.search(s);
            tmp.addDays(matcher.cap(1).toUInt());
            tmp.addHours(matcher.cap(2).toUInt());
            tmp.addMinutes(matcher.cap(3).toUInt());
            tmp.addSeconds(matcher.cap(4).toUInt());
            tmp.addMilliseconds(matcher.cap(5).toUInt());
            break;
        default:
            kdFatal()<<k_funcinfo<<"Unknown format"<<endl;
            break;
    }
    return tmp;
}

void KPTDuration::get(unsigned *days, unsigned *hours, unsigned *minutes, unsigned *seconds, unsigned *milliseconds) const {
    Q_INT64 ms;
    unsigned tmp;

    ms = m_ms;
    tmp = ms / (1000 * 60 * 60 * 24);
    *days = tmp;
    ms -= tmp * (1000 * 60 * 60 * 24);
    tmp = ms / (1000 * 60 * 60);
    *hours = tmp;
    ms -= tmp * (1000 * 60 * 60);
    tmp = ms / (1000 * 60);
    *minutes = tmp;
    ms -= tmp * (1000 * 60);
    tmp = ms / (1000);
    if (seconds)
        *seconds = tmp;
    ms -= tmp * (1000);
    if (milliseconds)
        *milliseconds = ms;
}

