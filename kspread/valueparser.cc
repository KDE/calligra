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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "valueparser.h"

#include "kspread_cell.h"
#include "kspread_locale.h"

using namespace KSpread;

ValueParser* ValueParser::_self = 0;

ValueParser::~ValueParser ()
{
  _self = 0;
}

ValueParser * ValueParser::self ()
{
  if (!_self)
    _self = new ValueParser;
  return _self;
}

void ValueParser::parse (const QString& str, KSpreadCell *cell)
{
  FormatType format = cell->formatType();
  
  // If the text is empty, we don't have a value
  // If the user stated explicitly that he wanted text
  // (using the format or using a quote),
  // then we don't parse as a value, but as string.
  if ( str.isEmpty() || format == Text_format || str.at(0)=='\'' )
  {
    cell->setValue (str);
    //cell->setFormatType(Text_format); // shouldn't be necessary. Won't apply with StringData anyway.
    return;
  }

  // Try parsing as various datatypes, to find the type of the cell
  // First as bool
  if (tryParseBool (str, cell))
    return;

  // Then as a number
  QString strStripped = str.stripWhiteSpace();
  if (tryParseNumber (strStripped, cell))
  {
    if ( strStripped.contains('E') || strStripped.contains('e') )
      cell->setFormatType (Scientific_format);
    else
    {
      //cell->checkNumberFormat();
      //can't call checkNumberFormat because it's protected
      if ( cell->value().asFloat() > 1e+10 )
        cell->setFormatType( Scientific_format );

    }
    return;
  }

  // Test if text is a percent value, ending with a '%'
  // It's a bit dirty to do this here, but we have to because the % gets
  // saved into the XML file. It would be cleaner to save only the numerical value
  // and treat the trailing % as pure formatting.
  if( str.at(str.length()-1)=='%')
  {
    QString strTrimmed = str.left(str.length()-1);
    if (tryParseNumber (strTrimmed, cell))
    {
      cell->setValue (KSpreadValue (cell->value().asFloat() / 100.0));
      if (cell->formatType() != Percentage_format)
      {
        cell->setFormatType (Percentage_format);
        cell->setPrecision (0); // Only set the precision if the format wasn't percentage.
      }
      cell->setFactor (100.0);
      return;
    }
  }

  // Test for money number
  bool ok;
  double money = cell->locale()->readMoney (str, &ok);
  if (ok)
  {
    cell->setFormatType(Money_format);
    cell->setFactor(1.0);
    cell->setPrecision(2);
    cell->setValue (KSpreadValue (money));
    return;
  }

  if (tryParseDate (str, cell))
  {
    //TODO: (Tomas) can't we simply use if (!isDate()) ?
    if ( format != TextDate_format &&
        !(format >= 200 && format <= 216))
    {
      //test if it's a short date or text date.
      if ((cell->locale()->formatDate( cell->value().asDateTime().date(), false) == str))
        cell->setFormatType(TextDate_format);
      else
        cell->setFormatType(ShortDate_format);
    }

    return;
  }

  if (tryParseTime (str, cell))
  {
    // Force default time format if format isn't time
    if (!cell->isTime())
      cell->setFormatType(Time_format);

    return;
  }

  // Nothing particular found, then this is simply a string
  cell->setValue (KSpreadValue (str));
}

bool ValueParser::tryParseBool (const QString& str, KSpreadCell *cell)
{
  if ((str.lower() == "true") ||
      (str.lower() == cell->locale()->translate("True").lower()))
  {
    cell->setValue (KSpreadValue (true));
    return true;
  }
  if ((str.lower() == "false") ||
      (str.lower() == cell->locale()->translate("false").lower()))
  {
    cell->setValue (KSpreadValue (false));
    return true;
  }
  return false;
}

bool ValueParser::tryParseNumber (const QString& str, KSpreadCell *cell)
{
  // First try to understand the number using the locale
  bool ok = false;
  double val = cell->locale()->readNumber (str, &ok);
  // If not, try with the '.' as decimal separator
  if ( !ok )
    val = str.toDouble(&ok);

  if ( ok )
  {
    kdDebug(36001) << "ValueParser::tryParseNumber '" << str <<
        "' successfully parsed as number: " << val << endl;
    cell->setValue (KSpreadValue (val));
    return true;
  }

  return false;
}

