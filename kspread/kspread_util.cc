/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "kspread_util.h"
#include "kspread_map.h"
#include "kspread_doc.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <ctype.h>

#include <qregexp.h>

#include <kdebug.h>

namespace kspread_util_LNS
{
  bool    g_Init = false;
  QString g_Monday;
  QString g_Tuesday;
  QString g_Wednesday;
  QString g_Thursday;
  QString g_Friday;
  QString g_Saturday;
  QString g_Sunday;
  QString g_Mon;
  QString g_Tue;
  QString g_Wed;
  QString g_Thu;
  QString g_Fri;
  QString g_Sat;
  QString g_Sun;
  QString g_January;
  QString g_February;
  QString g_March;
  QString g_April;
  QString g_MayL;
  QString g_June;
  QString g_July;
  QString g_August;
  QString g_September;
  QString g_October;
  QString g_November;
  QString g_December;
  QString g_Jan;
  QString g_Feb;
  QString g_Mar;
  QString g_Apr;
  QString g_May;
  QString g_Jun;
  QString g_Jul;
  QString g_Aug;
  QString g_Sep;
  QString g_Oct;
  QString g_Nov;
  QString g_Dec;
}

using namespace kspread_util_LNS;

void init( KLocale * locale )
{
  g_Init = true;
  g_Monday = locale->weekDayName( 1, false );
  g_Tuesday = locale->weekDayName( 2, false );
  g_Wednesday = locale->weekDayName( 3, false );
  g_Thursday = locale->weekDayName( 4, false );
  g_Friday = locale->weekDayName( 5, false );
  g_Saturday = locale->weekDayName( 6, false );
  g_Sunday = locale->weekDayName( 7, false );
  g_Mon = locale->weekDayName( 1, true );
  g_Tue = locale->weekDayName( 2, true );
  g_Wed = locale->weekDayName( 3, true );
  g_Thu = locale->weekDayName( 4, true );
  g_Fri = locale->weekDayName( 5, true );
  g_Sat = locale->weekDayName( 6, true );
  g_Sun = locale->weekDayName( 7, true );
  g_January  = locale->monthName( 1, false );
  g_February = locale->monthName( 2, false );
  g_March  = locale->monthName( 3, false );
  g_April  = locale->monthName( 4, false );
  g_MayL   = locale->monthName( 5, false );
  g_June   = locale->monthName( 6, false );
  g_July   = locale->monthName( 7, false );
  g_August = locale->monthName( 8, false );
  g_September = locale->monthName( 9, false );
  g_October   = locale->monthName( 10, false );
  g_November  = locale->monthName( 11, false );
  g_December  = locale->monthName( 12, false );
  g_Jan = locale->monthName( 1, true );
  g_Feb = locale->monthName( 2, true );
  g_Mar = locale->monthName( 3, true );
  g_Apr = locale->monthName( 4, true );
  g_May = locale->monthName( 5, true );
  g_Jun = locale->monthName( 6, true );
  g_Jul = locale->monthName( 7, true );
  g_Aug = locale->monthName( 8, true );
  g_Sep = locale->monthName( 9, true );
  g_Oct = locale->monthName( 10, true );
  g_Nov = locale->monthName( 11, true );
  g_Dec = locale->monthName( 12, true );
}

