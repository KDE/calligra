/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "ValueParser.h"

#include "Cell.h"
#include "Format.h"
#include "Locale.h"
#include "Value.h"

using namespace KSpread;

ValueParser::ValueParser( KLocale* locale ) : parserLocale( locale )
{
}

KLocale* ValueParser::locale()
{
  return parserLocale;
}

void ValueParser::parse (const QString& str, Cell *cell)
{
  FormatType format = cell->formatType();

  // If the text is empty, we don't have a value
  // If the user stated explicitly that he wanted text
  // (using the format or using a quote),
  // then we don't parse as a value, but as string.
  if ( str.isEmpty() || format == Text_format || str.at(0)=='\'' )
  {
    cell->setValue (str);
    return;
  }

  QString strStripped = str.trimmed();

  // Try parsing as various datatypes, to find the type of the cell

  // First as number
  if (tryParseNumber (strStripped, cell))
    return;

  // Then as bool
  if (tryParseBool (strStripped, cell))
    return;


  // Test for money number
  bool ok;
  double money = parserLocale->readMoney (strStripped, &ok);
  if (ok)
  {
    cell->format()->setPrecision(2);
    Value val (money);
    val.setFormat (Value::fmt_Money);
    cell->setValue (val);
    return;
  }

  if (tryParseDate (strStripped, cell))
    return;

  if (tryParseTime (strStripped, cell))
    return;

  // Nothing particular found, then this is simply a string
  cell->setValue (Value (str));
}

Value ValueParser::parse (const QString &str)
{
  Value val;

  // If the text is empty, we don't have a value
  // If the user stated explicitly that he wanted text
  // (using the format or using a quote),
  // then we don't parse as a value, but as string.
  if ( str.isEmpty() || str.at(0)=='\'' )
  {
    val.setValue (str);
    return val;
  }

  bool ok;

  QString strStripped = str.trimmed();
  // Try parsing as various datatypes, to find the type of the string

  // First as number
  val = tryParseNumber (strStripped, &ok);
  if (ok)
    return val;

 // Then as bool
 // Note - I swapped the order of these two to try parsing as a number
 // first because that will probably be the most common case
  val = tryParseBool (strStripped, &ok);
  if (ok)
    return val;


  // Test for money number
  double money = parserLocale->readMoney (strStripped, &ok);
  if (ok)
  {
    val.setValue (money);
    val.setFormat (Value::fmt_Money);
    return val;
  }

  val = tryParseDate (strStripped, &ok);
  if (ok)
    return val;

  val = tryParseTime (strStripped, &ok);
  if (ok)
    return val;

  // Nothing particular found, then this is simply a string
  val.setValue (str);
  return val;
}

bool ValueParser::tryParseBool (const QString& str, Cell *cell)
{
  bool ok;
  Value val = tryParseBool (str, &ok);
  if (ok)
    cell->setValue (val);
  return ok;
}

bool ValueParser::tryParseNumber (const QString& str, Cell *cell)
{
  bool ok;
  Value val = tryParseNumber (str, &ok);
  if (ok)
    cell->setValue (val);
  return ok;
}

bool ValueParser::tryParseDate (const QString& str, Cell *cell)
{
  bool ok;
  Value value = tryParseDate (str, &ok);
  if (ok)
    cell->setValue (value);
  return ok;
}

bool ValueParser::tryParseTime (const QString& str, Cell *cell)
{
  bool ok;
  Value value = tryParseTime (str, &ok);
  if (ok)
    cell->setValue (value);
  return ok;
}


Value ValueParser::tryParseBool (const QString& str, bool *ok)
{
  Value val;
  if (ok) *ok = false;

  const QString& lowerStr = str.toLower();

  if ((lowerStr == "true") ||
       (lowerStr == ki18n("true").toString(parserLocale).toLower()))
  {
    val.setValue (true);
    if (ok) *ok = true;
  }
  else if ((lowerStr == "false") ||
      (lowerStr == ki18n("false").toString(parserLocale).toLower()))
  {
    val.setValue (false);
    if (ok) *ok = true;
    fmtType = Number_format;    //TODO: really?
  }
  return val;
}

