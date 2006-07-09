/* This file is part of the KDE project
   Copyright (C) 1998-2003 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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

// built-in date/time functions

#include "Functions.h"
#include "functions/helper.h"
#include "Valuecalc.h"
#include "Valueconverter.h"

#include <kcalendarsystem.h>
#include <klocale.h>

using namespace KSpread;

// prototypes, sorted
Value func_currentDate (valVector args, ValueCalc *calc, FuncExtra *);
Value func_currentDateTime (valVector args, ValueCalc *calc, FuncExtra *);
Value func_currentTime (valVector args, ValueCalc *calc, FuncExtra *);
Value func_date (valVector args, ValueCalc *calc, FuncExtra *);
Value func_datevalue (valVector args, ValueCalc *calc, FuncExtra *);
Value func_day (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dayname (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dayOfYear (valVector args, ValueCalc *calc, FuncExtra *);
Value func_days (valVector args, ValueCalc *calc, FuncExtra *);
Value func_days360 (valVector args, ValueCalc *calc, FuncExtra *);
Value func_daysInMonth (valVector args, ValueCalc *calc, FuncExtra *);
Value func_daysInYear  (valVector args, ValueCalc *calc, FuncExtra *);
Value func_easterSunday (valVector args, ValueCalc *calc, FuncExtra *);
Value func_edate (valVector args, ValueCalc *calc, FuncExtra *);
Value func_eomonth (valVector args, ValueCalc *calc, FuncExtra *);
Value func_hour (valVector args, ValueCalc *calc, FuncExtra *);
Value func_hours (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isLeapYear  (valVector args, ValueCalc *calc, FuncExtra *);
Value func_isoWeekNum (valVector args, ValueCalc *calc, FuncExtra *);
Value func_minute (valVector args, ValueCalc *calc, FuncExtra *);
Value func_minutes (valVector args, ValueCalc *calc, FuncExtra *);
Value func_month (valVector args, ValueCalc *calc, FuncExtra *);
Value func_monthname (valVector args, ValueCalc *calc, FuncExtra *);
Value func_months (valVector args, ValueCalc *calc, FuncExtra *);
Value func_second (valVector args, ValueCalc *calc, FuncExtra *);
Value func_seconds (valVector args, ValueCalc *calc, FuncExtra *);
Value func_time (valVector args, ValueCalc *calc, FuncExtra *);
Value func_timevalue (valVector args, ValueCalc *calc, FuncExtra *);
Value func_today (valVector args, ValueCalc *calc, FuncExtra *);
Value func_weekday (valVector args, ValueCalc *calc, FuncExtra *);
Value func_weeks (valVector args, ValueCalc *calc, FuncExtra *);
Value func_weeksInYear (valVector args, ValueCalc *calc, FuncExtra *);
Value func_year (valVector args, ValueCalc *calc, FuncExtra *);
Value func_years (valVector args, ValueCalc *calc, FuncExtra *);

// registers all date/time functions
// sadly, many of these functions aren't Excel compatible
void RegisterDateTimeFunctions()
{
  // missing: Excel:    WORKDAY, NETWORKDAYS, WEEKNUM, DATEDIF
  //          Gnumeric: UNIX2DATE, DATE2UNIX
  // TODO: do we really need DATEVALUE and TIMEVALUE ?
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("CURRENTDATE",  func_currentDate);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("CURRENTDATETIME",  func_currentDateTime);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("CURRENTTIME",  func_currentTime);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("DATE",  func_date);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("DATEVALUE",  func_datevalue);
  repo->add (f);
  f = new Function ("DAY",  func_day);
  repo->add (f);
  f = new Function ("DAYNAME",  func_dayname);
  repo->add (f);
  f = new Function ("DAYOFYEAR",  func_dayOfYear);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("DAYS",  func_days);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("DAYS360",  func_days360);
  f->setParamCount (2, 3);
  repo->add (f);
  f = new Function ("DAYSINMONTH",  func_daysInMonth);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("DAYSINYEAR",  func_daysInYear);
  repo->add (f);
  f = new Function ("EASTERSUNDAY",  func_easterSunday);
  repo->add (f);
  f = new Function ("EDATE",  func_edate);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("EOMONTH",  func_eomonth);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("HOUR",  func_hour);
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("HOURS",  func_hour);  // same as HOUR
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("ISLEAPYEAR",  func_isLeapYear);
  repo->add (f);
  f = new Function ("ISOWEEKNUM",  func_isoWeekNum);
  repo->add (f);
  f = new Function ("MINUTE",  func_minute);
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("MINUTES",  func_minute);  // same as MINUTE
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("MONTH",  func_month);
  repo->add (f);
  f = new Function ("MONTHNAME",  func_monthname);
  repo->add (f);
  f = new Function ("MONTHS",  func_months);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("NOW",  func_currentDateTime);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("SECOND",  func_second);
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("SECONDS",  func_second);  // same as SECOND
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("TIME",  func_time);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("TIMEVALUE",  func_timevalue);
  repo->add (f);
  f = new Function ("TODAY",  func_currentDate);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("WEEKDAY",  func_weekday);
  f->setParamCount (1, 2);
  repo->add (f);
  f = new Function ("WEEKS",  func_weeks);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("WEEKSINYEAR",  func_weeksInYear);
  repo->add (f);
  f = new Function ("YEAR",   func_year);
  repo->add (f);
  f = new Function ("YEARS",  func_years);
  f->setParamCount (3);
  repo->add (f);
}

// Function: EDATE
Value func_edate (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date = calc->conv()->asDate (args[0]).asDate();
  int months = calc->conv()->asInteger (args[1]).asInteger();

  date = calc->conv()->locale()->calendar()->addMonths (date, months);

  if (!date.isValid())
    return Value::errorVALUE();

  return Value (date);
}

// Function: EOMONTH
Value func_eomonth (valVector args, ValueCalc *calc, FuncExtra *)
{
  // add months to date using EDATE
  Value modDate = func_edate (args, calc, 0);
  if (modDate.isError()) return modDate;

  // modDate is currently in Date format
  QDate date = modDate.asDate();
  date.setYMD (date.year(), date.month(), date.daysInMonth());

  return Value (date);
}

// Function: DAYS360
// algorithm adapted from gnumeric
Value func_days360 (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date1 = calc->conv()->asDate (args[0]).asDate();
  QDate date2 = calc->conv()->asDate (args[1]).asDate();
  bool european = false;
  if (args.count() == 3)
    european = calc->conv()->asBoolean (args[2]).asBoolean();

  int day1, day2;
  int month1, month2;
  int year1, year2;
  bool negative = false;

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

  int result = ( ( year2 - year1 ) * 12 + ( month2 - month1 ) ) * 30
    + ( day2 - day1 );

  return Value (result);
}

// Function: YEAR
Value func_year (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value v = calc->conv()->asDate (args[0]);
  if (v.isError()) return v;
  QDate date = v.asDate();
  return Value (date.year ());
}

// Function: MONTH
Value func_month (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value v = calc->conv()->asDate (args[0]);
  if (v.isError()) return v;
  QDate date = v.asDate();
  return Value (date.month ());
}

// Function: DAY
Value func_day (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value v = calc->conv()->asDate (args[0]);
  if (v.isError()) return v;
  QDate date = v.asDate();
  return Value (date.day ());
}

// Function: HOUR
Value func_hour (valVector args, ValueCalc *calc, FuncExtra *)
{
  QTime time;
  if (args.count() == 1)
  {
    Value v = calc->conv()->asTime (args[0]);
    if (v.isError()) return v;
    time = v.asTime();
  }
  else
    time = QTime::currentTime ();
  return Value (time.hour ());
}

// Function: MINUTE
Value func_minute (valVector args, ValueCalc *calc, FuncExtra *)
{
  QTime time;
  if (args.count() == 1)
  {
    Value v = calc->conv()->asTime (args[0]);
    if (v.isError()) return v;
    time = v.asTime();
  }
  else
    time = QTime::currentTime ();
  return Value (time.minute ());
}

// Function: SECOND
Value func_second (valVector args, ValueCalc *calc, FuncExtra *)
{
  QTime time;
  if (args.count() == 1)
  {
    Value v = calc->conv()->asTime (args[0]);
    if (v.isError()) return v;
    time = v.asTime();
  }
  else
    time = QTime::currentTime ();
  return Value (time.second ());
}

// Function: weekday
Value func_weekday (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value v = calc->conv()->asDate (args[0]).asDate();
  if (v.isError()) return v;
  QDate date = v.asDate();
  int method = 1;
  if (args.count() == 2)
    method = calc->conv()->asInteger (args[1]).asInteger();

  if ( method < 1 || method > 3 )
    return Value::errorVALUE();

  int result = date.dayOfWeek();

  if (method == 3)
    --result;
  else if (method == 1)
  {
    ++result;
    result = result % 7;
  }

  return Value (result);
}

// Function: datevalue
// same result would be obtained by applying number format on a date value
Value func_datevalue (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asFloat (calc->conv()->asDate (args[0]));
}

// Function: timevalue
// same result would be obtained by applying number format on a time value
Value func_timevalue (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asFloat (calc->conv()->asTime (args[0]));
}

// Function: years
Value func_years (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date1 = calc->conv()->asDate (args[0]).asDate();
  QDate date2 = calc->conv()->asDate (args[1]).asDate();
  if (!date1.isValid() || !date2.isValid())
    return Value::errorVALUE();

  int type = calc->conv()->asInteger (args[2]).asInteger();
  if (type == 0)
  {
    // max. possible years between both dates
    int years = date2.year() - date1.year();

    if (date2.month() < date1.month())
      --years;
    else if ( (date2.month() == date1.month()) && (date2.day() < date1.day()) )
      --years;

    return Value (years);
  }

  // type is non-zero now
  // the number of full years in between, starting on 1/1/XXXX
  if ( date1.year() == date2.year() )
    return Value (0);

  if ( (date1.month() != 1) || (date1.day() != 1) )
    date1.setYMD(date1.year() + 1, 1, 1);
  date2.setYMD(date2.year(), 1, 1);

  return Value (date2.year() - date1.year());
}

// Function: months
Value func_months (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date1 = calc->conv()->asDate (args[0]).asDate();
  QDate date2 = calc->conv()->asDate (args[1]).asDate();
  if (!date1.isValid() || !date2.isValid())
    return Value::errorVALUE();

  int type = calc->conv()->asInteger (args[2]).asInteger();
  if (type == 0)
  {
    int months  = (date2.year() - date1.year()) * 12;
    months += date2.month() - date1.month();

    if (date2.day() < date1.day())
      if (date2.day() != date2.daysInMonth())
        --months;

    return Value (months);
  }

  // type is now non-zero
  // the number of full months in between, starting on 1/XX/XXXX
  if (date1.month() == 12)
    date1.setYMD(date1.year() + 1, 1, 1);
  else
    date1.setYMD(date1.year(), date1.month() + 1, 1);
  date2.setYMD(date2.year(), date2.month(), 1);

  int months = (date2.year() - date1.year()) * 12;
  months += date2.month() - date1.month();

  return Value (months);
}

// Function: weeks
Value func_weeks (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date1 = calc->conv()->asDate (args[0]).asDate();
  QDate date2 = calc->conv()->asDate (args[1]).asDate();
  if (!date1.isValid() || !date2.isValid())
    return Value::errorVALUE();

  int type = calc->conv()->asInteger (args[2]).asInteger();
  int days = date1.daysTo (date2);
  if (type == 0)
    // just the number of full weeks between
    return Value ((int) (days / 7));

  // the number of full weeks between starting on mondays
  int weekStartDay = calc->conv()->locale()->weekStartDay();

  int dow1 = date1.dayOfWeek();
  int dow2 = date2.dayOfWeek();

  days -= (7 + (weekStartDay % 7) - dow1);
  days -= ((dow2 - weekStartDay) % 7);

  return Value ((int) (days / 7));
}

// Function: days
Value func_days (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date1 = calc->conv()->asDate (args[0]).asDate();
  QDate date2 = calc->conv()->asDate (args[1]).asDate();
  if (!date1.isValid() || !date2.isValid())
    return Value::errorVALUE();

  return Value (date1.daysTo (date2));
}

// Function: date
Value func_date (valVector args, ValueCalc *calc, FuncExtra *)
{
  int y = calc->conv()->asInteger (args[0]).asInteger();
  int m = calc->conv()->asInteger (args[1]).asInteger();
  int d = calc->conv()->asInteger (args[2]).asInteger();

  QDate _date;
  if( _date.setYMD (y, m, d))
    return Value (_date);
  return Value::errorVALUE();
}

// Function: day
Value func_dayname (valVector args, ValueCalc *calc, FuncExtra *)
{
  int number = calc->conv()->asInteger (args[0]).asInteger();

  QString weekName = calc->conv()->locale()->calendar()->weekDayName (number);
  if (weekName.isNull())
    return Value::errorVALUE();
  return Value (weekName);
}

// Function: monthname
Value func_monthname (valVector args, ValueCalc *calc, FuncExtra *)
{
  int number = calc->conv()->asInteger (args[0]).asInteger();

  QString monthName = calc->conv()->locale()->calendar()->monthName (number,
      QDate::currentDate().year());
  if (monthName.isNull())
    return Value::errorVALUE();
  return Value (monthName);
}

// Function: time
Value func_time (valVector args, ValueCalc *calc, FuncExtra *)
{
  int h = calc->conv()->asInteger (args[0]).asInteger();
  int m = calc->conv()->asInteger (args[1]).asInteger();
  int s = calc->conv()->asInteger (args[2]).asInteger();

  /* normalize the data */
  m += s / 60;
  s = s % 60;
  h += m / 60;
  m = m % 60;
  // we'll lose hours data that carries over into days
  h = h % 24;

  // now carry down hours/minutes for negative minutes/seconds
  if (s < 0) {
    s += 60;
    m -= 1;
  }
  if (m < 0) {
    m += 60;
    h -= 1;
  }
  if (h < 0)
    h += 24;

  return Value (QTime (h, m, s));
}