QString
util_fractionFormat(double value, KSpreadCell::FormatType fmtType)
{
    double result = value - floor(value);
    int index;
    int limit = 0;

    /* return w/o fraction part if not necessary */
    if (result == 0)
	return QString::number(value);

    switch (fmtType) {
    case KSpreadCell::fraction_half:
	index = 2;
	break;
    case KSpreadCell::fraction_quarter:
	index = 4;
	break;
    case KSpreadCell::fraction_eighth:
	index = 8;
	break;
    case KSpreadCell::fraction_sixteenth:
	index = 16;
	break;
    case KSpreadCell::fraction_tenth:
	index = 10;
	break;
    case KSpreadCell::fraction_hundredth:
	index = 100;
	break;
    case KSpreadCell::fraction_one_digit:
	index = 3;
	limit = 9;
	break;
    case KSpreadCell::fraction_two_digits:
	index = 4;
	limit = 99;
	break;
    case KSpreadCell::fraction_three_digits:
	index = 5;
	limit = 999;
	break;
    default:
	kdDebug(36001) << "Error in Fraction format\n";
	return QString::number(value);
	break;
    } /* switch */


    /* handle halves, quarters, tenths, ... */

    if (fmtType != KSpreadCell::fraction_three_digits
	&& fmtType != KSpreadCell::fraction_two_digits
	&& fmtType != KSpreadCell::fraction_one_digit) {
	double calc = 0;
	int index1 = 0;
	double diff = result;
	for (int i = 1; i <= index; i++) {
	    calc = i * 1.0 / index;
	    if (fabs(result - calc) < diff) {
		index1 = i;
		diff = fabs(result - calc);
	    }
	}
	if( index1 == 0 ) return QString("%1").arg( floor(value) );
	if( index1 == index ) return QString("%1").arg( floor(value)+1 );
        if( floor(value) == 0)
            return QString("%1/%2").arg( index1 ).arg( index );

	return QString("%1 %2/%3")
		.arg( floor(value) )
	      	.arg( index1 )
		.arg( index );
    }


    /* handle fraction_one_digit, fraction_two_digit
     * and fraction_three_digit style */

    double precision, denominator, numerator;

    do {
	double val1 = result;
	double val2 = rint(result);
	double inter2 = 1;
	double inter4, p,  q;
	inter4 = p = q = 0;

	precision = pow(10.0, -index);
	numerator = val2;
	denominator = 1;

	while (fabs(numerator/denominator - result) > precision) {
		val1 = (1 / (val1 - val2));
		val2 = rint(val1);
		p = val2 * numerator + inter2;
		q = val2 * denominator + inter4;
		inter2 = numerator;
		inter4 = denominator;
		numerator = p;
		denominator = q;
	}
	index--;
    } while (fabs(denominator) > limit);

    denominator = fabs(denominator);
    numerator = fabs(numerator);

    if (denominator == numerator)
	return QString().setNum(floor(value + 1));
    else
    {
        if ( floor(value) == 0 )
            return QString("%1/%2").arg(numerator).arg(denominator);
        else
            return QString("%1 %2/%3")
		.arg(floor(value))
		.arg(numerator)
		.arg(denominator);
    }
}

QString util_customNumberFormat( KLocale * locale, double value,
                                 KSpreadCell::FormatType fmtType,
                                 QString const & formatString )
{
  return "";
}

QString
util_customTimeFormat( KLocale * locale, QTime const & time,
                       KSpreadCell::FormatType fmtType,
                       QString const & formatString )
{
  int h = time.hour();
  int m = time.minute();
  int s = time.second();

  bool pm = (h > 12);
  QString AMPM( pm ? i18n( "PM" ) : i18n( "AM" ) );

  bool setPM = ( ( formatString.find( "AM", 0, false ) != -1 )
                 || ( formatString.find( "PM", 0, false ) != -1 ) );

  if ( setPM && h > 12 )
    h -= 12;

  QString hour = ( h < 10 ? "0" + QString::number(h) : QString::number(h) );
  QString minute = ( m < 10 ? "0" + QString::number(m) : QString::number(m) );
  QString second = ( s < 10 ? "0" + QString::number(s) : QString::number(s) );

  QString result;
  QChar next;

  int i = 0;
  int l = formatString.length();

  bool hourSeen = false;
  bool minSeen = false;
  bool secSeen = false;

  while ( i < l )
  {
    if ( i + 1 < l )
      next = formatString[i + 1];
    else
      next = '\0';
    switch( formatString[i] )
    {
     case 'S':
     case 's':
      if ( secSeen )
      {
        result += formatString[i];
        break;
      }
      secSeen = true;
      if ( next == 'D' || next == 'd' )
      {
        result += second;
        ++i;
      }
      else
        result += QString::number( s );
      break;

     case 'M':
     case 'm':
      if ( minSeen )
      {
        result += formatString[i];
        break;
      }
      minSeen = true;
      if ( next == 'm' || next == 'M' )
      {
        result += minute;
        ++i;
      }
      else
        result += QString::number( m );
      break;

     case 'H':
     case 'h':
      if ( hourSeen )
      {
        result += formatString[i];
        break;
      }
      hourSeen = true;
      if ( next == 'h' || next == 'H' )
      {
        result += hour;
        ++i;
      }
      else
        result += QString::number( h );
      break;

     case 'A':
     case 'a':
     case 'P':
     case 'p':
      if ( setPM && ( next == 'm' || next == 'M' ) )
      {
        if ( formatString.find( "AM/PM", i, false ) == i )
        {
          result += AMPM;
          i += 5;
        }
        else
        {
          i += 2;
          result += AMPM;
        }
        setPM = false;
      }
      else
        result += formatString[i];
      break;

     default:
      result += formatString[i];
    }

    ++i;
  }

  return result;
}

