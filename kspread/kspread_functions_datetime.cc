/* This file is part of the KDE project
   Copyright (C) 1998-2003 The KSpread Team
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// built-in date/time functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>
#include <klocale.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_cell.h"
#include "kspread_functions.h"
#include "kspread_functions_helper.h"
#include "kspread_interpreter.h"
#include "kspread_util.h"
#include "kspread_value.h"

// prototypes, sorted
bool kspreadfunc_currentDate( KSContext& context );
bool kspreadfunc_currentDateTime( KSContext& context );
bool kspreadfunc_currentTime( KSContext& context );
bool kspreadfunc_date( KSContext& context );
bool kspreadfunc_datevalue( KSContext& context );
bool kspreadfunc_day( KSContext& context );
bool kspreadfunc_dayname( KSContext& context );
bool kspreadfunc_dayOfYear( KSContext& context );
bool kspreadfunc_days( KSContext& context );
bool kspreadfunc_days360( KSContext& context );
bool kspreadfunc_daysInMonth( KSContext& context );
bool kspreadfunc_daysInYear ( KSContext& context );
bool kspreadfunc_easterSunday( KSContext& context );
bool kspreadfunc_edate( KSContext& context );
bool kspreadfunc_eomonth( KSContext& context );
bool kspreadfunc_hour( KSContext& context );
bool kspreadfunc_hours( KSContext& context );
bool kspreadfunc_isLeapYear ( KSContext& context );
bool kspreadfunc_isoWeekNum( KSContext& context );
bool kspreadfunc_minute( KSContext& context );
bool kspreadfunc_minutes( KSContext& context );
bool kspreadfunc_month( KSContext& context );
bool kspreadfunc_monthname( KSContext& context );
bool kspreadfunc_months( KSContext& context );
bool kspreadfunc_second( KSContext& context );
bool kspreadfunc_seconds( KSContext& context );
bool kspreadfunc_shortcurrentDate( KSContext& context );
bool kspreadfunc_time( KSContext& context );
bool kspreadfunc_timevalue( KSContext& context );
bool kspreadfunc_today( KSContext& context );
bool kspreadfunc_weekday( KSContext& context );
bool kspreadfunc_weeks( KSContext& context );
bool kspreadfunc_weeksInYear( KSContext& context );
bool kspreadfunc_year( KSContext& context );
bool kspreadfunc_years( KSContext& context );

// registers all date/time functions
// sadly, many of these functions aren't Excel compatible
void KSpreadRegisterDateTimeFunctions()
{
  // missing: Excel:    WORKDAY, NETWORKDAYS, WEEKNUM, DATEDIF
  //          Gnumeric: UNIX2DATE, DATE2UNIX
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();
  repo->registerFunction( "CURRENTDATE",  kspreadfunc_currentDate );
  repo->registerFunction( "CURRENTDATETIME",  kspreadfunc_currentDateTime );
  repo->registerFunction( "CURRENTTIME",  kspreadfunc_currentTime );
  repo->registerFunction( "DATE",  kspreadfunc_date );
  repo->registerFunction( "DATEVALUE",  kspreadfunc_datevalue );
  repo->registerFunction( "DAY",  kspreadfunc_day );
  repo->registerFunction( "DAYNAME",  kspreadfunc_dayname );
  repo->registerFunction( "DAYOFYEAR",  kspreadfunc_dayOfYear );
  repo->registerFunction( "DAYS",  kspreadfunc_days );
  repo->registerFunction( "DAYS360",  kspreadfunc_days360 );
  repo->registerFunction( "DAYSINMONTH",  kspreadfunc_daysInMonth );
  repo->registerFunction( "DAYSINYEAR",  kspreadfunc_daysInYear );
  repo->registerFunction( "EASTERSUNDAY",  kspreadfunc_easterSunday );
  repo->registerFunction( "EDATE",  kspreadfunc_edate );
  repo->registerFunction( "EOMONTH",  kspreadfunc_eomonth );
  repo->registerFunction( "HOUR",  kspreadfunc_hour );
  repo->registerFunction( "HOURS",  kspreadfunc_hours );
  repo->registerFunction( "ISLEAPYEAR",  kspreadfunc_isLeapYear );
  repo->registerFunction( "ISOWEEKNUM",  kspreadfunc_isoWeekNum );
  repo->registerFunction( "MINUTE",  kspreadfunc_minute );
  repo->registerFunction( "MINUTES",  kspreadfunc_minutes );
  repo->registerFunction( "MONTH",  kspreadfunc_month );
  repo->registerFunction( "MONTHNAME",  kspreadfunc_monthname );
  repo->registerFunction( "MONTHS",  kspreadfunc_months );
  repo->registerFunction( "NOW",  kspreadfunc_currentDateTime );
  repo->registerFunction( "SECOND",  kspreadfunc_second );
  repo->registerFunction( "SECONDS",  kspreadfunc_seconds );
  repo->registerFunction( "SHORTCURRENTDATE",  kspreadfunc_shortcurrentDate );
  repo->registerFunction( "TIME",  kspreadfunc_time );
  repo->registerFunction( "TIMEVALUE",  kspreadfunc_timevalue );
  repo->registerFunction( "TODAY",  kspreadfunc_today );
  repo->registerFunction( "WEEKDAY",  kspreadfunc_weekday );
  repo->registerFunction( "WEEKS",  kspreadfunc_weeks );
  repo->registerFunction( "WEEKSINYEAR",  kspreadfunc_weeksInYear );
  repo->registerFunction( "YEAR",   kspreadfunc_year );
  repo->registerFunction( "YEARS",  kspreadfunc_years );
}

// Function: EDATE
bool kspreadfunc_edate( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "EDATE", true ) )
    return false;

  QDate date;

  if ( !getDate( context, args[0], date ) )
    return false;

  int months;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
      return false;

    months = (int) args[1]->doubleValue();
  }

  months = args[1]->intValue();

  if ( months > 0 )
    addMonths( date, months );
  else
    subMonths( date, -months );

  if ( !date.isValid() )
    return false;

  context.setValue( new KSValue( date ) );
  return true;
}

// Function: EOMONTH
bool kspreadfunc_eomonth( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  QDate date;
  int months = 0;

  if ( !KSUtil::checkArgumentsCount( context, 2, "EOMONTH", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 1, "EOMONTH", true ) )
      return false;

    months = 0;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
      return false;

    months = (int) args[1]->doubleValue();
  }

  if ( !getDate( context, args[0], date ) )
    return false;

  if ( months > 0 )
    addMonths( date, months );
  else
    subMonths( date, -months );

  if ( !date.isValid() )
    return false;

  date.setYMD( date.year(), date.month(), date.daysInMonth() );

  context.setValue( new KSValue( date ) );
  return true;
}

// Function: DAYS360
// algorithm adapted from gnumeric
bool kspreadfunc_days360( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  QDate date1;
  QDate date2;
  bool european = false;

  if ( !KSUtil::checkArgumentsCount( context, 3, "DAYS360", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 2, "DAYS360", true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[2], KSValue::BoolType, true ) )
      return false;

    european = args[2]->boolValue();
  }

  if ( !getDate( context, args[0], date1 ) )
    return false;

  if ( !getDate( context, args[1], date2 ) )
    return false;

  int day1, day2;
  int month1, month2;
  int year1, year2;
  bool negative = false;

  if ( date1.daysTo( date2 ) < 0 )
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

  context.setValue( new KSValue( ( negative ? -result : result ) ) );
  return true;
}

// Function: YEAR
bool kspreadfunc_year( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "YEAR", false ) )
  {
    context.setValue( new KSValue( QDate::currentDate().year() ) );
    return true;
  }

  QDate date;
  if ( !getDate( context, args[0], date ) )
    return false;

  context.setValue( new KSValue( date.year() ) );
  return true;
}

// Function: MONTH
bool kspreadfunc_month( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "MONTH", false ) )
  {
    context.setValue( new KSValue( QDate::currentDate().month() ) );
    return true;
  }

  QDate date;
  if ( !getDate( context, args[0], date ) )
    return false;

  context.setValue( new KSValue( date.month() ) );
  return true;
}

// Function: DAY
bool kspreadfunc_day( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "DAY", false ) )
  {
    context.setValue( new KSValue( QDate::currentDate().day() ) );
    return true;
  }

  QDate date;
  if ( !getDate( context, args[0], date ) )
    return false;

  context.setValue( new KSValue( date.day() ) );
  return true;
}

// Function: HOUR
bool kspreadfunc_hour( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int hour;

  if ( !KSUtil::checkArgumentsCount( context, 1, "HOUR", false ) )
  {
    context.setValue( new KSValue( QTime::currentTime().hour() ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
  {
    hour = args[0]->timeValue().hour();
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
  {
    double d = args[0]->doubleValue() + HALFSEC;

    uint secs = (uint) ( ( d - floor( d ) ) * SECSPERDAY );

    hour = secs / 3600;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
  {
    QString s = args[0]->stringValue();
    bool valid = false;
    QTime tmpTime = KGlobal::locale()->readTime( s, &valid );
    if ( !valid )
      return false;

    hour = tmpTime.hour();
  }
  else
    return false;

  context.setValue( new KSValue( hour ) );
  return true;
}

// Function: MINUTE
bool kspreadfunc_minute( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int minute;

  if ( !KSUtil::checkArgumentsCount( context, 1, "MINUTE", false ) )
  {
    context.setValue( new KSValue( QTime::currentTime().minute() ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
  {
    minute = args[0]->timeValue().minute();
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
  {
    double d = args[0]->doubleValue() + HALFSEC;

    uint secs = (uint) ( ( d - floor( d ) ) * SECSPERDAY );
    minute = ( secs / 60 ) % 60;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
  {
    QString s = args[0]->stringValue();
    bool valid = false;
    QTime tmpTime = KGlobal::locale()->readTime( s, &valid );
    if ( !valid )
      return false;

    minute = tmpTime.minute();
  }
  else
    return false;

  context.setValue( new KSValue( minute ) );
  return true;
}

// Function: SECOND
bool kspreadfunc_second( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int second;

  if ( !KSUtil::checkArgumentsCount( context, 1, "SECOND", true ) )
  {
    context.setValue( new KSValue( QTime::currentTime().second() ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
  {
    second = args[0]->timeValue().second();
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
  {
    double d = args[0]->doubleValue() + HALFSEC;

    uint secs = (uint) ( ( d - floor( d ) ) * SECSPERDAY );
    second = secs % 60;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
  {
    QString s = args[0]->stringValue();
    bool valid = false;
    QTime tmpTime = KGlobal::locale()->readTime( s, &valid );
    if ( !valid )
      return false;

    second = tmpTime.second();
  }
  else
    return false;

  context.setValue( new KSValue( second ) );
  return true;
}

// Function: weekday
bool kspreadfunc_weekday( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int method = 1;

  if ( !KSUtil::checkArgumentsCount( context, 2, "WEEKDAY", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 1, "WEEKDAY", false ) )
      return false;

    method = 1;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
      return false;

    method = args[1]->intValue();

    if ( method < 1 || method > 3 )
      return false;
  }

  QDate date;

  if ( !getDate( context, args[0], date ) )
    return false;

  int result = date.dayOfWeek();

  if ( method == 3 )
    --result;
  else if ( method == 1 )
  {
    ++result;
    result = result % 7;
  }

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: datevalue
bool kspreadfunc_datevalue( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "DATEVALUE", true ) )
    return false;

  QDate date;

  if ( !getDate( context, args[0], date ) )
    return false;

  long int result = (long int) EDate::greg2jul( date );

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: timevalue
bool kspreadfunc_timevalue( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "TIMEVALUE", true ) )
    return false;

  QTime time;

  if ( !getTime( context, args[0], time ) )
    return false;

  double result = time.hour() * 3600 + time.minute() * 60 + time.second();
  result = result / (double) SECSPERDAY;

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: years
bool kspreadfunc_years( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "YEARS", true ) )
    return false;

  // date1 is supposed to be the smaller one
  QDate date1;
  QDate date2;

  if (!KSUtil::checkType( context, args[2], KSValue::IntType, true ))
    return false;

  if ( !getDate( context, args[0], date1 ) )
    return false;

  if ( !getDate( context, args[1], date2 ) )
    return false;

  if (!date1.isValid())
    return false;

  if (!date2.isValid())
    return false;

  int type  = args[2]->intValue();
  int years = 0;

  if (type == 0)
  {
    // max. possible years between both dates

    years  = date2.year() - date1.year();

    if (date2.month() < date1.month())
    {
      --years;
    }
    else if ( (date2.month() == date1.month()) && (date2.day() < date1.day()) )
    {
      --years;
    }

    context.setValue( new KSValue( years ) );
  }
  else
    //  if (type == 1)
  {
    // the number of full years in between, starting on 1/1/XXXX
    if ( date1.year() == date2.year() )
    {
      context.setValue( new KSValue( 0 ) );

      return true;
    }

    if ( (date1.month() != 1) || (date1.day() != 1) )
      date1.setYMD(date1.year() + 1, 1, 1);

    date2.setYMD(date2.year(), 1, 1);

    context.setValue( new KSValue( date2.year() - date1.year() ) );
  }

  return true;
}

// Function: months
bool kspreadfunc_months( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "WEEKS", true ) )
    return false;

  // date1 is supposed to be the smaller one
  QDate date1;
  QDate date2;

  if (!KSUtil::checkType( context, args[2], KSValue::IntType, true ))
    return false;

  if ( !getDate( context, args[0], date1 ) )
    return false;

  if ( !getDate( context, args[1], date2 ) )
    return false;

  if (!date1.isValid())
    return false;

  if (!date2.isValid())
    return false;

  int type   = args[2]->intValue();
  int months = 0;

  if (type == 0)
  {
    months  = (date2.year() - date1.year()) * 12;
    months += date2.month() - date1.month();

    if (date2.day() < date1.day())
    {
      if (date2.day() != date2.daysInMonth())
        --months;
    }

    context.setValue( new KSValue( months ) );
  }
  else
  //  if (type == 1)
  {
    // the number of full months in between, starting on 1/XX/XXXX
    if (date1.month() == 12)
      date1.setYMD(date1.year() + 1, 1, 1);
    else
      date1.setYMD(date1.year(), date1.month() + 1, 1);
    date2.setYMD(date2.year(), date2.month(), 1);

    months  = (date2.year() - date1.year()) * 12;
    months += date2.month() - date1.month();

    context.setValue( new KSValue( months ) );
  }

  return true;
}

// Function: weeks
bool kspreadfunc_weeks( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "WEEKS", true ) )
    return false;

  // date1 is supposed to be the smaller one
  QDate date1;
  QDate date2;

  if (!KSUtil::checkType( context, args[2], KSValue::IntType, true ))
    return false;

  if ( !getDate( context, args[0], date1 ) )
    return false;

  if ( !getDate( context, args[1], date2 ) )
    return false;

  if (!date1.isValid())
    return false;

  if (!date2.isValid())
    return false;

  int type = args[2]->intValue();

  int days = date1.daysTo(date2);

  if (type == 0)
  {
    // just the number of full weeks between
    context.setValue( new KSValue( (int)(days / 7) ) );
    return true;
  }
  else
    //  if (type == 1)
  {
    // the number of full weeks between starting on mondays
    bool mondayFirstDay = KGlobal::locale()->weekStartsMonday();

    int dow1 = date1.dayOfWeek();
    int dow2 = date2.dayOfWeek();

    if (mondayFirstDay)
    {
      days -= (8 - dow1);
      days -= (dow2 - 1);
    }
    else
    {
      days -= (7 - dow1);
      days -= dow2;
    }

    context.setValue( new KSValue( (int) (days / 7) ) );
  }

  return true;
}

// Function: days
bool kspreadfunc_days( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "DAYS", true ) )
    return false;

  QDate date1;
  QDate date2;

  if ( !getDate( context, args[0], date1 ) )
    return false;

  if ( !getDate( context, args[1], date2 ) )
    return false;

  if (!date1.isValid())
    return false;

  if (!date2.isValid())
    return false;

  int result = date1.daysTo(date2);

  context.setValue( new KSValue(result));
  return true;
}

// Function: hours
bool kspreadfunc_hours( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "hours", true ) )
    return false;

  QTime tmpTime;

  if (!getTime(context, args[0], tmpTime))
    return false;

  context.setValue( new KSValue( tmpTime.hour() ) );

  return true;
}

// Function: minutes
bool kspreadfunc_minutes( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "minutes", true ) )
    return false;

  QTime tmpTime;

  if (!getTime(context, args[0], tmpTime))
    return false;

  context.setValue( new KSValue( tmpTime.minute() ) );

  return true;
}

// Function: seconds
bool kspreadfunc_seconds( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "seconds", true ) )
    return false;

  QTime tmpTime;

  if (!getTime(context, args[0], tmpTime))
    return false;

  context.setValue( new KSValue( tmpTime.second() ) );

  return true;
}

// Function: date
bool kspreadfunc_date( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "date",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  QDate _date;
  if( _date.setYMD(args[0]->intValue(), args[1]->intValue(), args[2]->intValue()) )
    context.setValue( new KSValue(KGlobal::locale()->formatDate(_date)));
  else
    context.setValue( new KSValue(i18n("Err")) );

  return true;
}

// Function: day
bool kspreadfunc_dayname( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString tmp;
  if ( !KSUtil::checkArgumentsCount( context,1, "DAYNAME", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( KGlobal::locale()->weekDayName( args[0]->intValue() ).isNull() )
    tmp = i18n( "Err" );
  else
    tmp = KGlobal::locale()->weekDayName( args[0]->intValue() );

  context.setValue( new KSValue( tmp ) );
  return true;
}

// Function: monthname
bool kspreadfunc_monthname( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString tmp;
  if ( !KSUtil::checkArgumentsCount( context, 1, "MONTHNAME", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( KGlobal::locale()->monthName( args[0]->intValue()).isNull() )
    tmp = i18n( "Err" );
  else
    tmp = KGlobal::locale()->monthName( args[0]->intValue() );

  context.setValue( new KSValue( tmp ) );
  return true;
}

// Function: time
bool kspreadfunc_time( KSContext& context )
{
  int hour;
  int minute;
  int second;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "time",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  hour = args[0]->intValue();
  minute = args[1]->intValue();
  second = args[2]->intValue();

  /* normalize the data */
  minute += second / 60;
  second = second % 60;
  hour += minute / 60;
  minute = minute % 60;

  /* we'll lose hours data that carries over into days */
  hour = hour % 24;

  /* now carry down hours/minutes for negative minutes/seconds */

  if (second < 0)
  {
    second += 60;
    minute -= 1;
  }

  if (minute < 0)
  {
    minute += 60;
    hour -= 1;
  }

  if (hour < 0)
  {
    hour += 24;
  }

  context.setValue( new KSValue(KGlobal::locale()->formatTime(
    QTime(hour, minute, second),true )));

  return true;
}

