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
#include <klocale.h>

#include "kspread_functions.h"
#include "kspread_functions_helper.h"
#include "kspread_util.h"


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

  QDate::julianToGregorian( i + g_dateOrigin, y, m, d );
}



bool getDate( KSContext & context, KSValue::Ptr & arg, QDate & date )
{
  if ( !KSUtil::checkType( context, arg, KSValue::DateType, true ) )
  {
    if ( !KSUtil::checkType( context, arg, KSValue::StringType, true ) )
    {
      if ( !KSUtil::checkType( context, arg, KSValue::DoubleType, true ) )
        return false;

      double d = arg->doubleValue();

      int y = 0;
      int m = 0;
      int day = 0;
      EDate::jul2greg( d, y, m, day );

      date.setYMD( y, m, day );

      return true;
    }
    else
    {
      QString s = arg->stringValue();
      bool valid = false;

      date = KGlobal::locale()->readDate( s, &valid );
      if ( !valid )
        return false;

      return true;
    }
  }
  else
  {
    date = arg->dateValue();
    return true;
  }

  return false;
}

bool getTime( KSContext & context, KSValue::Ptr & arg, QTime & time )
{
  if ( !KSUtil::checkType( context, arg, KSValue::TimeType, true ) )
  {
    if ( !KSUtil::checkType( context, arg, KSValue::StringType, true ) )
    {
      if ( !KSUtil::checkType( context, arg, KSValue::DoubleType, true ) )
        return false;

      double d = arg->doubleValue();

      KSpreadValue v(d);
      time = v.asDateTime().time();

      return true;
    }
    else
    {
      QString s = arg->stringValue();
      bool valid = false;

      time = KGlobal::locale()->readTime( s, &valid );
      if ( !valid )
        return false;

      return true;
    }
  }
  else
  {
    time = arg->timeValue();
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
  double q,t,z;

  q=x-0.5;

  if(fabs(q)<=.425)
  {
    t=0.180625-q*q;

    z=
    q*
    (
      (
        (
          (
            (
              (
                (
                  t*2509.0809287301226727+33430.575583588128105
                )
                *t+67265.770927008700853
              )
              *t+45921.953931549871457
            )
            *t+13731.693765509461125
          )
          *t+1971.5909503065514427
        )
        *t+133.14166789178437745
      )
      *t+3.387132872796366608
    )
    /
    (
      (
        (
          (
            (
              (
                (
                  t*5226.495278852854561+28729.085735721942674
                )
                *t+39307.89580009271061
              )
              *t+21213.794301586595867
            )
            *t+5394.1960214247511077
          )
          *t+687.1870074920579083
        )
        *t+42.313330701600911252
      )
      *t+1.0
    );

  }
  else
  {
    if(q>0)  t=1-x;
    else    t=x;

    t=sqrt(-log(t));

    if(t<=5.0)
    {
      t+=-1.6;

      z=
      (
        (
          (
            (
              (
                (
                  (
                    t*7.7454501427834140764e-4+0.0227238449892691845833
                  )
                  *t+0.24178072517745061177
                )
                *t+1.27045825245236838258
              )
              *t+3.64784832476320460504
            )
            *t+5.7694972214606914055
          )
          *t+4.6303378461565452959
        )
        *t+1.42343711074968357734
      )
      /
      (
        (
          (
            (
              (
                (
                  (
                    t*1.05075007164441684324e-9+5.475938084995344946e-4
                  )
                  *t+0.0151986665636164571966
                )
                *t+0.14810397642748007459
              )
              *t+0.68976733498510000455
            )
            *t+1.6763848301838038494
          )
          *t+2.05319162663775882187
        )
        *t+1.0
      );

    }
    else
    {
      t+=-5.0;

      z=
      (
        (
          (
            (
              (
                (
                  (
                    t*2.01033439929228813265e-7+2.71155556874348757815e-5
                  )
                  *t+0.0012426609473880784386
                )
                *t+0.026532189526576123093
              )
              *t+0.29656057182850489123
            )
            *t+1.7848265399172913358
          )
          *t+5.4637849111641143699
        )
        *t+6.6579046435011037772
      )
      /
      (
        (
          (
            (
              (
                (
                  (
                    t*2.04426310338993978564e-15+1.4215117583164458887e-7
                  )
                  *t+1.8463183175100546818e-5
                )
                *t+7.868691311456132591e-4
              )
              *t+0.0148753612908506148525
            )
            *t+0.13692988092273580531
          )
          *t+0.59983220655588793769
        )
        *t+1.0
      );

    }

    if(q<0.0) z=-z;
  }

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
      if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
      {
        ++number;
      }
      else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      {
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

void getCond( KSpreadDB::Condition & cond, QString text )
{
  cond.comp = KSpreadDB::isEqual;
  text = text.stripWhiteSpace();

  if ( text.startsWith( "<=" ) )
  {
    cond.comp = KSpreadDB::lessEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( ">=" ) )
  {
    cond.comp = KSpreadDB::greaterEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "!=" ) || text.startsWith( "<>" ) )
  {
    cond.comp = KSpreadDB::notEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "==" ) )
  {
    cond.comp = KSpreadDB::isEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "<" ) )
  {
    cond.comp = KSpreadDB::isLess;
    text = text.remove( 0, 1 );
  }
  else if ( text.startsWith( ">" ) )
  {
    cond.comp = KSpreadDB::isGreater;
    text = text.remove( 0, 1 );
  }
  else if ( text.startsWith( "=" ) )
  {
    cond.comp = KSpreadDB::isEqual;
    text = text.remove( 0, 1 );
  }

  text = text.stripWhiteSpace();

  bool ok = false;
  double d = text.toDouble( &ok );
  if ( ok )
  {
    cond.type = KSpreadDB::numeric;
    cond.value = d;
    kdDebug() << "Numeric: " << d << ", Op: " << cond.comp << endl;
  }
  else
  {
    cond.type = KSpreadDB::string;
    cond.stringValue = text;
    kdDebug() << "String: " << text << ", Op: " << cond.comp << endl;
  }
}