double ValueParser::readNumber(const QString &_str, bool * ok, bool * isInt)
{
  QString str = _str.trimmed();
  bool neg = str.indexOf(parserLocale->negativeSign()) == 0;
  if (neg)
	str.remove( 0, parserLocale->negativeSign().length() );

  /* will hold the scientific notation portion of the number.
	 Example, with 2.34E+23, exponentialPart == "E+23"
  */
  QString exponentialPart;
  int EPos;

  EPos = str.indexOf('E', 0, Qt::CaseInsensitive);

  if (EPos != -1)
  {
    exponentialPart = str.mid(EPos);
    str = str.left(EPos);
  }

  int pos = str.indexOf(parserLocale->decimalSymbol());
  QString major;
  QString minor;
  if ( pos == -1 )
  {
    major = str;
    if (isInt) *isInt = true;
  }
  else
  {
    major = str.left(pos);
    minor = str.mid(pos + parserLocale->decimalSymbol().length());
    if (isInt) *isInt = false;
  }

  // Remove thousand separators
  int thlen = parserLocale->thousandsSeparator().length();
  int lastpos = 0;
  while ( ( pos = major.indexOf( parserLocale->thousandsSeparator() ) ) > 0 )
  {
    // e.g. 12,,345,,678,,922 Acceptable positions (from the end) are 5, 10, 15... i.e. (3+thlen)*N
    int fromEnd = major.length() - pos;
    if ( fromEnd % (3+thlen) != 0 // Needs to be a multiple, otherwise it's an error
         || pos - lastpos > 3 // More than 3 digits between two separators -> error
         || pos == 0          // Can't start with a separator
         || (lastpos>0 && pos-lastpos!=3))   // Must have exactly 3 digits between two separators
    {
      if (ok) *ok = false;
      return 0.0;
    }

    lastpos = pos;
    major.remove( pos, thlen );
  }
  if (lastpos>0 && major.length()-lastpos!=3)   // Must have exactly 3 digits after the last separator
  {
    if (ok) *ok = false;
    return 0.0;
  }

  QString tot;
  if (neg) tot = '-';

  tot += major + '.' + minor + exponentialPart;

  return tot.toDouble(ok);
}

Value ValueParser::tryParseNumber (const QString& str, bool *ok)
{
  Value value;

  bool percent = false;
  QString str2;
  if( !str.isEmpty() && str.at(str.length()-1)=='%')
  {
    str2 = str.left (str.length()-1).trimmed();
    percent = true;
  }
  else
    str2 = str;


  // First try to understand the number using the parserLocale
  bool isInt;
  double val = readNumber (str2, ok, &isInt);
  // If not, try with the '.' as decimal separator
  if (!(*ok))
  {
    val = str2.toDouble(ok);
    if (str.contains('.'))
      isInt = false;
    else
      isInt = true;
  }

  if (*ok)
  {
    if (percent)
    {
      //kDebug(36001) << "ValueParser::tryParseNumber '" << str <<
      //    "' successfully parsed as percentage: " << val << '%' << endl;
      value.setValue (val / 100.0);
      value.setFormat (Value::fmt_Percent);
      fmtType = Percentage_format;
    }
    else
    {
      //kDebug(36001) << "ValueParser::tryParseNumber '" << str <<
      //    "' successfully parsed as number: " << val << endl;
	  if (isInt)
		value.setValue (static_cast<long> (val));
	  else
		value.setValue (val);

      if ( str2.contains('E') || str2.contains('e') )
        fmtType = Scientific_format;
      else
      {
        if (val > 1e+10)
          fmtType = Scientific_format;
        else
          fmtType = Number_format;
      }
    }
  }

  return value;
}

