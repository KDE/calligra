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
   Boston, MA 02110-1301, USA.
*/

#include "ValueFormatter.h"

#include "Cell.h"
#include "Doc.h"
#include "Localization.h"
#include "ValueConverter.h"

#include <kcalendarsystem.h>
#include <kdebug.h>
#include <klocale.h>

#include <float.h>
#include <math.h>

using namespace KSpread;

ValueFormatter::ValueFormatter( const ValueConverter* converter )
    : m_converter( converter )
{
}

const Doc* ValueFormatter::doc() const
{
    return m_converter->doc();
}

QString ValueFormatter::formatText (const Cell *cell, Format::Type fmtType)
{
  if ( cell->value().isError() )
    return cell->value().errorMessage();

  QString str;

  Style::FloatFormat floatFormat = cell->style().floatFormat();
  int precision = cell->style().precision();
  QString prefix = cell->style().prefix();
  QString postfix = cell->style().postfix();
  Currency currency = cell->style().currency();

  return formatText (cell->value(), fmtType, precision,
      floatFormat, prefix, postfix, currency.symbol());
}

QString ValueFormatter::formatText (const Value &value,
    Format::Type fmtType, int precision, Style::FloatFormat floatFormat,
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
  if (fmtType == Format::Text)
  {
    str = m_converter->asString (value).asString();
    if (!str.isEmpty() && str[0]=='\'' )
      str = str.mid(1);
  }

  //date
  else if (Format::isDate (fmtType))
    str = dateFormat (value.asDate( doc() ), fmtType);

  //time
  else if (Format::isTime (fmtType))
    str = timeFormat (value.asDateTime( doc() ), fmtType);

  //fraction
  else if (Format::isFraction (fmtType))
    str = fractionFormat (value.asFloat(), fmtType);

  //another
  else
  {
    // complex
    if ( value.isComplex() )
        str = complexFormat( value, precision, fmtType, floatFormat, currencySymbol );

    // real number
    else
        str = createNumberFormat( m_converter->asFloat( value ).asFloat(),
                                  precision, fmtType, floatFormat, currencySymbol );
  }

  if (!prefix.isEmpty())
    str = prefix + ' ' + str;

  if( !postfix.isEmpty())
    str += ' ' + postfix;

  //kDebug() << "ValueFormatter says: " << str << endl;
  return str;
}