QString
util_timeFormat(KLocale * locale, const QTime & time,
		KSpreadCell::FormatType fmtType)
{
    if (fmtType == KSpreadCell::Time)
	return locale->formatTime(time, false);

    if (fmtType == KSpreadCell::SecondeTime)
	return locale->formatTime(time, true);

    int h = time.hour();
    int m = time.minute();
    int s = time.second();

    QString hour = ( h < 10 ? "0" + QString::number(h) : QString::number(h) );
    QString minute = ( m < 10 ? "0" + QString::number(m) : QString::number(m) );
    QString second = ( s < 10 ? "0" + QString::number(s) : QString::number(s) );
    bool pm = (h > 12);
    QString AMPM( pm ? i18n("PM"):i18n("AM") );

    if (fmtType == KSpreadCell::Time_format1) {	// 9 : 01 AM
	return QString("%1:%2 %3")
		.arg((pm ? h - 12 : h),2)
		.arg(minute,2)
		.arg(AMPM);
    }

    if (fmtType == KSpreadCell::Time_format2) {	//9:01:05 AM
	return QString("%1:%2:%3 %4")
		.arg((pm ? h-12 : h),2)
		.arg(minute,2)
		.arg(second,2)
		.arg(AMPM);
    }

    if (fmtType == KSpreadCell::Time_format3) {
	return QString("%1 %2 %3 %4 %5 %6")			// 9 h 01 min 28 s
		.arg(hour,2)
		.arg(i18n("h"))
		.arg(minute,2)
		.arg(i18n("min"))
		.arg(second,2)
		.arg(i18n("s"));
    }

    if (fmtType == KSpreadCell::Time_format4) {	// 9:01
	return QString("%1:%2").arg(hour, 2).arg(minute, 2);
    }

    if (fmtType == KSpreadCell::Time_format5) {	// 9:01:12
	return QString("%1:%2:%3").arg(hour, 2).arg(minute, 2).arg(second, 2);
    }

    // "m/d/yy h:mm",

    if (fmtType == KSpreadCell::Time_format6) {	// 01:12
	return QString("%1:%2").arg(minute, 2).arg(second, 2);
    }

    return locale->formatTime( time, false );
}

/*
QString util_durationFormat(KLocale * locale, const KSpreadDuration & m_Duration,
                            KSpreadCell::FormatType fmtType)
{
  int hour   = m_Duration.hour();
  int minute = m_Duration.minute();
  int second = m_Duration.second();

  if (fmtType == KSpreadCell::Duration_format1)  // 28:14:14
  {
    return QString("%1:%2:%3").arg(hour).arg(minute, 2).arg(second, 2);
  }
  else if (fmtType == KSpreadCell::Duration_format2)  // 28:14
  {
    return QString("%1:%2").arg(hour).arg(minute, 2);
  }
  else if (fmtType == KSpreadCell::Duration_format3)  // 128:14
  {
    minute = minute + hour * 60;
    return QString("%1:%2").arg(minute).arg(second, 2);
  }
  else if (fmtType == KSpreadCell::Duration_format4)  // 128123
  {
    minute = minute + hour * 60;
    second = second + minute * 60;
    return QString("%1").arg(second, 2);
  }

  return locale->formatTime(QTime( (m_Duration.hour() % 24), m_Duration.minute(), m_Duration.second()), false);
}
*/

