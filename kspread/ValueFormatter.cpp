/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright (C) 1998-2004 KSpread Team <koffice-devel@kde.org>

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

#include "ValueFormatter.h"

#include "Cell.h"
#include "Locale.h"
#include "Util.h"
#include "ValueConverter.h"

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <klocale.h>

#include <float.h>
#include <math.h>

using namespace KSpread;

ValueFormatter::ValueFormatter (ValueConverter *conv) : converter( conv )
{
}

QString ValueFormatter::formatText (Cell *cell, FormatType fmtType)
{
  if (cell->hasError ())
    return errorFormat (cell);

  QString str;

  Style::FloatFormat floatFormat =
      cell->format()->floatFormat (cell->column(), cell->row());
  int precision = cell->format()->precision (cell->column(), cell->row());
  QString prefix = cell->format()->prefix (cell->column(), cell->row());
  QString postfix = cell->format()->postfix (cell->column(), cell->row());
  Style::Currency currency;
  bool valid = cell->format()->currencyInfo(currency);
  QString currencySymbol = valid ? currency.symbol : QString::null;

  return formatText (cell->value(), fmtType, precision,
      floatFormat, prefix, postfix, currencySymbol);
}

QString ValueFormatter::formatText (const Value &value,
    FormatType fmtType, int precision, Style::FloatFormat floatFormat,
    const QString &prefix, const QString &postfix, const QString &currencySymbol)
{
  //if we have an array, use its first element
  if (value.isArray())
    return formatText (value.element (0, 0), fmtType, precision,
        floatFormat, prefix, postfix, currencySymbol);

  QString str;

  //step 1: determine formatting that will be used
  fmtType = determineFormatting (value, fmtType);

  //step 2: format the value !

  //text
  if (fmtType == Text_format)
  {
    str = converter->asString (value).asString();
    if (!str.isEmpty() && str[0]=='\'' )
      str = str.mid(1);
  }

  //date
  else if (formatIsDate (fmtType))
    str = dateFormat (value.asDate(), fmtType);

  //time
  else if (formatIsTime (fmtType))
    str = timeFormat (value.asDateTime(), fmtType);

  //fraction
  else if (formatIsFraction (fmtType))
    str = fractionFormat (value.asFloat(), fmtType);

  //another
  else
  {
    //some cell parameters ...
    double v = converter->asFloat (value).asFloat();

    // Always unsigned ?
    if ((floatFormat == Style::AlwaysUnsigned) && (v < 0.0))
      v *= -1.0;

    // Make a string out of it.
    str = createNumberFormat (v, precision, fmtType,
        (floatFormat == Style::AlwaysSigned), currencySymbol);

    // Remove trailing zeros and the decimal point if necessary
    // unless the number has no decimal point
    if (precision == -1)
    {
      QChar decimal_point = converter->locale()->decimalSymbol()[0];
      if ( decimal_point.isNull() )
        decimal_point = '.';

      removeTrailingZeros (str, decimal_point);
    }
  }

  if (!prefix.isEmpty())
    str = prefix + ' ' + str;

  if( !postfix.isEmpty())
    str += ' ' + postfix;

  //kDebug() << "ValueFormatter says: " << str << endl;
  return str;
}

FormatType ValueFormatter::determineFormatting (const Value &value,
    FormatType fmtType)
{
  //if the cell value is a string, then we want to display it as-is,
  //no matter what, same if the cell is empty
  if (value.isString () || (value.format() == Value::fmt_None))
    return Text_format;
  //same if we're supposed to display string, no matter what we actually got
  if (fmtType == Text_format)
    return Text_format;

  //now, everything depends on whether the formatting is Generic or not
  if (fmtType == Generic_format)
  {
    //here we decide based on value's format...
    Value::Format fmt = value.format();
    switch (fmt) {
      case Value::fmt_None:
        fmtType = Text_format;
      break;
      case Value::fmt_Boolean:
        fmtType = Text_format;
      break;
      case Value::fmt_Number:
        if (value.asFloat() > 1e+10)
          fmtType = Scientific_format;
        else
          fmtType = Number_format;
      break;
      case Value::fmt_Percent:
        fmtType = Percentage_format;
      break;
      case Value::fmt_Money:
        fmtType = Money_format;
      break;
      case Value::fmt_DateTime:
        fmtType = TextDate_format;
      break;
      case Value::fmt_Date:
        fmtType = ShortDate_format;
      break;
      case Value::fmt_Time:
        fmtType = Time_format;
      break;
      case Value::fmt_String:
        //this should never happen
        fmtType = Text_format;
      break;
    };
    return fmtType;
  }
  else
  {
    //we'll mostly want to use the given formatting, the only exception
    //being Boolean values

    //TODO: is this correct? We may also want to convert bools to 1s and 0s
    //if we want to display a number...

    //TODO: what to do about Custom formatting? We don't support it as of now,
    //  but we'll have it ... one day, that is ...
    if (value.isBoolean())
      return Text_format;
    else
      return fmtType;
  }
}


