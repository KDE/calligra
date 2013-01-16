/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptduration.h"
#include "kptdatetime.h"
#include "kptdebug.h"

#include <kglobal.h>
#include <klocale.h>

#include <QRegExp>
#include <QStringList>


namespace KPlato
{

// Set the value of Duration::zeroDuration to zero.
const Duration Duration::zeroDuration( 0, 0, 0 );

Duration::Duration() {
    m_ms = 0;
}

Duration::Duration( double value, Duration::Unit unit ) {
    if (unit == Unit_ms) m_ms = (qint64)value;
    else if (unit == Unit_s) m_ms = (qint64)(value * 1000);
    else if (unit == Unit_m) m_ms = (qint64)(value * ( 1000 * 60 ));
    else if (unit == Unit_h) m_ms = (qint64)(value * ( 1000 * 60 * 60 ));
    else if (unit == Unit_d) m_ms = (qint64)(value * ( 1000 * 60 * 60 * 24 ));
    else if (unit == Unit_w) m_ms = (qint64)(value * ( 1000 * 60 * 60 * 24 * 7 ));
    else if (unit == Unit_M) m_ms = (qint64)(value * (qint64)( 1000 * 60 * 60 ) * ( 24 * 30 ));
    else if (unit == Unit_Y) m_ms = (qint64)(value * (qint64)( 1000 * 60 * 60 ) * ( 24 * 365 ));
}

Duration::Duration(unsigned d, unsigned h, unsigned m, unsigned s, unsigned ms) {
    m_ms = ms;
    m_ms += static_cast<qint64>(s) * 1000; // cast to avoid potential overflow problem
    m_ms += static_cast<qint64>(m) * 60 * 1000;
    m_ms += static_cast<qint64>(h) * 60 * 60 * 1000;
    m_ms += static_cast<qint64>(d) * 24 * 60 * 60 * 1000;
}

Duration::Duration(const qint64 value, Duration::Unit unit) {
    if (unit == Unit_ms) m_ms = value;
    else if (unit == Unit_s) m_ms = (qint64)(value * 1000);
    else if (unit == Unit_m) m_ms = (qint64)(value * ( 1000 * 60 ));
    else if (unit == Unit_h) m_ms = (qint64)(value * ( 1000 * 60 * 60 ));
    else if (unit == Unit_d) m_ms = (qint64)(value * ( 1000 * 60 * 60 * 24 ));
    else if (unit == Unit_w) m_ms = (qint64)(value * ( 1000 * 60 * 60 * 24 * 7 ));
    else if (unit == Unit_M) m_ms = (qint64)(value * (qint64)( 1000 * 60 * 60 ) * ( 24 * 30 ));
    else if (unit == Unit_Y) m_ms = (qint64)(value * (qint64)( 1000 * 60 * 60 ) * ( 24 * 365 ));
    else kError()<<"Unknown unit: "<<unit;
}

void Duration::add(const Duration &delta) {
    m_ms += delta.m_ms;
}

void Duration::add(qint64 delta) {
    qint64 tmp = m_ms + delta;
    if (tmp < 0) {
        kDebug(planDbg())<<"Underflow"<<(long int)delta<<" from"<<this->toString();
        m_ms = 0;
        return;
    }
    m_ms = tmp;
}

void Duration::subtract(const Duration &delta) {
    if (m_ms < delta.m_ms) {
        kDebug(planDbg())<<"Underflow"<<delta.toString()<<" from"<<this->toString();
        m_ms = 0;
        return;
    }
    m_ms -= delta.m_ms;
}

Duration Duration::operator*(int value) const {
    Duration dur(*this);
    if (value < 0) {
        kDebug(planDbg())<<"Underflow"<<value<<" from"<<this->toString();
    }
    else {
        dur.m_ms = m_ms * value; //FIXME
    }
    return dur;
}

Duration Duration::operator/(int value) const {
    Duration dur(*this);
    if (value <= 0) {
        kDebug(planDbg())<<"Underflow"<<value<<" from"<<this->toString();
    }
    else {
        dur.m_ms = m_ms / value; //FIXME
    }
    return dur;
}

Duration Duration::operator*(const double value) const {
    Duration dur(*this);
    dur.m_ms = qAbs(m_ms * (qint64)value);
    return dur;
}

Duration Duration::operator*(const Duration value) const {
    Duration dur(*this);
    dur.m_ms = m_ms * value.m_ms;
    return dur;
}

double Duration::operator/(const Duration &d) const {
    if (d == zeroDuration) {
        kDebug(planDbg())<<"Divide by zero:"<<this->toString();
        return 0.0;
    }
    return (double)(m_ms) / (double)(d.m_ms);
}

QString Duration::format(Unit unit, int pres, const KLocale *locale) const
{
    if ( locale == 0 ) {
        locale = KGlobal::locale();
    }
    /* FIXME if necessary
    return i18nc( "<duration><unit>", "%1%2", locale->formatNumber(toDouble(unit), pres), unitToString(unit) );*/
    return locale->formatNumber( toDouble( unit ), pres ) + unitToString( unit );
}

QString Duration::toString(Format format) const {
    qint64 ms;
    double days;
    unsigned hours;
    unsigned minutes;
    unsigned seconds;
    QString result;

    switch (format) {
        case Format_Hour:
            ms = m_ms;
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
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
            ms -= (qint64)days * (1000 * 60 * 60 * 24);
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            ms -= minutes * (1000 * 60);
            seconds = ms / (1000);
            ms -= seconds * (1000);
            result.sprintf("%u %02u:%02u:%02u.%u", (unsigned)days, hours, minutes, seconds, (unsigned)ms);
            break;
        case Format_HourFraction:
            result = KGlobal::locale()->formatNumber(toDouble(Unit_h), 2);
            break;
        // i18n
        case Format_i18nHour:
            ms = m_ms;
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            result = i18nc("<hours>h:<minutes>m", "%1h:%2m", hours, minutes);
            break;
        case Format_i18nDay:
            result = KGlobal::locale()->prettyFormatDuration( m_ms );
            break;
        case Format_i18nWeek:
            result = this->format( Unit_w, 2, KGlobal::locale() );
            break;
        case Format_i18nMonth:
            result = this->format( Unit_M, 2, KGlobal::locale() );
            break;
        case Format_i18nYear:
            result = this->format( Unit_Y, 2, KGlobal::locale() );
            break;
        case Format_i18nDayTime:
            ms = m_ms;
            days = m_ms / (1000 * 60 * 60 * 24);
            ms -= (qint64)days * (1000 * 60 * 60 * 24);
            hours = ms / (1000 * 60 * 60);
            ms -= (qint64)hours * (1000 * 60 * 60);
            minutes = ms / (1000 * 60);
            ms -= minutes * (1000 * 60);
            seconds = ms / (1000);
            ms -= seconds * (1000);
            if (days == 0) {
                result = toString(Format_i18nHour);
            } else {
                result = i18nc("<days>d <hours>h:<minutes>m", "%1d %2h:%3m", days, hours, minutes);
            }
            break;
        case Format_i18nHourFraction:
            result = KGlobal::locale()->formatNumber(toDouble(Unit_h), 2);
            break;
        default:
            kFatal()<<"Unknown format"<<endl;
            break;
    }
    return result;
}

Duration Duration::fromString(const QString &s, Format format, bool *ok) {
    if (ok) *ok = false;
    QRegExp matcher;
    Duration tmp;
    switch (format) {
        case Format_Hour: {
            matcher.setPattern("^(\\d*)h(\\d*)m$" );
            int pos = matcher.indexIn(s);
            if (pos > -1) {
                tmp.addHours(matcher.cap(1).toUInt());
                tmp.addMinutes(matcher.cap(2).toUInt());
                if (ok) *ok = true;
            }
            break;
        }
        case Format_DayTime: {
            matcher.setPattern("^(\\d*) (\\d*):(\\d*):(\\d*)\\.(\\d*)$" );
            int pos = matcher.indexIn(s);
            if (pos > -1) {
                tmp.addDays(matcher.cap(1).toUInt());
                tmp.addHours(matcher.cap(2).toUInt());
                tmp.addMinutes(matcher.cap(3).toUInt());
                tmp.addSeconds(matcher.cap(4).toUInt());
                tmp.addMilliseconds(matcher.cap(5).toUInt());
                if (ok) *ok = true;
            }
            break;
        }
        case Format_HourFraction: {
            // should be in double format
            bool res;
            double f = KGlobal::locale()->readNumber(s, &res);
            if (ok) *ok = res;
            if (res) {
                return Duration((qint64)(f)*3600*1000);
            }
            break;
        }
        default:
            kFatal()<<"Unknown format"<<endl;
            break;
    }
    return tmp;
}

QStringList Duration::unitList( bool trans )
{
    QStringList lst;
    lst << ( trans ? i18nc( "Year. Note: Letter(s) only!", "Y" ) : "Y" )
        << ( trans ? i18nc( "Month. Note: Letter(s) only!", "M" ) : "M" )
        << ( trans ? i18nc( "Week. Note: Letter(s) only!", "w" ) : "w" )
        << ( trans ? i18nc( "Day. Note: Letter(s) only!", "d" ) : "d" )
        << ( trans ? i18nc( "Hour. Note: Letter(s) only!", "h" ) : "h" )
        << ( trans ? i18nc( "Minute. Note: Letter(s) only!", "m" ) : "m" )
        << ( trans ? i18nc( "Second. Note: Letter(s) only!", "s" ) : "s" )
        << ( trans ? i18nc( "Millisecond. Note: Letter(s) only!", "ms" ) : "ms" );
    return lst;
}

QString Duration::unitToString( Duration::Unit unit, bool trans )
{
    return unitList( trans ).at( unit );
}

Duration::Unit Duration::unitFromString( const QString &u )
{
    int i = unitList().indexOf( u );
    if ( i < 0 ) {
        kError()<<"Illegal unit: "<<u;
        return Unit_ms;
    }
    return (Duration::Unit)( i ); 
}

bool Duration::valueFromString( const QString &value, double &rv, Unit &unit ) {
    QStringList lst = Duration::unitList();
    foreach ( const QString &s, lst ) {
        int pos = value.lastIndexOf( s );
        if ( pos != -1 ) {
            unit = Duration::unitFromString( s );
            QString v = value;
            v.remove( s );
            bool ok;
            rv = v.toDouble( &ok );
            kError()<<value<<" -> "<<v<<", "<<s<<" = "<<ok<<endl;
            return ok;
        }
    }
    kError()<<"Illegal format, no unit: "<<value<<endl;
    return false;
}

double Duration::toHours() const
{
    return toDouble( Unit_h );
}

double Duration::toDouble( Unit u ) const
{
    if (u == Unit_ms) return (double)m_ms;
    else if (u == Unit_s) return (double)m_ms/1000.0;
    else if (u == Unit_m) return (double)m_ms/(1000.0*60.0);
    else if (u == Unit_h) return (double)m_ms/(1000.0*60.0*60.0);
    else if (u == Unit_d) return (double)m_ms/(1000.0*60.0*60.0*24.0);
    else if (u == Unit_w) return (double)m_ms/(1000.0*60.0*60.0*24.0*7.0);
    else if (u == Unit_M) return (double)m_ms/(1000.0*60.0*60.0*24.0*30); //Month
    else if (u == Unit_Y) return (double)m_ms/(1000.0*60.0*60.0*24.0*365); // Year
    return (double)m_ms;
}

}  //KPlato namespace