bool conditionMatches( KSpreadDB::Condition &cond, const double &d )
{
  kdDebug() << "Comparing: " << d << " - " << cond.value << "; Comp: " << cond.comp << endl;

  switch ( cond.comp )
  {
    case KSpreadDB::isEqual:
    if ( approx_equal( d, cond.value ) )
      return true;

    return false;

    case KSpreadDB::isLess:
    if ( d < cond.value )
      return true;

    return false;

    case KSpreadDB::isGreater:
    if ( d > cond.value )
      return true;

    return false;

    case KSpreadDB::lessEqual:
    if ( d <= cond.value )
      return true;

    return false;

    case KSpreadDB::greaterEqual:
    if ( d >= cond.value )
      return true;

    return false;

    case KSpreadDB::notEqual:
    if ( d != cond.value )
      return true;

    return false;

    default:
    return false;
  }
}

bool conditionMatches( KSpreadDB::Condition &cond, const QString &d )
{
  kdDebug() << "String: " << d << endl;

  switch ( cond.comp )
  {
    case KSpreadDB::isEqual:
    if ( d == cond.stringValue )
      return true;

    return false;

    case KSpreadDB::isLess:
    if ( d < cond.stringValue )
      return true;

    return false;

    case KSpreadDB::isGreater:
    if ( d > cond.stringValue )
      return true;

    return false;

    case KSpreadDB::lessEqual:
    if ( d <= cond.stringValue )
      return true;

    return false;

    case KSpreadDB::greaterEqual:
    if ( d >= cond.stringValue )
      return true;

    return false;

    case KSpreadDB::notEqual:
    if ( d != cond.stringValue )
      return true;

    return false;

    default:
    return false;
  }

  return true;
}
