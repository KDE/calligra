/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
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

#include <qdatetime.h>

#include "kspread_functions_helper.h"

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
    if ( QDate::leapYear( date.year() ) )
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

  isLeapYear = QDate::leapYear( year1 );

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