// Function: currentDate
bool kspreadfunc_currentDate( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context,0, "currentDate",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatDate(QDate::currentDate())));

    return true;
}

// Function: shortcurrentDate
bool kspreadfunc_shortcurrentDate( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context,0, "shortcurrentDate",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatDate(QDate::currentDate(),true)));

    return true;
}

// Function: today
bool kspreadfunc_today( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context,0, "shortcurrentDate",true ) )
      return false;

    context.setValue( new KSValue( QDate::currentDate() ) );
    KSpreadCell *  cell  = ((KSpreadInterpreter *) context.interpreter() )->cell();
    cell->setFormatType( KSpreadFormat::ShortDate );


    return true;
}

// Function: currentTime
bool kspreadfunc_currentTime( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context,0, "currentTime",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatTime(QTime::currentTime())));

    return true;
}

// Function: currentDateTime
bool kspreadfunc_currentDateTime( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context,0, "currentDateTime",true ) )
      return false;

    context.setValue( new KSValue(KGlobal::locale()->formatDateTime(QDateTime::currentDateTime(), false)));

    return true;
}

// Function: dayOfYear
bool kspreadfunc_dayOfYear( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,3, "dayOfYear",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  context.setValue( new KSValue(QDate(args[0]->intValue(),
                                      args[1]->intValue(),args[2]->intValue()).dayOfYear() ));

  return true;
}