void appendDay( QString & tmp, QString const & formatString, int & pos, int l,
                QDate const & date, bool & daySeen, bool & weekDaySeen )
{
  int num = 1;
  if ( ( formatString[pos + 1] != 'd' )
       && ( formatString[pos + 1] != 'D' ) )
    num = 2;
  else
  if ( ( formatString[pos + 2] != 'd' )
       && ( formatString[pos + 2] != 'D' ) )
    num = 3;
  else
  if ( ( formatString[pos + 3] != 'd' )
       && ( formatString[pos + 3] != 'D' ) )
    num = 4;

  if ( num > 2 )
  {
    weekDaySeen = true;
    int weekDay = date.dayOfWeek();
    switch ( weekDay )
    {
     case 1:
      tmp += ( num == 4 ? g_Monday : g_Mon );
      break;

     case 2:
      tmp += ( num == 4 ? g_Tuesday : g_Tue );
      break;

     case 3:
      tmp += ( num == 4 ? g_Wednesday : g_Wed );
      break;

     case 4:
      tmp += ( num == 4 ? g_Thursday : g_Thu );
      break;

     case 5:
      tmp += ( num == 4 ? g_Friday : g_Fri );
      break;

     case 6:
      tmp += ( num == 4 ? g_Saturday : g_Sat );
      break;

     case 7:
      tmp += ( num == 4 ? g_Sunday : g_Sun );
      break;
    }
  }
  else
  {
    if ( daySeen )
    {
      tmp += formatString[pos];
      if ( num == 2 )
        tmp += formatString[pos + 1];
    }
    else
    {
      int d = date.day();
      daySeen = true;
      if ( num == 2 )
        tmp += ( d < 10 ? "0" + QString::number( d ) : QString::number( d ) );
      else
        tmp += QString::number( d );
    }
  }

  pos += num - 1; // -1, because we increase it again at the end of the loop in util_dateFormat
}

void appendMonth( QString & tmp, QString const & formatString, int & pos, int l,
                  QDate const & date, bool & monthSeen )
{
  int num = 1;
  if ( ( formatString[pos + 1] != 'm' )
       && ( formatString[pos + 1] != 'M' ) )
    num = 2;
  else
  if ( ( formatString[pos + 2] != 'm' )
       && ( formatString[pos + 2] != 'M' ) )
    num = 3;
  else
  if ( ( formatString[pos + 3] != 'm' )
       && ( formatString[pos + 3] != 'M' ) )
    num = 4;

  if ( num > 2 )
  {
    int month = date.month();
    switch ( month )
    {
     case 1:
      tmp += ( num == 4 ? g_January : g_Jan );
      break;

     case 2:
      tmp += ( num == 4 ? g_February : g_Feb );
      break;

     case 3:
      tmp += ( num == 4 ? g_March : g_Mar );
      break;

     case 4:
      tmp += ( num == 4 ? g_April : g_Apr );
      break;

     case 5:
      tmp += ( num == 4 ? g_MayL : g_May );
      break;

     case 6:
      tmp += ( num == 4 ? g_June : g_Jun );
      break;

     case 7:
      tmp += ( num == 4 ? g_July : g_Jul );
      break;

     case 8:
      tmp += ( num == 4 ? g_August : g_Aug );
      break;

     case 9:
      tmp += ( num == 4 ? g_September : g_Sep );
      break;

     case 10:
      tmp += ( num == 4 ? g_October : g_Oct );
      break;

     case 11:
      tmp += ( num == 4 ? g_November : g_Nov );
      break;

     case 12:
      tmp += ( num == 4 ? g_December : g_Dec );
      break;
    }
  }
  else
  {
    if ( monthSeen )
    {
      tmp += formatString[ pos ];
      if ( num == 2 )
        tmp += formatString[ pos + 1 ];
    }
    else
    {
      int m = date.month();
      monthSeen = true;
      if ( num == 2 )
        tmp += ( m < 10 ? "0" + QString::number( m ) : QString::number( m ) );
      else
        tmp += QString::number( m );
    }
  }

  pos += num - 1; // -1, because we increase it again at the end of the loop in util_dateFormat
}

void appendYear( QString & tmp, QString const & formatString,
                 int & pos, int /*l*/, QDate const & date, bool & yearSeen )
{
  if ( yearSeen )
  {
    tmp += formatString[ pos ];
    return;
  }

  int num = 1;
  if ( ( formatString[pos + 1] != 'y' )
       && ( formatString[pos + 1] != 'Y' ) )
    num = 2;
  else
  if ( ( formatString[pos + 2] != 'y' )
       && ( formatString[pos + 2] != 'Y' ) )
    num = 3;
  else
  if ( ( formatString[pos + 3] != 'y' )
       && ( formatString[pos + 3] != 'Y' ) )
    num = 4;

  yearSeen = true;

  int y = date.year();
  if ( num <= 2 )
    tmp += QString::number( y ).right( 2 );
  else
    tmp += QString::number( y );

  pos += num - 1;
}