// Function: currentDate
Value func_currentDate (valVector, ValueCalc *, FuncExtra *)
{
  return Value (QDate::currentDate ());
}

// Function: currentTime
Value func_currentTime (valVector, ValueCalc *, FuncExtra *)
{
  return Value (QTime::currentTime ());
}

// Function: currentDateTime
Value func_currentDateTime (valVector, ValueCalc *, FuncExtra *)
{
  return Value (QDateTime::currentDateTime ());
}

// Function: dayOfYear
Value func_dayOfYear (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value date = func_date (args, calc, 0);
  if (date.isError()) return date;
  return Value (date.asDate().dayOfYear());
}

// Function: daysInMonth
Value func_daysInMonth (valVector args, ValueCalc *calc, FuncExtra *)
{
  int y = calc->conv()->asInteger (args[0]).asInteger();
  int m = calc->conv()->asInteger (args[1]).asInteger();
  QDate date (y, m, 1);
  return Value (date.daysInMonth());
}

// Function: isLeapYear
Value func_isLeapYear  (valVector args, ValueCalc *calc, FuncExtra *)
{
  int y = calc->conv()->asInteger (args[0]).asInteger();
  return Value (QDate::isLeapYear (y));
}

// Function: daysInYear
Value func_daysInYear  (valVector args, ValueCalc *calc, FuncExtra *)
{
  int y = calc->conv()->asInteger (args[0]).asInteger();
  return Value (QDate::isLeapYear (y) ? 366 : 365);
}