bool isLeapYear_helper(int _year)
{
    return (((_year % 4) == 0) && ((_year % 100) != 0) || ((_year % 400) == 0));
}

// Function: daysInMonth
bool kspreadfunc_daysInMonth( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,2,"daysInMonth",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
  {
    context.setValue( new KSValue( i18n("Err") ) );
    return true;
  }
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
  {
    context.setValue( new KSValue( i18n("Err") ) );
    return true;
  }

  static uint aDaysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  int nYear = args[0]->intValue();
  int nMonth = args[1]->intValue();
  int result;

  if ( nMonth != 2)
    result = aDaysInMonth[nMonth-1];
  else
  {
    if (isLeapYear_helper(nYear))
        result = aDaysInMonth[nMonth-1] + 1;
    else
        result = aDaysInMonth[nMonth-1];
  }

  context.setValue( new KSValue(result));

  return true;
}

// Function: isLeapYear
bool kspreadfunc_isLeapYear ( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1,"isLeapYear",true ) )
    return false;
  bool result=true;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if(result)
  {
    int nYear = args[0]->intValue();
    result = isLeapYear_helper(nYear);
  }

  context.setValue( new KSValue(result));

  return true;
}

// Function: daysInYear
bool kspreadfunc_daysInYear ( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1,"daysInYear",true ) )
    return false;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  int nYear = args[0]->intValue();
  bool leap = isLeapYear_helper(nYear);
  int result;

  if (leap)
    result = 366;
  else
    result = 365;

  context.setValue( new KSValue(result));

  return true;
}