QString util_customDateFormat( KLocale * locale, QDate const & date,
                               KSpreadCell::FormatType fmtType,
                               QString const & formatString )
{
  if ( !g_Init )
    init( locale );

  QString tmp;


  bool yearSeen    = false;
  bool monthSeen   = false;
  bool daySeen     = false;
  bool weekDaySeen = false;

  int i = 0;
  int l = formatString.length();

  while ( i < l )
  {
    switch( formatString[i] )
    {
     case 'D':
     case 'd':
      appendDay( tmp, formatString, i, l, date, daySeen, weekDaySeen );
      break;

     case 'M':
     case 'm':
      appendMonth( tmp, formatString, i, l, date, monthSeen );
      break;

     case 'Y':
     case 'y':
      appendYear( tmp, formatString, i, l, date, yearSeen );
      break;

     default:
      tmp += formatString[i];
    }

    ++i;
  }

  return tmp;
}

QString
util_dateFormat(KLocale * locale, const QDate &date,
		KSpreadCell::FormatType fmtType)
{
    QString tmp;
    if (fmtType == KSpreadCell::ShortDate) {
	tmp = locale->formatDate(date, true);
    }
    else if (fmtType == KSpreadCell::TextDate) {
	tmp = locale->formatDate(date, false);
    }
    else if (fmtType == KSpreadCell::date_format1) {	/*18-Feb-99 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + locale->monthName(date.month(), true) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format2) {	/*18-Feb-1999 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + locale->monthName(date.month(), true) + "-";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format3) {	/*18-Feb */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + locale->monthName(date.month(), true);
    }
    else if (fmtType == KSpreadCell::date_format4) {	/*18-05 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "-" + QString().sprintf("%02d", date.month());
    }
    else if (fmtType == KSpreadCell::date_format5) {	/*18/05/00 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString().sprintf("%02d", date.month()) + "/";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format6) {	/*18/05/1999 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString().sprintf("%02d", date.month()) + "/";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format7) {	/*Feb-99 */
	tmp = locale->monthName(date.month(), true) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format8) {	/*February-99 */
	tmp = locale->monthName(date.month()) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format9) {	/*February-1999 */
	tmp = locale->monthName(date.month()) + "-";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format10) {	/*F-99 */
	tmp = locale->monthName(date.month()).at(0) + "-";
	tmp = tmp + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format11) {	/*18/Feb */
	tmp = QString().sprintf("%02d", date.day()) + "/";
	tmp += locale->monthName(date.month(), true);
    }
    else if (fmtType == KSpreadCell::date_format12) {	/*18/02 */
	tmp = QString().sprintf("%02d", date.day()) + "/";
	tmp += QString().sprintf("%02d", date.month());
    }
    else if (fmtType == KSpreadCell::date_format13) {	/*18/Feb/1999 */
	tmp = QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + locale->monthName(date.month(), true) + "/";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format14) {	/*2000/Feb/18 */
	tmp = QString::number(date.year());
	tmp = tmp + "/" + locale->monthName(date.month(), true) + "/";
	tmp = tmp + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format15) {	/*2000-Feb-18 */
	tmp = QString::number(date.year());
	tmp = tmp + "-" + locale->monthName(date.month(), true) + "-";
	tmp = tmp + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format16) {	/*2000-02-18 */
	tmp = QString::number(date.year());
	tmp = tmp + "-" + QString().sprintf("%02d", date.month()) + "-";
	tmp = tmp + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format17) {	/*2 february 2000 */
	tmp = QString().sprintf("%d", date.day());
	tmp = tmp + " " + locale->monthName(date.month()) + " ";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format18) {	/*02/18/1999 */
	tmp = QString().sprintf("%02d", date.month());
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format19) {	/*02/18/99 */
	tmp = QString().sprintf("%02d", date.month());
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format20) {	/*Feb/18/99 */
	tmp = locale->monthName(date.month(), true);
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format21) {	/*Feb/18/1999 */
	tmp = locale->monthName(date.month(), true);
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
	tmp = tmp + "/" + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format22) {	/*Feb-1999 */
	tmp = locale->monthName(date.month(), true) + "-";
	tmp = tmp + QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format23) {	/*1999 */
	tmp = QString::number(date.year());
    }
    else if (fmtType == KSpreadCell::date_format24) {	/*99 */
	tmp = QString::number(date.year()).right(2);
    }
    else if (fmtType == KSpreadCell::date_format25) {	/*2000/02/18 */
	tmp = QString::number(date.year());
	tmp = tmp + "/" + QString().sprintf("%02d", date.month());
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
    }
    else if (fmtType == KSpreadCell::date_format26) {	/*2000/Feb/18 */
	tmp = QString::number(date.year());
	tmp = tmp + "/" + locale->monthName(date.month(), true);
	tmp = tmp + "/" + QString().sprintf("%02d", date.day());
    }
    else
	tmp = locale->formatDate(date, true);

    // Missing compared with gnumeric:
    //	"m/d/yy h:mm",		/* 20 */
    //	"m/d/yyyy h:mm",	/* 21 */
    //	"mmm/ddd/yy",		/* 12 */
    //	"mmm/ddd/yyyy",		/* 13 */
    //	"mm/ddd/yy",		/* 14 */
    //	"mm/ddd/yyyy",		/* 15 */

    return tmp;
}