bool ValueParser::tryParseDate (const QString& str, KSpreadCell *cell)
{
  bool valid = false;
  QDate tmpDate = cell->locale()->readDate (str, &valid);
  if (!valid)
  {
    // Try without the year
    // The tricky part is that we need to remove any separator around the year
    // For instance %Y-%m-%d becomes %m-%d and %d/%m/%Y becomes %d/%m
    // If the year is in the middle, say %m-%Y/%d, we'll remove the sep.
    // before it (%m/%d).
    QString fmt = cell->locale()->dateFormatShort();
    int yearPos = fmt.find ("%Y", 0, false);
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
      //kdDebug(36001) << "KSpreadCell::tryParseDate short format w/o date: " << fmt << endl;
      tmpDate = cell->locale()->readDate( str, fmt, &valid );
    }
  }
  if (valid)
  {
    // Note: if shortdate format only specifies 2 digits year, then 3/4/1955
    // will be treated as in year 3055, while 3/4/55 as year 2055
    // (because 55 < 69, see KLocale) and thus there's no way to enter for
    // year 1995
  
    // The following fixes the problem, 3/4/1955 will always be 1955

    QString fmt = cell->locale()->dateFormatShort();
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
      if( ( str.contains( yearTwoDigits ) >= 1 ) &&
          ( str.contains( yearFourDigits ) == 0 ) )
        tmpDate = tmpDate.addYears( -100 );
    }
  }
  if (valid)
  {
    Q_ASSERT( tmpDate.isValid() );

    //KLocale::readDate( QString ) doesn't support long dates...
    // (David: it does now...)
    // _If_ the input is a long date, check if the first character isn't
    // a number...
    // (David: why? this looks specific to some countries)

    // Deactivating for now. If you reactivate, please explain better (David).
    //if ( str.contains( ' ' ) == 0 )  //No spaces " " in short dates...
    {
      cell->setValue (KSpreadValue (tmpDate));
      return true;
    }
  }
  return false;
}

bool ValueParser::tryParseTime (const QString& str, KSpreadCell *cell)
{
  bool valid    = false;
  bool duration = false;

  QDateTime tmpTime = readTime(str, cell->locale(), true, &valid, duration);
  if (!tmpTime.isValid())
    tmpTime = readTime (str, cell->locale(), false, &valid, duration);

  if (!valid)
  {
    QTime tm;
    if (cell->locale()->use12Clock())
    {
      QString stringPm = cell->locale()->translate("pm");
      QString stringAm = cell->locale()->translate("am");
      int pos=0;
      if((pos=str.find(stringPm))!=-1)
      {
          QString tmp=str.mid(0,str.length()-stringPm.length());
          tmp=tmp.simplifyWhiteSpace();
          tm = cell->locale()->readTime(tmp+" "+stringPm, &valid);
          if (!valid)
              tm = cell->locale()->readTime(tmp+":00 "+stringPm, &valid);
      }
      else if((pos=str.find(stringAm))!=-1)
      {
          QString tmp = str.mid(0,str.length()-stringAm.length());
          tmp = tmp.simplifyWhiteSpace();
          tm = cell->locale()->readTime (tmp + " " + stringAm, &valid);
          if (!valid)
              tm = cell->locale()->readTime (tmp + ":00 " + stringAm, &valid);
      }
    }
    if ( valid )
      cell->setValue (KSpreadValue (tm));
    return valid;
  }
  if (valid)
  {
    if ( duration )
    {
      cell->setValue (KSpreadValue (tmpTime));
      cell->setFormatType( Time_format7 );
    }
    else
      cell->setValue (KSpreadValue (tmpTime.time()));
  }
  return valid;
}


QDateTime ValueParser::readTime (const QString & intstr, KLocale * locale,
    bool withSeconds, bool * ok, bool & duration)
{
  duration = false;
  QString str = intstr.simplifyWhiteSpace().lower();
  QString format = locale->timeFormat().simplifyWhiteSpace();
  if ( !withSeconds )
  {
    int n = format.find("%S");
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

    // remove space at the begining
    if (sl > strpos && str.at( strpos).isSpace() )
      ++strpos;

    c = format.at( formatpos++ );
    switch (c)
    {
     case 'p':
      {
        QString s;
        s = locale->translate("pm").lower();
        int len = s.length();
        if (str.mid(strpos, len) == s)
        {
          pm = true;
          strpos += len;
        }
        else
        {
          s = locale->translate("am").lower();
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
  for ( ; str.length() > pos && str.at(pos).isDigit(); pos++ )
  {
    result *= 10;
    result += str.at(pos).digitValue();
  }

  return result;
}