// Function: weeksInYear
bool kspreadfunc_weeksInYear( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1,"weeksInYear",true ) )
    return false;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  int nYear = args[0]->intValue();
  int result;
  QDate _date(nYear, 1, 1);
  int nJan1DayOfWeek = _date.dayOfWeek();   //first day of the year

  if ( nJan1DayOfWeek == 4 ) { // Thursday
        result = 53;
  } else if ( nJan1DayOfWeek == 3 ) { // Wednesday
        result = isLeapYear_helper(nYear) ? 53 : 52 ;
  } else {
        result = 52;
  }

  context.setValue( new KSValue(result));

  return true;
}

// Function: easterSunday
bool kspreadfunc_easterSunday( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();
    if ( !KSUtil::checkArgumentsCount( context,1,"easterSunday",true ) )
        return false;
    if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        return false;

    int nDay, nMonth;
    int nYear = args[0]->intValue();

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

    context.setValue( new KSValue( KGlobal::locale()->formatDate( QDate(nYear, nMonth, nDay) ) ) );

    return true;
}

// Function: isoWeekNum
bool kspreadfunc_isoWeekNum( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();
    if ( !KSUtil::checkArgumentsCount( context,1,"isoWeekNum",true ) )
        return false;

    QDate tmpDate;

    if (!getDate(context, args[0], tmpDate))
        return false;

    if (!tmpDate.isValid())
        return false;

    int result = tmpDate.weekNumber();

    if (result==0)
        return false;

    context.setValue( new KSValue(result) );
    return true;
}
