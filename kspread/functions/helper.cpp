/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "helper.h"

#include <kdebug.h>

#include <QDateTime>

/*  DISABLED - we use KCalendarSystem instead
void addMonths( QDate & date, int months )
{
  int d = date.day();
  int m = date.month() + months;
  int y = date.year();

  if ( m > 12 )
  {
    y += (int) ( m / 12 );
    m %= 12;
  }

  // e.g. 31 Feb: decrease day...
  while ( !QDate::isValid( y, m, d ) && d > 0 )
    --d;

  date.setYMD( y, m, d );
}

void subMonths( QDate & date, int months )
{
  int d = date.day();
  int m = date.month() - months;
  int y = date.year();

  while ( m < 1 )
  {
    m += 12;
    y -= 1;
  }

  // e.g. 31 Feb: decrease day
  while ( !QDate::isValid( y, m, d ) && d > 0 )
    --d;

  date.setYMD( y, m, d );
}

*/

int KSpread::daysPerYear(QDate const & date, int basis)
{
  switch( basis )
  {
   case 0:
    return 360;

   case 1:
    if ( QDate::isLeapYear( date.year() ) )
      return 366;
    return 365;

   case 2:
    return 360;
   case 3:
    return 365;
   case 4:
    return 360;
  }

  return -1;
}

int KSpread::daysBetweenDates(QDate const & date1, QDate const & date2, int basis)
{
  int day1, day2, month1, month2, year1, year2;
  bool isLeapYear = false;
  int days, months, years;

  day1   = date1.day();
  month1 = date1.month();
  year1  = date1.year();
  day2   = date2.day();
  month2 = date2.month();
  year2  = date2.year();

  years  = year2  - year1;
  months = month2 - month1 + years * 12;
  days   = day2   - day1;

  isLeapYear = QDate::isLeapYear( year1 );

  switch (basis)
  {
   case 0:
    if ( month1 == 2 && month2 != 2 && year1 == year2 )
    {
      if ( isLeapYear )
        return months * 30 + days - 1;
      else
        return months * 30 + days - 2;
    }
    return months * 30 + days;

   case 1: // TODO: real days for difference between months!
    //    return ( month2 - month1 ) * 30 + years * 360 + days;

   case 2: // TODO: real days for difference between months!
    //    return ( month2 - month1 ) * 30 + years * 365 + days;

   case 3:
    return date1.daysTo( date2 );

   case 4:
    return months * 30 + days;
  }

  return -1;
}

// days360
int KSpread::days360( const QDate& _date1, const QDate& _date2, bool european )
{
  int day1, day2;
  int month1, month2;
  int year1, year2;
  bool negative = false;
  QDate date1( _date1 );
  QDate date2( _date2 );

  if (date1.daysTo( date2 ) < 0)
  {
    QDate tmp( date1 );
    date1 = date2;
    date2 = tmp;
    negative = true;
  }

  day1   = date1.day();
  day2   = date2.day();
  month1 = date1.month();
  month2 = date2.month();
  year1  = date1.year();
  year2  = date2.year();

  if ( european )
  {
    if ( day1 == 31 )
      day1 = 30;
    if ( day2 == 31 )
      day2 = 30;
  }
  else
  {
    // thanks to the Gnumeric developers for this...
    if ( month1 == 2 && month2 == 2
         && date1.daysInMonth() == day1
         && date2.daysInMonth() == day2 )
      day2 = 30;

    if ( month1 == 2 && date1.daysInMonth() == day1 )
      day1 = 30;

    if ( day2 == 31 && day1 >= 30 )
      day2 = 30;

    if ( day1 == 31 )
      day1 = 30;
  }

  return ( ( year2 - year1 ) * 12 + ( month2 - month1 ) ) * 30
    + ( day2 - day1 );
}

// yearFrac
double KSpread::yearFrac( const QDate& refDate, const QDate& startDate, const QDate& endDate, const int basis )
{
  QDate date1 = startDate;
  QDate date2 = endDate;

  //
  // calculation
  //

  QDate date0=refDate; // referenceDate

  if ( date2 < date1 )
  {
    // exchange dates
    QDate Temp1=date1;
    date1=date2;
    date2=Temp1;
  }

  int days = date0.daysTo(date2) - date0.daysTo(date1);

  kDebug(36002) << "date1 = " << date1 << "    date2 = " << date2 << "    days = " << days << "    basis = " << basis << endl;

  double res=0;
  double peryear=0;

  switch(basis)
  {
    case 1:
    {
      // Actual/actual
      int leaps=0,years=0;
      double k;

      if (days < (365 + QDate::isLeapYear(date1.year()) + 1))
      {
        // less than 1 year
        kDebug(36002) << "less than 1 year ..." << endl;

        // bool 1 = 29.2. is in between dates
        k = (QDate::isLeapYear(date1.year()) && date1.month()<3) || (QDate::isLeapYear(date2.year()) && date2.month()*100+date2.day() >= 2*100+29);
        years = 1;
      }
      else
      {
        // more than 1 year
        kDebug(36002) << "more than 1 year ..." << endl;
        years = date2.year()-date1.year()+1;
        leaps = QDate(date2.year()+1, 1, 1).toJulianDay() - QDate(date1.year(), 1, 1).toJulianDay() - 365*years;
        k = (double)leaps/years;
      }

      kDebug(36002) << "leaps = " << leaps << "    years = " << years << "    leaps per year = " << (double)leaps/years << endl;
      peryear = 365 + k;
      break;
    }
    case 2:
    {
      // Actual/360
      peryear = 360;
      break;
    }
    case 3:
    {
      // Actual/365
      peryear = 365;
      break;
    }
    case 4:
    {
      // 30/360 Europe

      // calc datedif360 (start, end, Europe)
      days = days360( date1, date2, 1);

      peryear = 360;
      break;
    }
    default:
    {
      // NASD 30/360
      //basis = 0;

      // calc datedif360 (start, end, US)
      days = days360( date1, date2, 0);

      peryear = 360;
    }
  }

  res = (double)days / peryear;
  kDebug()<<"getYearFrac res="<<res<<endl;
  return res;
}