Value ValueParser::tryParseDate (const QString& str, bool *ok)
{
  bool valid = false;
  QDate tmpDate = parserLocale->readDate (str, &valid);
  if (!valid)
  {
    // Try without the year
    // The tricky part is that we need to remove any separator around the year
    // For instance %Y-%m-%d becomes %m-%d and %d/%m/%Y becomes %d/%m
    // If the year is in the middle, say %m-%Y/%d, we'll remove the sep.
    // before it (%m/%d).
    QString fmt = parserLocale->dateFormatShort();
    int yearPos = fmt.indexOf("%Y", 0, Qt::CaseInsensitive);
    if ( yearPos > -1 )
    {
      if ( yearPos == 0 )
      {
        fmt.remove( 0, 2 );
        while ( fmt[0] != '%' )
          fmt.remove( 0, 1 );
      } else
      {
        fmt.remove( yearPos, 2 );
        for ( ; yearPos > 0 && fmt[yearPos-1] != '%'; --yearPos )
          fmt.remove( yearPos, 1 );
      }
      //kDebug(36001) << "Cell::tryParseDate short format w/o date: " << fmt << endl;
      tmpDate = parserLocale->readDate( str, fmt, &valid );
    }
  }
  if (valid)
  {
    // Note: if shortdate format only specifies 2 digits year, then 3/4/1955
    // will be treated as in year 3055, while 3/4/55 as year 2055
    // (because 55 < 69, see KLocale) and thus there's no way to enter for
    // year 1995

    // The following fixes the problem, 3/4/1955 will always be 1955

    QString fmt = parserLocale->dateFormatShort();
    if( ( fmt.contains( "%y" ) == 1 ) && ( tmpDate.year() > 2999 ) )
      tmpDate = tmpDate.addYears( -1900 );

    // this is another HACK !
    // with two digit years, 0-69 is treated as year 2000-2069 (see KLocale)
    // however, in Excel only 0-29 is year 2000-2029, 30 or later is 1930
    // onwards

    // the following provides workaround for KLocale so we're compatible
    // with Excel
    // (e.g 3/4/45 is Mar 4, 1945 not Mar 4, 2045)
    if( ( tmpDate.year() >= 2030 ) && ( tmpDate.year() <= 2069 ) )
    {
      QString yearFourDigits = QString::number( tmpDate.year() );
      QString yearTwoDigits = QString::number( tmpDate.year() % 100 );

      // if year is 2045, check to see if "2045" isn't there --> actual
      // input is "45"
      if( ( str.count( yearTwoDigits ) >= 1 ) &&
          ( str.count( yearFourDigits ) == 0 ) )
        tmpDate = tmpDate.addYears( -100 );
    }

    //test if it's a short date or text date.
    if (parserLocale->formatDate (tmpDate, false) == str)
      fmtType = TextDate_format;
    else
      fmtType = ShortDate_format;
  }
  if (!valid)
  {
    //try to use the standard Qt date parsing, using ISO 8601 format
    tmpDate = QDate::fromString(str,Qt::ISODate);
    if (tmpDate.isValid())
    {
      valid = true;
    }
  }

  if (ok)
    *ok = valid;

  return Value (tmpDate);
}

Value ValueParser::tryParseTime (const QString& str, bool *ok)
{
  if (ok)
    *ok = false;

  bool valid    = false;
  bool duration = false;
  Value val;

  QDateTime tmpTime = readTime (str, true, &valid, duration);
  if (!tmpTime.isValid())
    tmpTime = readTime (str, false, &valid, duration);

  if (!valid)
  {
    QTime tm;
    if (parserLocale->use12Clock())
    {
      QString stringPm = ki18n("pm").toString(parserLocale);
      QString stringAm = ki18n("am").toString(parserLocale);
      int pos=0;
      if((pos=str.indexOf(stringPm))!=-1)
      {
          QString tmp=str.mid(0,str.length()-stringPm.length());
          tmp=tmp.simplified();
          tm = parserLocale->readTime(tmp+' '+stringPm, &valid);
          if (!valid)
              tm = parserLocale->readTime(tmp+":00 "+stringPm, &valid);
      }
      else if((pos=str.indexOf(stringAm))!=-1)
      {
          QString tmp = str.mid(0,str.length()-stringAm.length());
          tmp = tmp.simplified();
          tm = parserLocale->readTime (tmp + ' ' + stringAm, &valid);
          if (!valid)
              tm = parserLocale->readTime (tmp + ":00 " + stringAm, &valid);
      }
    }
  }
  if (valid)
  {
    fmtType = Time_format;
    if ( duration )
    {
      val.setValue (tmpTime);
      fmtType = Time_format7;
    }
    else
      val.setValue (tmpTime.time());
  }

  if (ok)
    *ok = valid;

  return val;
}