void ValueFormatter::removeTrailingZeros (QString &str, QChar decimal_point)
{
  if (str.indexOf(decimal_point) < 0)
    //no decimal point -> nothing to do
    return;

  int start = 0;
  int cslen = converter->locale()->currencySymbol().length();
  if (str.indexOf('%') != -1)
    start = 2;
  else if (str.indexOf(converter->locale()->currencySymbol()) ==
      ((int) (str.length() - cslen)))
    start = cslen + 1;
  else if ((start = str.indexOf('E')) != -1)
    start = str.length() - start;
  else
    start = 0;

  int i = str.length() - start;
  bool bFinished = false;
  while ( !bFinished && i > 0 )
  {
    QChar ch = str[i - 1];
    if (ch == '0')
      str.remove (--i,1);
    else
    {
      bFinished = true;
      if (ch == decimal_point)
        str.remove (--i, 1);
    }
  }
}

QString ValueFormatter::createNumberFormat ( double value, int precision,
    FormatType fmt, bool alwaysSigned, const QString& currencySymbol)
{
  // if precision is -1, ask for a huge number of decimals, we'll remove
  // the zeros later. Is 8 ok ?
  // Stefan: No. Use maximum possible decimal precision (DBL_DIG) instead.
  int p = (precision == -1) ? 8 : precision;
  QString localizedNumber;
  int pos = 0;

  //multiply value by 100 for percentage format
  if (fmt == Percentage_format)
    value *= 100;

  // this will avoid displaying negative zero, i.e "-0.0000"
  if( fabs( value ) < DBL_EPSILON ) value = 0.0;

  // round the number, based on desired precision if not scientific is chosen
  //(scientific has relative precision)
  if( fmt != Scientific_format )
  {
    double m[] = { 1, 10, 100, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10 };
    double mm = (p > 10) ? pow(10.0,p) : m[p];
    bool neg = value < 0;
    value = floor( fabs(value)*mm + 0.5 ) / mm;
    if( neg ) value = -value;
  }

  QChar decimal_point;
  switch (fmt)
  {
    case Number_format:
      localizedNumber = converter->locale()->formatNumber(value, p);
      break;
    case Percentage_format:
      localizedNumber = converter->locale()->formatNumber (value, p)+ " %";
      break;
    case Money_format:
      localizedNumber = converter->locale()->formatMoney (value,
        currencySymbol.isEmpty() ? converter->locale()->currencySymbol() : currencySymbol, p );
      break;
    case Scientific_format:
      decimal_point = converter->locale()->decimalSymbol()[0];
      localizedNumber = QString::number (value, 'E', p);
      if ((pos = localizedNumber.indexOf('.')) != -1)
        localizedNumber = localizedNumber.replace (pos, 1, decimal_point);
      break;
    default :
      //other formatting?
      // This happens with Custom_format...
      kDebug(36001)<<"Wrong usage of ValueFormatter::createNumberFormat fmt=" << fmt << "\n";
      break;
  }

  //prepend positive sign if needed
  if (alwaysSigned && value >= 0 )
    if (converter->locale()->positiveSign().isEmpty())
      localizedNumber='+'+localizedNumber;

  return localizedNumber;
}

