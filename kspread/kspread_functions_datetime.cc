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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// built-in date/time functions
#include "kspread_util.h"
#include "kspread_doc.h"
#include "kspread_table.h"

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>

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

  // Accept both: dates and strings
  if (!KSUtil::checkType( context, args[0], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[0], KSValue::DateType, true ))
      return false;

    date1 = args[0]->dateValue();
  }
  else
    date1 = KGlobal::locale()->readDate(args[0]->stringValue());

  if (!KSUtil::checkType( context, args[1], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[1], KSValue::DateType, true ))
      return false;

    date2 = args[1]->dateValue();
  }
  else
    date2 = KGlobal::locale()->readDate(args[1]->stringValue());

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

  // Accept both: dates and strings
  if (!KSUtil::checkType( context, args[0], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[0], KSValue::DateType, true ))
      return false;

    date1 = args[0]->dateValue();
  }
  else
    date1 = KGlobal::locale()->readDate(args[0]->stringValue());

  if (!KSUtil::checkType( context, args[1], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[1], KSValue::DateType, true ))
      return false;

    date2 = args[1]->dateValue();
  }
  else
    date2 = KGlobal::locale()->readDate(args[1]->stringValue());


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

  if (!KSUtil::checkType( context, args[0], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[0], KSValue::DateType, true ))
      return false;

    date1 = args[0]->dateValue();
  }
  else
    date1 = KGlobal::locale()->readDate(args[0]->stringValue());

  if (!KSUtil::checkType( context, args[1], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[1], KSValue::DateType, true ))
      return false;

    date2 = args[1]->dateValue();
  }
  else
    date2 = KGlobal::locale()->readDate(args[1]->stringValue());


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

  if (!KSUtil::checkType( context, args[0], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[0], KSValue::DateType, true ))
      return false;

    date1 = args[0]->dateValue();
  }
  else
    date1 = KGlobal::locale()->readDate(args[0]->stringValue());


  if (!KSUtil::checkType( context, args[1], KSValue::StringType, true ))
  {
    if (!KSUtil::checkType( context, args[1], KSValue::DateType, true ))
      return false;

    date2 = args[1]->dateValue();
  }
  else
    date2 = KGlobal::locale()->readDate(args[1]->stringValue());


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
  if ( !KSUtil::checkArgumentsCount( context,1, "hours",true ) )
    return false;
  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;
      QTime tmp=KGlobal::locale()->readTime(args[0]->stringValue());
      if(tmp.isValid())
	context.setValue(new KSValue(tmp.hour()));
      else
	context.setValue(new KSValue(i18n("Err")));
      return true;
    }
  else
    {
      QTime tmp=args[0]->timeValue();
      context.setValue(new KSValue(tmp.hour()));
    }
   return true;
}

// Function: minutes
bool kspreadfunc_minutes( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "minutes",true ) )
    return false;
  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;
      QTime tmp=KGlobal::locale()->readTime(args[0]->stringValue());
      if(tmp.isValid())
	context.setValue(new KSValue(tmp.minute()));
      else
	context.setValue(new KSValue(i18n("Err")));
      return true;
    }
  else
    {
      QTime tmp=args[0]->timeValue();
      context.setValue(new KSValue(tmp.minute()));
    }
   return true;
}

// Function: seconds
bool kspreadfunc_seconds( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "seconds",true ) )
    return false;
  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;
      QTime tmp=KGlobal::locale()->readTime(args[0]->stringValue());
      if(tmp.isValid())
	context.setValue(new KSValue(tmp.second()));
      else
	context.setValue(new KSValue(i18n("Err")));
      return true;
    }
  else
    {
      QTime tmp=args[0]->timeValue();
      context.setValue(new KSValue(tmp.second()));
    }
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
    context.setValue( new KSValue(QString(i18n("Err"))) );

  return true;
}

// Function: day
bool kspreadfunc_day( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString tmp;
  if ( !KSUtil::checkArgumentsCount( context,1, "day",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if(KGlobal::locale()->weekDayName(args[0]->intValue()).isNull())
        tmp=i18n("Err");
  else
        tmp= KGlobal::locale()->weekDayName(args[0]->intValue());

  //context.setValue( new KSValue(KGlobal::locale()->weekDayName(args[0]->intValue())));
  context.setValue( new KSValue(tmp));
  return true;
}

// Function: month
bool kspreadfunc_month( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString tmp;
  if ( !KSUtil::checkArgumentsCount( context,1, "month",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  if(KGlobal::locale()->monthName(args[0]->intValue()).isNull())
        tmp=i18n("Err");
  else
        tmp=KGlobal::locale()->monthName(args[0]->intValue());

  context.setValue( new KSValue(tmp));
  //context.setValue( new KSValue(KGlobal::locale()->monthName(args[0]->intValue())));

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