QDateTime ValueParser::readTime (const QString & intstr, bool withSeconds,
    bool *ok, bool & duration)
{
  duration = false;
  QString str = intstr.simplified().toLower();
  QString format = parserLocale->timeFormat().simplified();
  if ( !withSeconds )
  {
    int n = format.indexOf("%S");
    format = format.left( n - 1 );
  }

  int days = -1;
  int hour = -1, minute = -1;
  int second = withSeconds ? -1 : 0; // don't require seconds
  bool g_12h = false;
  bool pm = false;
  uint strpos = 0;
  uint formatpos = 0;

  QDate refDate( 1899, 12, 31 );

  uint l  = format.length();
  uint sl = str.length();

  while (l > formatpos || sl > strpos)
  {
    if ( !(l > formatpos && sl > strpos) )
      goto error;

    QChar c( format.at( formatpos++ ) );

    if (c != '%')
    {
      if (c.isSpace())
        ++strpos;
      else if (c != str.at(strpos++))
        goto error;
      continue;
    }

    // remove space at the beginning
    if (sl > strpos && str.at( strpos).isSpace() )
      ++strpos;

    c = format.at( formatpos++ );
    switch ( c.toLatin1() )
    {
     case 'p':
      {
        QString s(ki18n("pm").toString(parserLocale).toLower());
        int len = s.length();
        if (str.mid(strpos, len) == s)
        {
          pm = true;
          strpos += len;
        }
        else
        {
          s = ki18n("am").toString(parserLocale).toLower();
          len = s.length();
          if (str.mid(strpos, len) == s)
          {
            pm = false;
            strpos += len;
          }
          else
            goto error;
        }
      }
      break;

     case 'k':
     case 'H':
      g_12h = false;
      hour = readInt(str, strpos);
      if (hour < 0)
        goto error;
      if (hour > 23)
      {
        days = (int)(hour / 24);
        hour %= 24;
      }

      break;

     case 'l':
     case 'I':
      g_12h = true;
      hour = readInt(str, strpos);
      if (hour < 1 || hour > 12)
        goto error;

      break;

     case 'M':
      minute = readInt(str, strpos);
      if (minute < 0 || minute > 59)
        goto error;

      break;

     case 'S':
      second = readInt(str, strpos);
      if (second < 0 || second > 59)
        goto error;

      break;
    }
  }

  if (g_12h)
  {
    hour %= 12;
    if (pm) hour += 12;
  }

  if (days > 0)
  {
    refDate.addDays( days );
    duration = true;
  }

  if (ok)
    *ok = true;
  return QDateTime( refDate, QTime( hour, minute, second ) );

 error:
  if (ok)
    *ok = false;
  // return invalid date if it didn't work
  return QDateTime( refDate, QTime( -1, -1, -1 ) );
}

/**
 * helper function to read integers, used in readTime
 * @param str
 * @param pos the position to start at. It will be updated when we parse it.
 * @return the integer read in the string, or -1 if no string
 */
int ValueParser::readInt (const QString &str, uint &pos)
{
  if (!str.at(pos).isDigit())
    return -1;
  int result = 0;
  for ( ; (uint) str.length() > pos && str.at(pos).isDigit(); pos++ )
  {
    result *= 10;
    result += str.at(pos).digitValue();
  }

  return result;
}

