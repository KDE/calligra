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

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <kspread_doc.h>
#include <kspread_functions.h>
#include "kspread_functions_helper.h"
#include <kspread_table.h>
#include <kspread_util.h>


// copied from gnumeric: src/format.c:
static const int g_dateSerial_19000228 = 59;
/* One less that the Julian day number of 19000101.  */
static int g_dateOrigin = 0;

uint EDate::greg2jul( int y, int m, int d )
{
  return QDate::gregorianToJulian( y, m, d ) - g_dateOrigin;
}

uint EDate::greg2jul( QDate const & date )
{
  // reference is 31 Dec, 1899 midnight
  QDate refDate = QDate( 1899, 12, 31 );
  
  return refDate.daysTo( date ) + 1;
}

void EDate::jul2greg( double num, int & y, int & m, int & d )
{
  kdDebug() << "Here" << endl;
  
  QDate date = QDate( 1899, 12, 31 );
  
  date = date.addDays( (int) num );
  
  y = date.year();
  m = date.month();
  d = date.day();
  
  return;
  
  if ( g_dateOrigin == 0 )
    g_dateOrigin = EDate::greg2jul( 1900, 1, 1 ) - 1;
  
  int i = (int) floor( num + HALFSEC );
  if (i > g_dateSerial_19000228)
    --i;
  else if (i == g_dateSerial_19000228 + 1)
    kdWarning() << "Request for date 02/29/1900." << endl;
  
  kdDebug() << "num: " << num << ", i: " << i << " - " << i + g_dateOrigin << endl;

  QDate::julianToGregorian( i + g_dateOrigin, y, m, d );
}



bool getDate( KSContext & context, KSValue::Ptr & arg, QDate & date )
{
  kdDebug() << "Here" << endl;
  if ( !KSUtil::checkType( context, arg, KSValue::DateType, true ) )
  {
    kdDebug() << "Here1" << endl;
    if ( !KSUtil::checkType( context, arg, KSValue::StringType, true ) )
    {
      kdDebug() << "Here2" << endl;
      if ( !KSUtil::checkType( context, arg, KSValue::DoubleType, true ) )
        return false;

      kdDebug() << "Here3" << endl;
      double d = arg->doubleValue();

      int y = 0; 
      int m = 0; 
      int day = 0;
      kdDebug() << "D: " << d << endl;
      EDate::jul2greg( d, y, m, day );
      kdDebug() << "Alive" << endl;
      date.setYMD( y, m, day );
      kdDebug() << "Alive2" << endl;

      return true;
    }
    else
    {
      kdDebug() << "Here4" << endl;
      QString s = arg->stringValue();
      bool valid = false;
      kdDebug() << "S: " << s << endl;
      date = KGlobal::locale()->readDate( s, &valid );
      if ( !valid )
        return false;

      kdDebug() << "Here end s" << endl;
      return true;
    }
  }
  else 
  {
    kdDebug() << "Here5" << endl;
    date = arg->dateValue();
    return true;
  }

  return false;
}

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
  kdDebug() << "Subtract: " << months << endl;
  
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

int daysPerYear(QDate const & date, int basis)
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

int daysBetweenDates(QDate const & date1, QDate const & date2, int basis)
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

  kdDebug() << "day1: " << day1 << ", day2: " << day2 << endl;

  years  = year2  - year1;
  months = month2 - month1 + years * 12;
  days   = day2   - day1;

  kdDebug() << "Months: " << months << ", Days: " << days << ", Basis: " << basis << endl;

  isLeapYear = QDate::leapYear( year1 );

  switch (basis) 
  {
   case 0:
    kdDebug() << "D: " << months * 30 + days << endl;

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


double fact(int n)
{
  return ( n != 0 ? n * fact(n - 1) : 1 );
}

double combin(int n, int k)
{
  if (n >= 15) 
  {
    double result = exp(lgamma (n + 1) - lgamma (k + 1) - lgamma (n - k + 1));
    return floor(result + 0.5);
  } 
  else 
  {
    double result = fact( n ) / fact( k ) / fact( n - k );
    return result;
  }
}

double gaussinv_helper (double x) 
{
  double c0, c1, c2, d1, d2, d3, q, t, z;
  c0 = 2.515517;
  c1 = 0.802853;
  c2 = 0.010328;
  d1 = 1.432788;
  d2 = 0.189269;
  d3 = 0.001308;
  if (x < 0.5)
    q = x;
  else
    q = 1.0-x;
  t = sqrt(-log(q*q));
  z = t - (c0 + t*(c1 + t*c2)) / (1.0 + t*(d1 + t*(d2 + t*d3)));
  if (x < 0.5)
    z *= -1.0;
  return z;
}

bool approx_equal(double a, double b)
{
  if ( a == b )
    return TRUE;
  double x = a - b;
  return (x < 0.0 ? -x : x)  <  ((a < 0.0 ? -a : a) * DBL_EPSILON);
}

bool kspreadfunc_average_helper( KSContext & context, QValueList<KSValue::Ptr> & args, 
                                 double & result, int & number, bool aMode )
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_average_helper( context, (*it)->listValue(), result, number, aMode ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += (*it)->doubleValue();
      ++number;
    }
    else if ( aMode )
    {
      kdDebug() << "'A' - Mode" << endl;
      if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      {
        kdDebug() << "String: " << (*it)->stringValue() << endl;
        ++number;
      }
      else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      {
        kdDebug() << "Bool: " << (*it)->boolValue() << endl;
        result += ( (*it)->boolValue() ? 1.0 : 0.0 );
        ++number;
      }
    }
  }

  return true;
}

bool kspreadfunc_stddev_helper( KSContext & context, QValueList<KSValue::Ptr> & args, 
                                double & result, double & avera, bool aMode )
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_stddev_helper( context, (*it)->listValue(), result, avera, aMode ) )
        return false;

    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += ( ( (*it)->doubleValue() - avera ) * ( (*it)->doubleValue() - avera ) );
    }
    else if ( aMode )
    {
      if ( KSUtil::checkType( context, *it, KSValue::StringType, false ) )
      {
        result += ( ( 0.0 - avera ) * ( 0.0 - avera ) );
      }
      else if ( KSUtil::checkType( context, *it, KSValue::BoolType, false ) )
      {
        double a = ( (*it)->boolValue() ? 1.0 : 0.0 );
        result += ( ( a - avera ) * ( a - avera ) );
      }
    }
  }

  return true;
}

bool kspreadfunc_variance_helper( KSContext & context, QValueList<KSValue::Ptr> & args, 
                                  double & result, double avera, bool aMode )
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_variance_helper( context, (*it)->listValue(), result, avera, aMode ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += ( (*it)->doubleValue() - avera ) * ( (*it)->doubleValue() - avera );
    }
    else if ( aMode )
    {
      if ( KSUtil::checkType( context, *it, KSValue::StringType, false ) )
      {
        result += ( ( 0.0 - avera ) * ( 0.0 - avera ) );
      }
      else if ( KSUtil::checkType( context, *it, KSValue::BoolType, false ) )
      {
        double a = ( (*it)->boolValue() ? 1.0 : 0.0 );
        result += ( ( a - avera ) * ( a - avera ) );
      }
    }
  }

  return true;
}