QString util_dateTimeFormat( KLocale * /*locale*/, double /*date*/, KSpreadCell::FormatType /*fmtType*/, QString const & /*format*/ )
{
    return QString::null;
}

int util_decodeColumnLabelText( const QString &_col )
{
    int col = 0;
    int offset='a'-'A';
    int counterColumn = 0;
    for ( uint i=0; i < _col.length(); i++ )
    {
        counterColumn = (int) pow(26.0 , static_cast<int>(_col.length() - i - 1));
        if( _col[i] >= 'A' && _col[i] <= 'Z' )
            col += counterColumn * ( _col[i].latin1() - 'A' + 1);  // okay here (Werner)
        else if( _col[i] >= 'a' && _col[i] <= 'z' )
            col += counterColumn * ( _col[i].latin1() - 'A' - offset + 1 );
        else
            kdDebug(36001) << "util_decodeColumnLabelText: Wrong characters in label text for col:'" << _col << "'" << endl;
    }
    return col;
}

QString util_encodeColumnLabelText( int column )
{
    int tmp;

    /* we start with zero */
    tmp = column - 1;

    if (tmp < 26) /* A-Z */
	return QString("%1").arg((char) ('A' + tmp));

    tmp -= 26;
    if (tmp < 26*26) /* AA-ZZ */
	return QString("%1%2").arg( (char) ('A' + tmp / 26) )
			      .arg( (char) ('A' + tmp % 26) );

    tmp -= 26*26;
    if (tmp < 26 * 26 * 26 ) /* AAA-ZZZ */
	return QString("%1%2%3").arg( (char) ('A' + tmp / (26 * 26)) )
				.arg( (char) ('A' + (tmp / 26) % 26 ) )
				.arg( (char) ('A' + tmp % 26) );

    tmp -= 26*26*26;
    if (tmp < 26 * 26 * 26 * 26) /* AAAA-ZZZZ */
	return QString("%1%2%3%4").arg( (char) ('A' + (tmp / (26 * 26 * 26 )      ) ))
				  .arg( (char) ('A' + (tmp / (26 * 26      ) % 26 ) ))
				  .arg( (char) ('A' + (tmp / (26           ) % 26 ) ))
				  .arg( (char) ('A' + (tmp                   % 26 ) ));

    /* limit is currently 26^4 + 26^3 + 26^2 + 26^1 = 475254 */
    kdDebug(36001) << "invalid column\n";
    return QString("@@@");
}

QString util_rangeColumnName( const QRect &_area)
{
    return QString("%1:%2")
        .arg(util_encodeColumnLabelText( _area.left()))
        .arg(util_encodeColumnLabelText(_area.right()));
}