// Function: weeksInYear
Value func_weeksInYear (valVector args, ValueCalc *calc, FuncExtra *)
{
  int y = calc->conv()->asInteger (args[0]).asInteger();
  QDate date (y, 12, 31);  // last day of the year
  return Value (date.weekNumber ());
}

// Function: easterSunday
Value func_easterSunday (valVector args, ValueCalc *calc, FuncExtra *)
{
  int nDay, nMonth;
  int nYear = calc->conv()->asInteger (args[0]).asInteger();

  // (Tomas) the person who wrote this should be hanged :>
  int B,C,D,E,F,G,H,I,K,L,M,N,O;
  N = nYear % 19;
  B = int(nYear / 100);
  C = nYear % 100;
  D = int(B / 4);
  E = B % 4;
  F = int((B + 8) / 25);
  G = int((B - F + 1) / 3);
  H = (19 * N + B - D - G + 15) % 30;
  I = int(C / 4);
  K = C % 4;
  L = (32 + 2 * E + 2 * I - H - K) % 7;
  M = int((N + 11 * H + 22 * L) / 451);
  O = H + L - 7 * M + 114;
  nDay = O % 31 + 1;
  nMonth = int(O / 31);

  return Value (QDate (nYear, nMonth, nDay));
}

// Function: isoWeekNum
Value func_isoWeekNum (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate date = calc->conv()->asDate (args[0]).asDate();
  if (!date.isValid())
      return Value::errorVALUE();

  return Value (date.weekNumber());
}