Format::Type ValueFormatter::determineFormatting (const Value &value,
    Format::Type fmtType)
{
  //if the cell value is a string, then we want to display it as-is,
  //no matter what, same if the cell is empty
  if (value.isString () || (value.format() == Value::fmt_None))
    return Format::Text;
  //same if we're supposed to display string, no matter what we actually got
  if (fmtType == Format::Text)
    return Format::Text;

  //now, everything depends on whether the formatting is Generic or not
  if (fmtType == Format::Generic)
  {
    //here we decide based on value's format...
    Value::Format fmt = value.format();
    switch (fmt) {
      case Value::fmt_None:
        fmtType = Format::Text;
      break;
      case Value::fmt_Boolean:
        fmtType = Format::Text;
      break;
      case Value::fmt_Number:
        if (value.asFloat() > 1e+10)
          fmtType = Format::Scientific;
        else
          fmtType = Format::Number;
      break;
      case Value::fmt_Percent:
        fmtType = Format::Percentage;
      break;
      case Value::fmt_Money:
        fmtType = Format::Money;
      break;
      case Value::fmt_DateTime:
        fmtType = Format::TextDate;
      break;
      case Value::fmt_Date:
        fmtType = Format::ShortDate;
      break;
      case Value::fmt_Time:
        fmtType = Format::Time;
      break;
      case Value::fmt_String:
        //this should never happen
        fmtType = Format::Text;
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
      return Format::Text;
    else
      return fmtType;
  }
}


QString ValueFormatter::removeTrailingZeros( const QString& str, const QString& decimalSymbol )
{
  if ( !str.contains( decimalSymbol ) )
    //no decimal symbol -> nothing to do
    return str;

  int start = 0;
  int cslen = m_converter->locale()->currencySymbol().length();
  if (str.indexOf('%') != -1)
    start = 2;
  else if (str.indexOf(m_converter->locale()->currencySymbol()) ==
      ((int) (str.length() - cslen)))
    start = cslen + 1;
  else if ((start = str.indexOf('E')) != -1)
    start = str.length() - start;
  else
    start = 0;

  QString result = str;
  int i = str.length() - start;
  bool bFinished = false;
  while ( !bFinished && i > 0 )
  {
    QChar ch = result[i - 1];
    if (ch == '0')
      result.remove (--i,1);
    else
    {
      bFinished = true;
      if ( result.mid( i - decimalSymbol.length(), decimalSymbol.length() ) == decimalSymbol )
        result.remove( i - decimalSymbol.length(), decimalSymbol.length() );
    }
  }
  return result;
}

QString ValueFormatter::createNumberFormat ( double value, int precision,
    Format::Type fmt, Style::FloatFormat floatFormat, const QString& currencySymbol)
{
    // NOTE: If precision (obtained from the cell style) is -1 (arbitrary),
    //       use the document default decimal precision.
    int p = (precision == -1) ? doc()->defaultDecimalPrecision() : precision;
  QString localizedNumber;
  int pos = 0;

    // Always unsigned ?
    if ((floatFormat == Style::AlwaysUnsigned) && (value < 0.0))
      value *= -1.0;

  //multiply value by 100 for percentage format
  if (fmt == Format::Percentage)
    value *= 100;

  // this will avoid displaying negative zero, i.e "-0.0000"
  if( fabs( value ) < DBL_EPSILON ) value = 0.0;

  // round the number, based on desired precision if not scientific is chosen
  //(scientific has relative precision)
  if( fmt != Format::Scientific )
  {
    double m[] = { 1, 10, 100, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10 };
    double mm = (p > 10) ? pow(10.0,p) : m[p];
    bool neg = value < 0;
    value = floor( fabs(value)*mm + 0.5 ) / mm;
    if( neg ) value = -value;
  }

  switch (fmt)
  {
    case Format::Number:
      localizedNumber = m_converter->locale()->formatNumber(value, p);
      break;
    case Format::Percentage:
      localizedNumber = m_converter->locale()->formatNumber (value, p)+ " %";
      break;
    case Format::Money:
      localizedNumber = m_converter->locale()->formatMoney (value,
        currencySymbol.isEmpty() ? m_converter->locale()->currencySymbol() : currencySymbol, p );
      break;
    case Format::Scientific:
    {
      const QString decimalSymbol = m_converter->locale()->decimalSymbol();
      localizedNumber = QString::number (value, 'E', p);
      if ((pos = localizedNumber.indexOf('.')) != -1)
        localizedNumber = localizedNumber.replace (pos, 1, decimalSymbol);
      break;
    }
    default :
      //other formatting?
      // This happens with Format::Custom...
      kDebug(36001)<<"Wrong usage of ValueFormatter::createNumberFormat fmt=" << fmt << "\n";
      break;
  }

  //prepend positive sign if needed
  if ((floatFormat == Style::AlwaysSigned) && value >= 0 )
    if (m_converter->locale()->positiveSign().isEmpty())
      localizedNumber='+'+localizedNumber;

    // Remove trailing zeros and the decimal point if necessary
    // unless the number has no decimal point
    if ( precision == -1 )
    {
      QString decimalSymbol = m_converter->locale()->decimalSymbol();
      if ( decimalSymbol.isNull() )
        decimalSymbol = '.';

      localizedNumber = removeTrailingZeros( localizedNumber, decimalSymbol );
    }

  return localizedNumber;
}

QString ValueFormatter::fractionFormat (double value, Format::Type fmtType)
{
  double result = value - floor(value);
  int index;
  int limit = 0;

  /* return w/o fraction part if not necessary */
  if (result == 0)
    return QString::number(value);

  switch (fmtType) {
  case Format::fraction_half:
    index = 2;
    break;
  case Format::fraction_quarter:
    index = 4;
    break;
  case Format::fraction_eighth:
    index = 8;
    break;
  case Format::fraction_sixteenth:
    index = 16;
    break;
  case Format::fraction_tenth:
    index = 10;
    break;
  case Format::fraction_hundredth:
    index = 100;
    break;
  case Format::fraction_one_digit:
    index = 3;
    limit = 9;
    break;
  case Format::fraction_two_digits:
    index = 4;
    limit = 99;
    break;
  case Format::fraction_three_digits:
    index = 5;
    limit = 999;
    break;
  default:
    kDebug(36001) << "Error in Fraction format\n";
    return QString::number(value);
    break;
  } /* switch */


  /* handle halves, quarters, tenths, ... */

  if (fmtType != Format::fraction_three_digits
    && fmtType != Format::fraction_two_digits
    && fmtType != Format::fraction_one_digit) {
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


  /* handle Format::fraction_one_digit, Format::fraction_two_digit
    * and Format::fraction_three_digit style */

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

QString ValueFormatter::timeFormat (const QDateTime &_dt, Format::Type fmtType)
{
  const QDateTime dt( _dt.toUTC() );
  if (fmtType == Format::Time)
    return m_converter->locale()->formatTime(dt.time(), false);

  if (fmtType == Format::SecondeTime)
    return m_converter->locale()->formatTime(dt.time(), true);

  int h = dt.time().hour();
  int m = dt.time().minute();
  int s = dt.time().second();

  QString hour = ( h < 10 ? '0' + QString::number(h) : QString::number(h) );
  QString minute = ( m < 10 ? '0' + QString::number(m) : QString::number(m) );
  QString second = ( s < 10 ? '0' + QString::number(s) : QString::number(s) );
  bool pm = (h > 12);
  QString AMPM( pm ? i18n("PM"):i18n("AM") );

  if (fmtType == Format::Time1) {  // 9 : 01 AM
    return QString("%1:%2 %3")
      .arg((pm ? h - 12 : h),2)
      .arg(minute,2)
      .arg(AMPM);
  }

  if (fmtType == Format::Time2) {  //9:01:05 AM
    return QString("%1:%2:%3 %4")
      .arg((pm ? h-12 : h),2)
      .arg(minute,2)
      .arg(second,2)
      .arg(AMPM);
  }

  if (fmtType == Format::Time3) {
    return QString("%1 %2 %3 %4 %5 %6")      // 9 h 01 min 28 s
      .arg(hour,2)
      .arg(i18n("h"))
      .arg(minute,2)
      .arg(i18n("min"))
      .arg(second,2)
      .arg(i18n("s"));
  }

  if (fmtType == Format::Time4) {  // 9:01
    return QString("%1:%2").arg(hour, 2).arg(minute, 2);
  }

  if (fmtType == Format::Time5) {  // 9:01:12
    return QString("%1:%2:%3").arg(hour, 2).arg(minute, 2).arg(second, 2);
  }

  QDate refDate( doc()->referenceDate() );
  int d = refDate.daysTo( dt.date() );

  h += d * 24;

  if (fmtType == Format::Time6)
  {  // [mm]:ss
    m += (h * 60);
    return QString("%1:%2").arg(m, 1).arg(second, 2);
  }
  if (fmtType == Format::Time7) {  // [h]:mm:ss
    return QString("%1:%2:%3").arg(h, 1).arg(minute, 2).arg(second, 2);
  }
  if (fmtType == Format::Time8)
  {  // [h]:mm
    m += (h * 60);
    return QString("%1:%2").arg(h, 1).arg(minute, 2);
  }

  return m_converter->locale()->formatTime( dt.time(), false );
}

QString ValueFormatter::dateFormat (const QDate &date, Format::Type fmtType)
{
  QString tmp;
  if (fmtType == Format::ShortDate) {
    tmp = m_converter->locale()->formatDate(date, KLocale::ShortDate);
  }
  else if (fmtType == Format::TextDate) {
    tmp = m_converter->locale()->formatDate(date, KLocale::LongDate);
  }
  else if (fmtType == Format::Date1) {  /*18-Feb-99 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + m_converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date2) {  /*18-Feb-1999 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + m_converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year());
  }
  else if (fmtType == Format::Date3) {  /*18-Feb */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + m_converter->locale()->calendar()->monthString(date, true);
  }
  else if (fmtType == Format::Date4) {  /*18-05 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '-' + QString().sprintf("%02d", date.month() );
  }
  else if (fmtType == Format::Date5) {  /*18/05/00 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '/' + QString().sprintf("%02d", date.month()) + '/';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date6) {  /*18/05/1999 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '/' + QString().sprintf("%02d", date.month()) + '/';
    tmp += QString::number(date.year());
  }
  else if (fmtType == Format::Date7) {  /*Feb-99 */
    tmp = m_converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date8) {  /*February-99 */
    tmp = m_converter->locale()->calendar()->monthString(date, false) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date9) {  /*February-1999 */
    tmp = m_converter->locale()->calendar()->monthString(date, false) + '-';
    tmp += QString::number(date.year());
  }
  else if (fmtType == Format::Date10) {  /*F-99 */
    tmp = m_converter->locale()->calendar()->monthString(date, false).at(0) + '-';
    tmp += QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date11) {  /*18/Feb */
    tmp = QString().sprintf("%02d", date.day()) + '/';
    tmp += m_converter->locale()->calendar()->monthString(date, true);
  }
  else if (fmtType == Format::Date12) {  /*18/02 */
    tmp = QString().sprintf("%02d", date.day()) + '/';
    tmp += QString().sprintf("%02d", date.month());
  }
  else if (fmtType == Format::Date13) {  /*18/Feb/1999 */
    tmp = QString().sprintf("%02d", date.day());
    tmp += '/' + m_converter->locale()->calendar()->monthString(date, true) + '/';
    tmp += QString::number(date.year());
  }
  else if (fmtType == Format::Date14) {  /*2000/Feb/18 */
    tmp = QString::number(date.year());
    tmp += '/' + m_converter->locale()->calendar()->monthString(date, true) + '/';
    tmp += QString().sprintf("%02d", date.day());
  }
  else if (fmtType == Format::Date15) {  /*2000-Feb-18 */
    tmp = QString::number(date.year());
    tmp += '-' + m_converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString().sprintf("%02d", date.day());
  }
  else if (fmtType == Format::Date16) {  /*2000-02-18 */
    tmp = QString::number(date.year());
    tmp += '-' + QString().sprintf("%02d", date.month()) + '-';
    tmp += QString().sprintf("%02d", date.day());
  }
  else if (fmtType == Format::Date17) {  /*2 february 2000 */
    tmp = QString().sprintf("%d", date.day());
    tmp += ' ' + m_converter->locale()->calendar()->monthString(date, false) + ' ';
    tmp += QString::number(date.year());
  }
  else if (fmtType == Format::Date18) {  /*02/18/1999 */
    tmp = QString().sprintf("%02d", date.month());
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year());
  }
  else if (fmtType == Format::Date19) {  /*02/18/99 */
    tmp = QString().sprintf("%02d", date.month());
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date20) {  /*Feb/18/99 */
    tmp = m_converter->locale()->calendar()->monthString(date, true);
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date21) {  /*Feb/18/1999 */
    tmp = m_converter->locale()->calendar()->monthString(date, true);
    tmp += '/' + QString().sprintf("%02d", date.day());
    tmp += '/' + QString::number(date.year());
  }
  else if (fmtType == Format::Date22) {  /*Feb-1999 */
    tmp = m_converter->locale()->calendar()->monthString(date, true) + '-';
    tmp += QString::number(date.year());
  }
  else if (fmtType == Format::Date23) {  /*1999 */
    tmp = QString::number(date.year());
  }
  else if (fmtType == Format::Date24) {  /*99 */
    tmp = QString::number(date.year()).right(2);
  }
  else if (fmtType == Format::Date25) {  /*2000/02/18 */
    tmp = QString::number(date.year());
    tmp += '/' + QString().sprintf("%02d", date.month());
    tmp += '/' + QString().sprintf("%02d", date.day());
  }
  else if (fmtType == Format::Date26) {  /*2000/Feb/18 */
    tmp = QString::number(date.year());
    tmp += '/' + m_converter->locale()->calendar()->monthString(date, true);
    tmp += '/' + QString().sprintf("%02d", date.day());
  }
  else
    tmp = m_converter->locale()->formatDate(date, KLocale::ShortDate);

  // Missing compared with gnumeric:
  //  "m/d/yy h:mm",    /* 20 */
  //  "m/d/yyyy h:mm",  /* 21 */
  //  "mmm/ddd/yy",    /* 12 */
  //  "mmm/ddd/yyyy",    /* 13 */
  //  "mm/ddd/yy",    /* 14 */
  //  "mm/ddd/yyyy",    /* 15 */

  return tmp;
}

QString ValueFormatter::complexFormat( const Value& value, int precision,
                                       Format::Type formatType,
                                       Style::FloatFormat floatFormat,
                                       const QString& currencySymbol )
{
    // FIXME Stefan: percentage, currency and scientific formats!
    QString str;
    const double real = value.asComplex().real();
    const double imag = value.asComplex().imag();
    str = createNumberFormat( real, precision, formatType, floatFormat, QString() );
    str += createNumberFormat( imag, precision, formatType, Style::AlwaysSigned, currencySymbol );
    str += 'i';
    return str;
}