QString util_rangeRowName( const QRect &_area)
{
    return QString("%1:%2")
        .arg( _area.top())
        .arg(_area.bottom());
}


QString util_rangeName(const QRect &_area)
{
    return KSpreadCell::name( _area.left(), _area.top() ) + ":" +
	KSpreadCell::name( _area.right(), _area.bottom() );
}

QString util_rangeName(KSpreadSheet * _table, const QRect &_area)
{
    return _table->tableName() + "!" + util_rangeName(_area);
}

KSpreadPoint::KSpreadPoint(const QString & _str)
{
    table = 0;
    init(_str);
}

void
 KSpreadPoint::init(const QString & _str)
{
    pos.setX(-1);

    uint len = _str.length();
    if ( !len )
    {
	kdDebug(36001) << "KSpreadPoint::init: len = 0" << endl;
	return;
    }

    QString str( _str );
    int n = _str.find( '!' );
    if ( n != 1 )
    {
      tableName = _str.left( n );
      str = _str.right( len - n - 1 ); // remove the '!'
      len = str.length();
    }

    uint p = 0;

    // Fixed ?
    if ( str[0] == '$' )
    {
	columnFixed = true;
	p++;
    }
    else
	columnFixed = false;

    // Malformed ?
    if ( p == len )
    {
	kdDebug(36001) << "KSpreadPoint::init: no point after '$' (str: '" << str.mid( p ) << "'" << endl;
	return;
    }
    if ( str[p] < 'A' || str[p] > 'Z' )
    {
	if ( str[p] < 'a' || str[p] > 'z' )
	{
	    kdDebug(36001) << "KSpreadPoint::init: wrong first character in point (str: '" << str.mid( p ) << "'" << endl;
	    return;
	}
    }
    //default is error
    int x = -1;
    //search for the first character != text
    int result = str.find( QRegExp("[^A-Za-z]+"), p );

    //get the colomn number for the character between actual position and the first non text charakter
    if ( result != -1 )
	x = util_decodeColumnLabelText( str.mid( p, result - p ) ); // x is defined now
    else  // If there isn't any, then this is not a point -> return
    {
	kdDebug(36001) << "KSpreadPoint::init: no number in string (str: '" << str.mid( p, result ) << "'" << endl;
	return;
    }
    p = result;

    //limit is KS_colMax
    if ( x > KS_colMax )
    {
	kdDebug(36001) << "KSpreadPoint::init: column value too high (col: " << x << ")" << endl;
	return;
    }

    // Malformed ?
    if (p == len)
    {
	kdDebug(36001) << "KSpreadPoint::init: p==len after cols" << endl;
	return;
    }

    if (str[p] == '$')
    {
	rowFixed = true;
	p++;
	// Malformed ?
	if ( p == len )
	{
	    kdDebug(36001) << "KSpreadPoint::init: p==len after $ of row" << endl;
	    return;
	}
    }
    else
	rowFixed = false;

    uint p2 = p;
    while ( p < len )
    {
	if ( !isdigit( QChar(str[p++]) ) )
	{
	    kdDebug(36001) << "KSpreadPoint::init: no number" << endl;
	    return;
	}
    }

    //int y = atoi( str.mid( p2, p-p2 ).latin1() );
    bool ok;
    int y = str.mid( p2, p-p2 ).toInt( &ok );
    if ( !ok )
    {
	kdDebug(36001) << "KSpreadPoint::init: Invalid number (str: '" << str.mid( p2, p-p2 ) << "'" << endl;
	return;
    }
    if ( y > KS_rowMax )
    {
	kdDebug(36001) << "KSpreadPoint::init: row value too high (row: " << y << ")" << endl;
	return;
    }
    if ( y <= 0 )
    {
	kdDebug(36001) << "KSpreadPoint::init: y <= 0" << endl;
	return;
    }
    pos = QPoint( x, y );
}

KSpreadPoint::KSpreadPoint(const QString & _str, KSpreadMap * _map,
			   KSpreadSheet * _table)
{
    uint p = 0;
    int p2 = _str.find('!');
    if (p2 != -1) {
	tableName = _str.left(p2++);
        while ( true )
        {
          table = _map->findTable(tableName);
          if ( !table && tableName[0] == ' ' )
          {
            tableName = tableName.right( tableName.length() - 1 );
            continue;
          }
          break;
        }
	p = p2;
    } else
	table = _table;

    init(_str.mid(p));
}