QString ValueFormatter::fractionFormat (double value, FormatType fmtType)
{
  double result = value - floor(value);
  int index;
  int limit = 0;

  /* return w/o fraction part if not necessary */
  if (result == 0)
    return QString::number(value);

  switch (fmtType) {
  case fraction_half:
    index = 2;
    break;
  case fraction_quarter:
    index = 4;
    break;
  case fraction_eighth:
    index = 8;
    break;
  case fraction_sixteenth:
    index = 16;
    break;
  case fraction_tenth:
    index = 10;
    break;
  case fraction_hundredth:
    index = 100;
    break;
  case fraction_one_digit:
    index = 3;
    limit = 9;
    break;
  case fraction_two_digits:
    index = 4;
    limit = 99;
    break;
  case fraction_three_digits:
    index = 5;
    limit = 999;
    break;
  default:
    kDebug(36001) << "Error in Fraction format\n";
    return QString::number(value);
    break;
  } /* switch */


  /* handle halves, quarters, tenths, ... */

  if (fmtType != fraction_three_digits
    && fmtType != fraction_two_digits
    && fmtType != fraction_one_digit) {
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

QString ValueFormatter::timeFormat (const QDateTime &dt, FormatType fmtType)
{
  if (fmtType == Time_format)
    return converter->locale()->formatTime(dt.time(), false);

  if (fmtType == SecondeTime_format)
    return converter->locale()->formatTime(dt.time(), true);

  int h = dt.time().hour();
  int m = dt.time().minute();
  int s = dt.time().second();

  QString hour = ( h < 10 ? '0' + QString::number(h) : QString::number(h) );
  QString minute = ( m < 10 ? '0' + QString::number(m) : QString::number(m) );
  QString second = ( s < 10 ? '0' + QString::number(s) : QString::number(s) );
  bool pm = (h > 12);
  QString AMPM( pm ? i18n("PM"):i18n("AM") );

  if (fmtType == Time_format1) {  // 9 : 01 AM
    return QString("%1:%2 %3")
      .arg((pm ? h - 12 : h),2)
      .arg(minute,2)
      .arg(AMPM);
  }

  if (fmtType == Time_format2) {  //9:01:05 AM
    return QString("%1:%2:%3 %4")
      .arg((pm ? h-12 : h),2)
      .arg(minute,2)
      .arg(second,2)
      .arg(AMPM);
  }

  if (fmtType == Time_format3) {
    return QString("%1 %2 %3 %4 %5 %6")      // 9 h 01 min 28 s
      .arg(hour,2)
      .arg(i18n("h"))
      .arg(minute,2)
      .arg(i18n("min"))
      .arg(second,2)
      .arg(i18n("s"));
  }

  if (fmtType == Time_format4) {  // 9:01
    return QString("%1:%2").arg(hour, 2).arg(minute, 2);
  }

  if (fmtType == Time_format5) {  // 9:01:12
    return QString("%1:%2:%3").arg(hour, 2).arg(minute, 2).arg(second, 2);
  }

  QDate d1(dt.date());
  QDate d2( 1899, 12, 31 );
  int d = d2.daysTo( d1 ) + 1;

  h += d * 24;

  if (fmtType == Time_format6)
  {  // [mm]:ss
    m += (h * 60);
    return QString("%1:%2").arg(m, 1).arg(second, 2);
  }
  if (fmtType == Time_format7) {  // [h]:mm:ss
    return QString("%1:%2:%3").arg(h, 1).arg(minute, 2).arg(second, 2);
  }
  if (fmtType == Time_format8)
  {  // [h]:mm
    m += (h * 60);
    return QString("%1:%2").arg(h, 1).arg(minute, 2);
  }

  return converter->locale()->formatTime( dt.time(), false );
}

QString ValueFormatter::dateFormat (const QDate &date, FormatType fmtType)
{
  QString tmp;
  if (fmtType == ShortDate_format) {
    tmp = converter->locale()->formatDate(date, true);
  }
  else if (fmtType == TextDate_format) {
    tmp = converter->locale()->formatDate(date, false);
  }
  else if (fmtType == date_format1) {  /*18-Feb-99 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format2) {  /*18-Feb-1999 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year());
  }
  else if (fmtType == date_format3) {  /*18-Feb */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + converter->locale()->calendar()->monthString(date, true);
  }
  else if (fmtType == date_format4) {  /*18-05 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + QString().sprintf("%02d", date.month() );
  }
  else if (fmtType == date_format5) {  /*18/05/00 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '/' + QString().sprintf("%02d", date.month()) + '/';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format6) {  /*18/05/1999 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '/' + QString().sprintf("%02d", date.month()) + '/';
    tmp += QString::number(date.year());
  }
  else if (fmtType == date_format7) {  /*Feb-99 */
    tmp = converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format8) {  /*February-99 */
    tmp = converter->locale()->calendar()->monthString(date, false) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format9) {  /*February-1999 */
    tmp = converter->locale()->calendar()->monthString(date, false) + '-';
    tmp += QString::number(date.year());
  }
  else if (fmtType == date_format10) {  /*F-99 */
    tmp = converter->locale()->calendar()->monthString(date, false).at(0) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format11) {  /*18/Feb */
    tmp = QString().sprintf("%02d", date.day()) + '/';
    tmp += converter->locale()->calendar()->monthString(date, true);
  }
  else if (fmtType == date_format12) {  /*18/02 */
    tmp = QString().sprintf("%02d", date.day()) + '/';
    tmp += QString().sprintf("%02d", date.month());
  }
  else if (fmtType == date_format13) {  /*18/Feb/1999 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '/' + converter->locale()->calendar()->monthString(date, true) + '/';
    tmp += QString::number(date.year());
  }
  else if (fmtType == date_format14) {  /*2000/Feb/18 */
    tmp = QString::number(date.year());
    tmp += '/' + converter->locale()->calendar()->monthString(date, true) + '/';
    tmp += QString().sprintf("%02d", date.day());
  }
  else if (fmtType == date_format15) {  /*2000-Feb-18 */
    tmp = QString::number(date.year());
    tmp += '-' + converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString().sprintf("%02d", date.day());
  }
  else if (fmtType == date_format16) {  /*2000-02-18 */
    tmp = QString::number(date.year());
    tmp += '-' + QString().sprintf("%02d", date.month()) + '-';
    tmp += QString().sprintf("%02d", date.day());
  }
  else if (fmtType == date_format17) {  /*2 february 2000 */
    tmp = QString().sprintf("%d", date.day());
    tmp += ' ' + converter->locale()->calendar()->monthString(date, false) + ' ';
    tmp += QString::number(date.year());
  }
  else if (fmtType == date_format18) {  /*02/18/1999 */
    tmp = QString().sprintf("%02d", date.month());
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year());
  }
  else if (fmtType == date_format19) {  /*02/18/99 */
    tmp = QString().sprintf("%02d", date.month());
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format20) {  /*Feb/18/99 */
    tmp = converter->locale()->calendar()->monthString(date, true);
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format21) {  /*Feb/18/1999 */
    tmp = converter->locale()->calendar()->monthString(date, true);
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year());
  }
  else if (fmtType == date_format22) {  /*Feb-1999 */
    tmp = converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year());
  }
  else if (fmtType == date_format23) {  /*1999 */
    tmp = QString::number(date.year());
  }
  else if (fmtType == date_format24) {  /*99 */
    tmp = QString::number(date.year()).right(2);
  }
  else if (fmtType == date_format25) {  /*2000/02/18 */
    tmp = QString::number(date.year());
    tmp += '/' + QString().sprintf("%02d", date.month());
    tmp += '/' + QString().sprintf("%02d", date.day());
  }
  else if (fmtType == date_format26) {  /*2000/Feb/18 */
    tmp = QString::number(date.year());
    tmp += '/' + converter->locale()->calendar()->monthString(date, true);
    tmp += '/' + QString().sprintf("%02d", date.day());
  }
  else
    tmp = converter->locale()->formatDate(date, true);

  // Missing compared with gnumeric:
  //  "m/d/yy h:mm",    /* 20 */
  //  "m/d/yyyy h:mm",  /* 21 */
  //  "mmm/ddd/yy",    /* 12 */
  //  "mmm/ddd/yyyy",    /* 13 */
  //  "mm/ddd/yy",    /* 14 */
  //  "mm/ddd/yyyy",    /* 15 */

  return tmp;
}

QString ValueFormatter::errorFormat (Cell *cell)
{
  QString err;
  if (cell->testFlag (Cell::Flag_ParseError))
    err = '#' + i18n ("Parse") + '!';
  else if ( cell->testFlag (Cell::Flag_CircularCalculation))
    err = '#' + i18n ("Circle") + '!';
  else if ( cell->testFlag (Cell::Flag_DependencyError))
    err = '#' + i18n ("Depend") + '!';
  else
  {
    err = "####";
    kDebug(36001) << "Unhandled error type." << endl;
  }
  return err;
}