KSpreadCell *KSpreadPoint::cell()
{
    return table->cellAt(pos);
}

KSpreadRange::KSpreadRange(const QString & _str)
{
    range.setLeft(-1);
    table = 0;

    int p = _str.find(':');
    if (p == -1)
	return;

    KSpreadPoint ul(_str.left(p));
    KSpreadPoint lr(_str.mid(p + 1));
    range = QRect(ul.pos, lr.pos);
    tableName = ul.tableName;

    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
}

KSpreadRange::KSpreadRange(const QString & _str, KSpreadMap * _map,
			   KSpreadSheet * _table)
{
    range.setLeft(-1);
    table = 0;
    //used area Name as range
    if (_str.at(0) == "'" && _str.at(_str.length() - 1) == "'") {
	QString tmp = _str.right(_str.length() - 1);
	tmp = tmp.left(tmp.length() - 1);
	QValueList < Reference >::Iterator it;
	QValueList < Reference > area = _map->doc()->listArea();
	for (it = area.begin(); it != area.end(); ++it) {
	    if ((*it).ref_name == tmp) {
		range = (*it).rect;
		table = _map->findTable((*it).table_name);
		break;
	    }
	}
	leftFixed = false;
	rightFixed = false;
	topFixed = false;
	bottomFixed = false;
	return;
    }
    range.setLeft(-1);
    table = 0;

    int p = 0;
    int p2 = _str.find('!');
    if (p2 != -1)
    {
      tableName = _str.left(p2++);
      while ( true )
      {
	table = _map->findTable(tableName);
        if ( !table && tableName[0] == ' ' )
        {
          tableName = tableName.right( tableName.length() - 1 );
          continue;
        }
        break;
      }
      p = p2;
    } else
      table = _table;


    int p3 = _str.find(':', p);
    if (p3 == -1)
	return;

    KSpreadPoint ul(_str.mid(p, p3 - p));
    KSpreadPoint lr(_str.mid(p3 + 1));
    range = QRect(ul.pos, lr.pos);

    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
}

double util_fact( double val, double end )
{
  /* fact =i*(i-1)*(i-2)*...*1 */
  if(val<0.0 || end<0.0)
    return (-1);
  if(val==0.0)
    return (1);
  else if (val==end)
    return(1);
  /*val==end => you don't multiplie it */
  else
    return (val*util_fact((double)(val-1),end));
}

bool util_isColumnSelected(const QRect &selection)
{
  return ( (selection.top() == 1) && (selection.bottom() == KS_rowMax) );
}

bool util_isRowSelected(const QRect &selection)
{
  return ( (selection.left() == 1) && (selection.right() == KS_colMax) );
}

bool util_validateTableName(QString name)
{
  if (name[0] == ' ')
  {
    return false;
  }
  for (unsigned int i = 0; i < name.length(); i++)
  {
    if ( !(name[i].isLetterOrNumber() ||
           name[i] == ' ' || name[i] == '.' ||
           name[i] == '_'))
    {
      return false;
    }
  }
  return true;
}


KSpreadRangeIterator::KSpreadRangeIterator(QRect _range, KSpreadSheet* _table)
{
  range = _range;
  table = _table;
  current = QPoint(0,0);
}

KSpreadRangeIterator::~KSpreadRangeIterator()
{
}

KSpreadCell* KSpreadRangeIterator::first()
{
  current.setY(range.top());

  /* OK, because even if this equals zero, the 'getNextCellRight' won't
     try to access it*/
  current.setX(range.left() - 1);
  return next();
}

KSpreadCell* KSpreadRangeIterator::next()
{
  if (current.x() == 0 && current.y() == 0)
  {
    return first();
  }

  KSpreadCell* cell = NULL;
  bool done = false;

  while (cell == NULL && !done)
  {
    cell = table->getNextCellRight(current.x(), current.y());
    if (cell != NULL && cell->column() > range.right())
    {
      cell = NULL;
    }

    if (cell == NULL)
    {
      current.setX(range.left() - 1);
      current.setY(current.y() + 1);
      done = (current.y() > range.bottom());
    }
  }
  return cell;
}
