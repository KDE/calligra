/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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

/*
The only function visible from the outside is formatNumber, whose
implementation is at the very bottom of this file. Its prototype
is declared in kspread_util.h. However, it is not used anywhere.
TODO: Find out whether it is supposed to be used instead of
something else (locale()->formatNumber() maybe?) and either use it
or get rid of it.
Tomas
*/
#include <ctype.h>
#include <math.h>

#include <qdatetime.h>
#include <qmap.h>
#include <qstring.h>

#include <kcalendarsystem.h>
#include <klocale.h>

#include "kspread_util.h"
#include "kspread_value.h"

namespace NumFormat_Local
{
  enum { Unknown, TimeDate, Number, Scientific, Fraction } Type;

  QString g_Monday;
  QString g_Tuesday;
  QString g_Wednesday;
  QString g_Thursday;
  QString g_Friday;
  QString g_Saturday;
  QString g_Sunday;
  QString g_Mon;
  QString g_Tue;
  QString g_Wed;
  QString g_Thu;
  QString g_Fri;
  QString g_Sat;
  QString g_Sun;
  QString g_January;
  QString g_February;
  QString g_March;
  QString g_April;
  QString g_MayL;
  QString g_June;
  QString g_July;
  QString g_August;
  QString g_September;
  QString g_October;
  QString g_November;
  QString g_December;
  QString g_Jan;
  QString g_Feb;
  QString g_Mar;
  QString g_Apr;
  QString g_May;
  QString g_Jun;
  QString g_Jul;
  QString g_Aug;
  QString g_Sep;
  QString g_Oct;
  QString g_Nov;
  QString g_Dec;

  struct DateTime
  {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
  };

  struct ConvertionInfo
  {
    DateTime * dt;

    int        rightOpt;
    int        rightReq;
    int        leftReq;
    int        rightSpace;
    int        leftSpace;
    int        upReq;

    int        reqCounter;
    int        reqFirst;
    int        optFirst;

    bool       ampm;

    bool       thSet;
    bool       showMinus;
    bool       negRed;
    bool       negBr;
    QString    postfix;
    QString    prefix;
  };

  class BaseFormat
  {
   public: 
    int        type;

    QString    postfix;
    QString    prefix;
  };

  class NumberFormat : public BaseFormat
  {
   public:
    bool       thSet;
    bool       showMinus;
    bool       negRed;
    bool       negBr;
    int        rightOpt;
    int        rightReq;
    int        leftReq;
    int        rightSpace;
    int        leftSpace;
  };

  class FractionFormat : public BaseFormat
  {
   public:
    bool       thSet;
    bool       showMinus;
    bool       negRed;
    bool       negBr;
    int        optFirst;
    int        reqFirst;
    int        reqCounter;
    int        fraction;
    int        fractionDigists;
  };

  class DateTimeFormat : public BaseFormat
  {
   public:
    bool       ampm;    
    QString    format;
  };

  class ScientificFormat : public BaseFormat
  {
   public:
    bool       thSet;
    int        leftReq;
    int        rightReq;
    int        rightOpt;
    int        upReq;
    bool       showMinus;
    bool       negRed;
    bool       negBr;
    int        rightSpace;
    int        leftSpace;
  };

  class FormatStore
  {
   public:

    int  getType( QString const & format, BaseFormat * f ) const
    {
      FormatMap::const_iterator iter = m_formats.find( format );
      if ( iter == m_formats.end() )
      {
        f = 0;
        return -1;
      }

      f = iter.data();
      return f->type;
    }

    void addFraction( QString const & format, FractionFormat * f )
    {
      m_formats.insert( format, f );
    }

    void addNumber( QString const & format, NumberFormat * n )
    {
      m_formats.insert( format, n );
    }

    void addDateTime( QString const & format, DateTimeFormat * d )
    {
      m_formats.insert( format, d );
    }

    void addScientific( QString const & format, ScientificFormat * d )
    {
      m_formats.insert( format, d );
    }

   private:
    class FormatMap : public QMap<QString, BaseFormat *> {};
    FormatMap    m_formats;
  };

  QChar          g_dcSymbol;
  QChar          g_thSymbol;
  QChar          g_posSymbol;
  QChar          g_negSymbol;
  DateTime       g_dateTime;
  ConvertionInfo g_convertionInfo;
  bool           g_init = false;

  FormatStore    g_formatStore;
}

using namespace NumFormat_Local;
using namespace KSpread;

void resetGlobals()
{
  g_convertionInfo.dt = 0;
  g_convertionInfo.thSet = false;
  g_convertionInfo.showMinus = true;
  g_convertionInfo.negRed = false;
  g_convertionInfo.negBr = false;
  g_convertionInfo.reqCounter = 0;
  g_convertionInfo.reqFirst = 0;
  g_convertionInfo.prefix = "";
  g_convertionInfo.postfix = "";
  g_convertionInfo.rightOpt = 0;
  g_convertionInfo.rightReq = 0;
  g_convertionInfo.leftReq = 0;
  g_convertionInfo.rightSpace = 0;
  g_convertionInfo.leftSpace = 0;
  g_convertionInfo.optFirst = 0;
  g_convertionInfo.upReq = 0;
  g_convertionInfo.ampm = false;
}

void initGlobals( KLocale const * const locale )
{
  g_Monday    = locale->calendar()->weekDayName( 1, false );
  g_Tuesday   = locale->calendar()->weekDayName( 2, false );
  g_Wednesday = locale->calendar()->weekDayName( 3, false );
  g_Thursday  = locale->calendar()->weekDayName( 4, false );
  g_Friday    = locale->calendar()->weekDayName( 5, false );
  g_Saturday  = locale->calendar()->weekDayName( 6, false );
  g_Sunday    = locale->calendar()->weekDayName( 7, false );
  g_Mon       = locale->calendar()->weekDayName( 1, true );
  g_Tue       = locale->calendar()->weekDayName( 2, true );
  g_Wed       = locale->calendar()->weekDayName( 3, true );
  g_Thu       = locale->calendar()->weekDayName( 4, true );
  g_Fri       = locale->calendar()->weekDayName( 5, true );
  g_Sat       = locale->calendar()->weekDayName( 6, true );
  g_Sun       = locale->calendar()->weekDayName( 7, true );
  g_January   = locale->calendar()->monthName(  1, 2005, false );
  g_February  = locale->calendar()->monthName(  2, 2005, false );
  g_March     = locale->calendar()->monthName(  3, 2005, false );
  g_April     = locale->calendar()->monthName(  4, 2005, false );
  g_MayL      = locale->calendar()->monthName(  5, 2005, false );
  g_June      = locale->calendar()->monthName(  6, 2005, false );
  g_July      = locale->calendar()->monthName(  7, 2005, false );
  g_August    = locale->calendar()->monthName(  8, 2005, false );
  g_September = locale->calendar()->monthName(  9, 2005, false );
  g_October   = locale->calendar()->monthName( 10, 2005, false );
  g_November  = locale->calendar()->monthName( 11, 2005, false );
  g_December  = locale->calendar()->monthName( 12, 2005, false );
  g_Jan       = locale->calendar()->monthName(  1, 2005, true );
  g_Feb       = locale->calendar()->monthName(  2, 2005, true );
  g_Mar       = locale->calendar()->monthName(  3, 2005, true );
  g_Apr       = locale->calendar()->monthName(  4, 2005, true );
  g_May       = locale->calendar()->monthName(  5, 2005, true );
  g_Jun       = locale->calendar()->monthName(  6, 2005, true );
  g_Jul       = locale->calendar()->monthName(  7, 2005, true );
  g_Aug       = locale->calendar()->monthName(  8, 2005, true );
  g_Sep       = locale->calendar()->monthName(  9, 2005, true );
  g_Oct       = locale->calendar()->monthName( 10, 2005, true );
  g_Nov       = locale->calendar()->monthName( 11, 2005, true );
  g_Dec       = locale->calendar()->monthName( 12, 2005, true );

  g_dcSymbol  = locale->decimalSymbol()[0];
  g_thSymbol  = locale->thousandsSeparator()[0];
  g_posSymbol = locale->positiveSign()[0];
  g_negSymbol = locale->negativeSign()[0];

  g_init = true;
}

void convertDateTime( Value const & value )
{
  QDateTime dt( value.asDateTime() );
  QDate d( dt.date() );
  QTime t( dt.time() );

  g_dateTime.year   = d.year();
  g_dateTime.month  = d.month();
  g_dateTime.day    = d.day();
  g_dateTime.hour   = t.hour();
  g_dateTime.minute = t.minute();
  g_dateTime.second = t.second();

  g_convertionInfo.dt = &g_dateTime;
}

void parseNegativePart( QString & format, int i, 
                        int l, bool acceptDigits )
{
  g_convertionInfo.showMinus = false;
  g_convertionInfo.negRed    = false;
  g_convertionInfo.negRed    = false;
  bool end = false;

  while ( i < l && !end)
  {
    QChar c( format[i] );
    switch( c )
    {
     case '-':
      g_convertionInfo.showMinus = true;
      break;
     case '(':
      g_convertionInfo.negBr = true;
      break;
     case '[':
      if ( format.find( "[red]", i, false ) == i )
      {
        g_convertionInfo.negRed = true;
        i += 5;
      }
      break;
     default:
      end = true;
    }
    ++i;
  }

  // find postfix
  bool quote = false;
  for ( int j = l - 1; j > i; --j )
  {
    if ( format[j] == '"' )
    {
      quote = !quote;
      continue;
    }

    if ( !quote && ( format[j] == '0' || format[j] != '?' 
                     || format[j] != '#' 
                     || ( format[j].isDigit() && acceptDigits ) ) )
    {
      g_convertionInfo.postfix = format.mid( j + 1 );
      format.remove( (unsigned int) (j + 1), (unsigned int) (l - j) );
      break;
    }
  }

  int p = g_convertionInfo.postfix.find( '"' );
  while ( p != -1 )
  {
    g_convertionInfo.postfix.remove( p, 1 );

    p = g_convertionInfo.postfix.find( '"', p );
  }
}

void createNumberStruct( BaseFormat * data, QString const & format, bool insert )
{
  NumberFormat * d = new NumberFormat();
  d->type       = Number;
  d->prefix     = g_convertionInfo.prefix;
  d->postfix    = g_convertionInfo.postfix;
  d->thSet      = g_convertionInfo.thSet;
  d->showMinus  = g_convertionInfo.showMinus;
  d->negRed     = g_convertionInfo.negRed;
  d->negBr      = g_convertionInfo.negBr;
  d->rightOpt   = g_convertionInfo.rightOpt;
  d->rightReq   = g_convertionInfo.rightReq;
  d->leftReq    = g_convertionInfo.leftReq;
  d->rightSpace = g_convertionInfo.rightSpace;
  d->leftSpace  = g_convertionInfo.leftSpace;

  if ( insert )
    g_formatStore.addNumber( format, d );
  data = d;
}

void createDateTimeStruct( BaseFormat * data, QString const & format, 
                           QString const & optFormat, bool insert )
{
  DateTimeFormat * d = new DateTimeFormat();
  d->type       = TimeDate;
  d->prefix     = g_convertionInfo.prefix;
  d->postfix    = g_convertionInfo.postfix;
  d->ampm       = g_convertionInfo.ampm;
  d->format     = optFormat;

  if ( insert )
    g_formatStore.addDateTime( format, d );
  data = d;
}

void createScientificStruct( BaseFormat * data, QString const & format, bool insert )
{
  ScientificFormat * d = new ScientificFormat();
  d->type       = Scientific;
  d->prefix     = g_convertionInfo.prefix;
  d->postfix    = g_convertionInfo.postfix;
  d->thSet      = g_convertionInfo.thSet;
  d->showMinus  = g_convertionInfo.showMinus;
  d->negRed     = g_convertionInfo.negRed;
  d->negBr      = g_convertionInfo.negBr;
  d->rightOpt   = g_convertionInfo.rightOpt;
  d->rightReq   = g_convertionInfo.rightReq;
  d->leftReq    = g_convertionInfo.leftReq;
  d->rightSpace = g_convertionInfo.rightSpace;
  d->leftSpace  = g_convertionInfo.leftSpace;
  d->upReq      = g_convertionInfo.upReq;

  if ( insert )
    g_formatStore.addScientific( format, d );
  data = d;
}


int doPreScan( QString & format, QString const & formatBack, KLocale const * const /* locale */,
               bool insert, BaseFormat * data )
{  
  int type = g_formatStore.getType( format, data );
  if ( data != 0 )
    return type;

  resetGlobals();

  int l = format.length();
  int i = 0;
  int thFound = false;
  int leftReq  = 0;
  int leftOpt  = 0;
  int rightOpt = 0;
  int spaceInNum = -1;
  bool dcSeen = false;
  bool endFixed = false;

  FractionFormat * df = 0;
  int f   = 0;
  int d   = 0;
  int len = 0;
  int n   = 0;
  bool ok = false;
  QString frac;

  while ( i < l )
  {
    QString s;
    if ( endFixed )
    {
      g_convertionInfo.postfix += format.mid( i );
      format.remove( i, l - i );
      break;
    }
    QChar ch( format[i] );
    switch( ch )
    {
     case '[':
      if ( type == Number )
        endFixed = true;

      if ( format[ i + 1] == '$' )
      {
        i += 2;
        bool found = false;
        while ( i < l && format[i] != ']' )
        {
          if ( format[i] == '-' )
            found = true;
          if ( !found )
          {
            if ( !endFixed )
              g_convertionInfo.prefix += format[i];
            else
              g_convertionInfo.postfix += format[i];
            format.remove( i, 1 );
            --i; --l;
          }
          ++i;
        }
      }
      else
      {
        if ( i + 1 >= l )  
        {
          g_convertionInfo.postfix += '[';
          format.remove( i, 1 );
          --l; --i;
        }
        else
          if ( ( format[ i + 1].lower() != 's' )
               && ( format[ i + 1].lower() != 'm' )
               && ( format[ i + 1].lower() != 'h' ) )
          {
            // strange!

            if ( endFixed )
              g_convertionInfo.postfix += format[i];
            else
              g_convertionInfo.prefix  += format[i];
            format.remove( i, 1 );
            --l; --i;
          }
          else
          {
            type = TimeDate;
            ++i;
            QChar c( format[i] );
            ++i;
            while ( i < l && format[i] != ']' )
            {
              if ( format[i] != c )
              {
                format.remove( i, 1 );
                --l; --i;
                break;
              }
              ++i;
            }
          }
      }
      break;

     case '¤':
     case '$':
     case '¥':
     case '£':
     case '%':
      if ( type == Number )
        endFixed = true;

      if ( endFixed )
        g_convertionInfo.postfix += format[i];
      else
        g_convertionInfo.prefix  += format[i];
      format.remove( i, 1 );
      --i; --l;
      break;

     case '#':
      type = Number;
      if ( !dcSeen && leftReq > 0 )
      { // 00##.00 <= remove the '#'
        format.remove( i, 1 );
        --l; --i;
      }
      if ( !dcSeen )
        ++leftOpt;
      else
        ++g_convertionInfo.rightOpt;
      break;

     case '0':
      if ( spaceInNum > 0 )
      { // for fractions
        ++g_convertionInfo.reqCounter;
        break;
      }
      type = Number;
      if ( !dcSeen && rightOpt > 0 )
      { // 00##.##00 <= remove the '0'
        format.remove( i, 1 );
        --l; --i;
      }
      if ( !dcSeen )
        ++g_convertionInfo.leftReq;
      else
        ++g_convertionInfo.rightReq;
      break;

     case '/':
      if ( ( i + 1 < l ) && ( format[i + 1] == ' ' ) )
        ++i;
      while ( i < l )
      {
        if ( format[i] != '?' && !format[i].isDigit() && format[i] != '#' )
        {
          g_convertionInfo.postfix = format.mid(i);
          format.remove( i, l - i );
          break;
        }
        else 
        {
          ++d;
          frac += format[i];
        }
        ++i;
      }
      if ( i < l )
      {
        if ( format[i] == ';' )
        {
          ++i;
          parseNegativePart( format, i, l, true );
        }
        else
          if ( i + 3 < l )
          {
            if ( ( format[i + 1] == ')' ) && ( format[i + 2] == ';' ) )
            {
              i += 3;
              parseNegativePart( format, i, l, true );
            }
          }
      }      

      ok = false;
      f = frac.toInt( &ok );

      df = new FractionFormat();
      if ( ok )
        df->fraction = f;
      else
        df->fraction = -1;
      df->type       = Fraction;
      df->thSet      = g_convertionInfo.thSet;
      df->showMinus  = g_convertionInfo.showMinus;
      df->negRed     = g_convertionInfo.negRed;
      df->negBr      = g_convertionInfo.negBr;
      df->fractionDigists = d;
      df->reqCounter = g_convertionInfo.reqCounter;
      df->reqFirst   = g_convertionInfo.reqFirst;
      df->prefix     = g_convertionInfo.prefix;
      df->postfix    = g_convertionInfo.postfix;

      if ( insert )
        g_formatStore.addFraction( formatBack, df );
      data = df;

      return Fraction;
      break;

     case ',':
      if ( type == Unknown )
      {
        g_convertionInfo.prefix += ',';
      }      
      else if ( type == Number )
      {
        if ( dcSeen )
        {
          g_convertionInfo.postfix += ',';
          format.remove( i, 1 );
          --i; --l;
        }
        else
        {
          if ( thFound )
          {
            format.remove( i, 1 );
            --l; --i;
          }
          else
            thFound = true;
        }
      }

     case '.': // decimal point
      if ( type == Unknown )
      {
        int j = i + 1;
        if ( ( j < l ) 
             && ( format[j] == '0' || format[j] == '#' ) )
        {
          type = Number;
          dcSeen = true;
        }
        else
        {
          if ( j == l )
            g_convertionInfo.postfix += '.';
          else
            g_convertionInfo.prefix += '.';
          format.remove( i, 1 );
          --i; --l;
        }
      }
      else if ( type == Number )
      {
        dcSeen = true;
      }
      break;

     case '*':
      break;
      
     case '"':
      n = i;
      ++i;
      while ( i < l && format[i] != '"' )
      {
        s += format[i];
        ++i;
      }
      if ( type == Unknown )
        g_convertionInfo.prefix += s;
      else 
      {
        g_convertionInfo.postfix += s;
      }
      len = s.length();
      format.remove( i, len );
      i -= len; l -= len;
      break;

     case '_':
      if ( type == Number )
      {
        bool pr = false;
        if ( i + 3 < l )
        {
          if ( ( format[i + 1] != ')' ) || ( format[i + 2] != ';' ) )
            pr = true;
          else
          {
            i += 3;
            parseNegativePart( format, i, l, false );

            createNumberStruct( data, formatBack, insert );

            return Number;
          }
        }

        if ( pr )
        {
          g_convertionInfo.postfix += format.mid( i );
          format.remove( i, l - i );

          createNumberStruct( data, formatBack, insert );

          return Number;
        }
      }    
      break;

     case ';':
      if ( type == Unknown )
      {
        g_convertionInfo.postfix += ';';
        format.remove( i, 1 );
        --i; --l;
      }
      else
      {
        if ( type == Number )
        {
          ++i;
          parseNegativePart( format, i, l, false );

          createNumberStruct( data, formatBack, insert );

          return Number;
        }
        else
          if ( type == Scientific )
          {
            ++i;
            parseNegativePart( format, i, l, false );

            createScientificStruct( data, formatBack, insert );

            return Scientific;
          }
      }

     case ' ':
      if ( type == Number )
      {
        g_convertionInfo.optFirst = (leftOpt > 0 ? leftOpt : 0);
        g_convertionInfo.reqFirst = (leftReq > 0 ? leftReq : 0);

        spaceInNum = i;
        g_convertionInfo.postfix += ' ';
      }
      else if ( type == Unknown )
      {
        g_convertionInfo.prefix += ' ';
        format.remove( i, 1 );
        --i; --l;
      }      
      break;

     case 'A':
     case 'a':
      if ( type == TimeDate || type == Unknown )
      {
        if ( ( i + 1 < l ) && ( format[i + 1].lower() == 'm' ) )
        {
          g_convertionInfo.ampm = true;
          ++i;
          if ( ( i + 3 < l ) && ( format[i + 1] == '/' )
               && ( format[i + 2].lower() == 'p' ) 
               && ( format[i + 3].lower() == 'm' ) )
          {
            i += 3;
          }
        }
        else if ( type == Unknown )
        {
          g_convertionInfo.prefix += format[i];
          format.remove( i, 1 );
          --i; --l;
        }
      }
      else 
      {
        if ( !endFixed )
          endFixed = true;
        g_convertionInfo.postfix += format[i];
        format.remove( i, 1 );
        --i; --l;
      }
      break;

     case 'P':
     case 'p':
      if ( type == TimeDate || type == Unknown )
      {
        if ( ( i + 1 < l ) && ( format[i + 1].lower() == 'm' ) )
        {
          g_convertionInfo.ampm = true;
          i += 1;
        }
        else if ( type == Unknown )
        {
          g_convertionInfo.prefix += format[i];
          format.remove( i, 1 );
          --i; --l;
        }
      }
      else
      {
        if ( !endFixed )
          endFixed = true;
        g_convertionInfo.postfix += format[i];
        format.remove( i, 1 );
        --i; --l;
      }
      break;

     case 'M':
     case 'm':
      if ( type == Unknown )
        type = TimeDate;
      else if ( type != TimeDate )
        endFixed = true;        
      break;

     case 'S':
     case 's':
     case 'H':
     case 'h':
      if ( type != Unknown && type != TimeDate )
        endFixed = true;        
      else
        type = TimeDate;
      break;

     case 'D':
     case 'd':
     case 'Y':
     case 'y':
      if ( type != Unknown && type != TimeDate )
        endFixed = true;        
      else
        type = TimeDate;
      break;

     default:
      if ( type == Unknown )
      {
        g_convertionInfo.prefix += format[i];
        format.remove( i, 1 );
        --i; --l;
      }
      else if ( type == Number || type == Scientific 
                || type == Fraction )
      {
        endFixed = true;
        g_convertionInfo.postfix += format[i];
        format.remove( i, 1 );
        --l; --i;      
      }
    }

    ++i;
  }

  if ( type == Number )
    createNumberStruct( data, formatBack, insert );
  else if ( type == TimeDate )
    createDateTimeStruct( data, formatBack, format, insert );
  else if ( type == Scientific )
    createScientificStruct( data, formatBack, insert );

  return type;
}

void createNumber( QString & result, Value const & value, 
                   QString const & /*format*/, bool & setRed,
                   NumberFormat const * const data )
{
  int    prec = data->rightReq + data->rightOpt;
  double num  = value.asFloat();
  double m[]  = { 1, 10, 100, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10 };
  double mm   = ( prec > 10 ) ? pow( 10.0, prec ) : m[prec];

  num = floor( fabs( num ) * mm + 0.5 ) / mm;

  bool negative = ( num < 0 ? true : false );
  double nnum = ( negative ? -num : num );

  result = QString::number( nnum, 'f', prec );

  int pos = result.find( '.' );
  if ( pos >= 0 )
  {
    result = result.replace( pos, 1, g_dcSymbol );

    // remove '0' from the end if not required
    if ( data->rightOpt > 0 )
    {      
      int i = result.length() - 1; // index
      int n = result.length() - data->rightOpt;

      for ( ; i > n; --i )
      {
        if ( result[i] != '0' )
          break;
      }
      result = result.left( i + 1 ); //length

      if ( i == pos ) // just decimal point
        result = result.remove( i, 1 );
    }

    // prepend '0' if wanted
    while ( data->leftReq > pos )
    {
      result.prepend( '0' );
      ++pos;
    }

    // put in thousand symbol if wanted
    if ( data->thSet && pos > 3 )
    {
      int l = pos - 3;
      while ( 0 < l )
      {
        result.insert( l, g_thSymbol );
        l -= 3;
      }
    }
  }

  if ( data->leftSpace > 0 )
  {
    for ( int i = 0; i < data->leftSpace; ++i )
      result.prepend( ' ' );
  }
  if ( data->rightSpace > 0 )
  {
    for ( int i = 0; i < data->rightSpace; ++i )
      result.append( ' ' );
  }

  if ( negative )
  { 
    if ( data->showMinus )
      result.prepend( g_negSymbol );

    if ( data->negBr )
    {
      result.prepend( '(' );
      result.append( ')' );
    }

    if ( data->negRed )
      setRed = true;
  }

  result.prepend( data->prefix );
  result.append( data->postfix );
}

void createFraction( QString & result, Value const & value, 
                     QString const & /*format*/,  bool & setRed,
                     FractionFormat const * const data )
{
  double num = value.asFloat();

  bool negative = ( num < 0 ? true : false );

  double fnum = floor( negative ? -num : num );
  
  double dec = num - fnum;
  double fraction;
  int index = 0;

  if ( data->fraction <= 0 )
  {
    // #,### ?/???
    double nnum = ( negative ? -num : num );
    double precision, denominator, numerator;
    int index = 2 + data->fractionDigists;
    int limit = 9;
    if ( data->fractionDigists == 2 )
      limit += 90;
    if ( data->fractionDigists >= 3 )
      limit += 990;

    do 
    {
      double val1   = nnum;
      double val2   = rint( nnum );
      double inter2 = 1;
      double inter4, p,  q;
      inter4 = p = q = 0.0;

      precision   = pow( 10.0, - index );
      numerator   = val2;
      denominator = 1;

      while ( fabs( numerator / denominator - nnum ) > precision ) 
      {
        val1 = (1 / ( val1 - val2 ) );
        val2 = rint( val1 );
        p = val2 * numerator + inter2;
        q = val2 * denominator + inter4;
        inter2 = numerator;
        inter4 = denominator;

        numerator = p;
        denominator = q;
      }
      --index;
    } while ( fabs( denominator ) > limit );

    index    = (int) fabs( numerator );
    fraction = (int) fabs( denominator );
  }
  else
  {
    // # #/4
    fraction = data->fraction;

    double calc = 0.0;
    double diff = dec;
    double d;
    for ( int i = 1; i <= fraction; ++i ) 
    {
      calc = i * 1.0 / index;
      d = fabs( dec - calc );
      if ( d < diff ) 
      {
        index = i;
        diff = d;
      }
    }
  }
   
  // ? index/fraction

  // 2.25:  #/4 => 9/4
  if ( data->optFirst == 0 && data->reqFirst == 0 && fnum > 0 )
    index += (int) (fnum * fraction);
  
  QString frac;
  QString left;
  if ( index > 0 )
  {
    QString numerator;
    QString denominator;
    
    numerator = QString::number( index );
    int n = numerator.length() - data->reqCounter;
    for ( int i = 0; i < n; ++i )
    {
      numerator.prepend( '0' );
    }
    
    denominator = QString::number( fraction );
    frac = numerator + '/' + denominator;
  }
  
  if ( data->optFirst > 0 || data->reqFirst > 0 )
  {
    if ( fnum == 0 && data->reqFirst > 0 )
    {
      for ( int i = 0; i < data->reqFirst; ++i )
        left += '0';
    }
    else if ( fnum > 0 )
    {
      left = QString::number( fnum );
      int n = data->reqFirst - left.length();
      if ( n > 0 )
      {
        for ( int i = 0; i < n; ++i )
        {
          left.prepend( '0' );
        }
      }
    }
  }
  
  if ( data->thSet )
  {
    int l = left.length() - 3;
    while ( 0 < l )
    {
      left.insert( l, g_thSymbol );
      l -= 3;
    }
  }

  left = left + ' ' + frac;

  if ( negative )
  {
    if ( data->showMinus )
      left.prepend( g_negSymbol );

    if ( data->negBr )
    {
      left.prepend( '(' );
      left.append( ')' );
    }

    if ( data->negRed )
      setRed = true;
  }

  result = left;
}

void createScientific( QString & result, Value const & value, 
                       QString const & /*format*/, bool & setRed,
                       ScientificFormat const * const data )
{
  double num = value.asFloat();

  bool negative   = ( num < 0 ? true : false );
  double nnum     = ( negative ? -num : num );

  result = QString::number( nnum, 'E', data->rightReq + data->rightOpt );
  
  int pos = result.find( '.' );
  if ( pos >= 0 )
  {
    result = result.replace( pos, 1, g_dcSymbol );
    if ( data->rightOpt > 0 )
    {      
      int i   = result.find( 'E', pos, false ) - 1;
      int n   = result.length() - data->rightOpt;

      if ( i > 0 )
      {
        int rem = 0;
        for ( ; i > n; --i )
        {
          if ( result[i] != '0' )
            break;
          else
            ++rem;
        }
        result = result.remove( i + 1, rem ); 
      }
    }

    while ( data->leftReq > pos )
    {
      result.prepend( '0' );
      ++pos;
    }

    if ( data->thSet && pos > 3 )
    {
      int l = pos - 3;
      while ( 0 < l )
      {
        result.insert( l, g_thSymbol );
        l -= 3;
      }
    }
  }

  if ( negative )
  { 
    if ( data->showMinus )
      result.prepend( g_negSymbol );

    if ( data->negBr )
    {
      result.prepend( '(' );
      result.append( ')' );
    }

    if ( data->negRed )
      setRed = true;
  }

  result.prepend( data->prefix );
  result.append( data->postfix );
}

void appendAMPM( QString & result, Value const & value )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );

  int hour = g_convertionInfo.dt->hour;
  if ( hour > 12 )
    result.append( i18n("PM") );
  else
    result.append( i18n("AM") );
}

void appendHour( QString & result, Value const & value, 
                 int digits, bool elapsed, bool ampm )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );
    
  int hour = g_convertionInfo.dt->hour;
  if ( elapsed )
  {
    QDate d1( g_convertionInfo.dt->year, g_convertionInfo.dt->month, g_convertionInfo.dt->day );
    QDate d2( 1900, 1, 1 );
    hour += ( d2.daysTo( d1 ) * 24 );
  }
  if ( hour < 10 && digits == 2 )
    result += '0';
  else
    if ( hour > 12 && ampm )
    {
      hour -= 12;
      if ( digits == 2 && hour < 10 )
        result += '0';
    }

  result += QString::number( hour );
}

void appendMinutes( QString & result, Value const & value, 
                    int digits, bool elapsed )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );
    
  int minute = g_convertionInfo.dt->minute;
  if ( elapsed )
  {
    QDate d1( g_convertionInfo.dt->year, g_convertionInfo.dt->month, g_convertionInfo.dt->day );
    QDate d2( 1900, 1, 1 );
    minute += ( d2.daysTo( d1 ) * 24 * 60 );
  }
  if ( minute < 10 && digits == 2 )
    result += '0';

  result += QString::number( minute );
}

void appendSecond( QString & result, Value const & value, 
                   int digits, bool elapsed )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );
    
  int second = g_convertionInfo.dt->second;
  if ( elapsed )
  {
    QDate d1( g_convertionInfo.dt->year, g_convertionInfo.dt->month, g_convertionInfo.dt->day );
    QDate d2( 1900, 1, 1 );
    second += ( d2.daysTo( d1 ) * 24 * 60 * 60 );
  }
  if ( second < 10 && digits == 2 )
    result += '0';

  result += QString::number( second );
}

void appendYear( QString & result, Value const & value, 
                 int digits )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );

  int year = g_convertionInfo.dt->year;
  if ( digits <= 2 )
    result += QString::number( year ).right( 2 );
  else
    result += QString::number( year );
}

void appendMonth( QString & result, Value const & value, 
                  int digits )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );

  int month = g_convertionInfo.dt->month;
  if ( digits == 1 )
    result += QString::number( month );
  else
    if ( digits == 2 )
    {
      if ( month < 10 )
        result += '0';

      result += QString::number( month );
    }
    else
    {
      switch ( month )
      {
       case 1:
        result += ( digits != 3 ? g_January : g_Jan );
        break;

       case 2:
        result += ( digits != 3 ? g_February : g_Feb );
        break;

       case 3:
        result += ( digits != 3 ? g_March : g_Mar );
        break;

       case 4:
        result += ( digits != 3 ? g_April : g_Apr );
        break;

       case 5:
        result += ( digits != 3 ? g_MayL : g_May );
        break;

       case 6:
        result += ( digits != 3 ? g_June : g_Jun );
        break;

       case 7:
        result += ( digits != 3 ? g_July : g_Jul );
        break;

       case 8:
        result += ( digits != 3 ? g_August : g_Aug );
        break;

       case 9:
        result += ( digits != 3 ? g_September : g_Sep );
        break;

       case 10:
        result += ( digits != 3 ? g_October : g_Oct );
        break;

       case 11:
        result += ( digits != 3 ? g_November : g_Nov );
        break;

       case 12:
        result += ( digits != 3 ? g_December : g_Dec );
        break;
      }    
    }
}

void appendDays( QString & result, Value const & value, 
                 int digits )
{
  if ( !g_convertionInfo.dt )
    convertDateTime( value );

  int day = g_convertionInfo.dt->day;
  if ( digits == 1 )
    result += QString::number( day );
  else
    if ( digits == 2 )
    {
      if ( day < 10 )
        result += '0';

      result += QString::number( day );
    }    
    else
    {
      QDate date( g_convertionInfo.dt->year, g_convertionInfo.dt->month, g_convertionInfo.dt->day );
      int weekDay = date.dayOfWeek();
      switch ( weekDay )
      {
       case 1:
        result += ( digits != 3 ? g_Monday : g_Mon );
        break;

       case 2:
        result += ( digits != 3 ? g_Tuesday : g_Tue );
        break;

       case 3:
        result += ( digits != 3 ? g_Wednesday : g_Wed );
        break;

       case 4:
        result += ( digits != 3 ? g_Thursday : g_Thu );
        break;

       case 5:
        result += ( digits != 3 ? g_Friday : g_Fri );
        break;

       case 6:
        result += ( digits != 3 ? g_Saturday : g_Sat );
        break;

       case 7:
        result += ( digits != 3 ? g_Sunday : g_Sun );
        break;
      }
    }
}

void createDateTime( QString & result, Value const & value, 
                     QString const & /*format*/, 
                     DateTimeFormat const * const data )
{
  result = data->prefix;
  bool elapsed = false;
  bool elapsedFound = false;
  bool minute = false; // how to interpret 'm'
  int digits = 1;
  int i = 0;
  int l = (int) data->format.length();
  while ( i < l )
  {
    switch( data->format[i].lower() )
    {
     case '"':
      ++i;
      while ( i < l )
      {
        if ( data->format[i] == '"' )
          break;
        else
          result += data->format[i];
      }
      break;

     case '[':
      if ( elapsedFound )
        result += '[';
      else
      {
        elapsed = true;
        elapsedFound = true;
      }
      break;

     case ']':
      if ( elapsed )
        elapsed = false;
      else
        result += ']';
      break;

     case 'h':
      minute = true;
      if ( data->format[i + 1] == 'h' )
      {
        appendHour( result, value, 2, elapsed, data->ampm );
        ++i;
      }
      else
        appendHour( result, value, 1, elapsed, data->ampm );
      break;

     case 'm':
      digits = 1;
      
      while ( data->format[i + 1] == 'm' )
      {
        ++i;
        ++digits;
      }

      if ( minute )
        appendMinutes( result, value, digits, elapsed );
      else
        appendMonth( result, value, digits );
      
      break;

     case 's':
      minute = true;
      if ( data->format[i + 1] == 's' )
      {
        appendSecond( result, value, 2, elapsed );
        ++i;
      }
      else
        appendSecond( result, value, 1, elapsed );
      break;

     case 'd':
      minute = false;
      digits = 1;
      
      while ( data->format[i + 1] == 'd' )
      {
        ++i;
        ++digits;
      }
      appendDays( result, value, digits );
      break;

     case 'y':
      minute = false;
      digits = 1;
      
      while ( data->format[i + 1] == 'y' )
      {
        ++i;
        ++digits;
      }
      appendYear( result, value, digits );
      break;

     case 'a':
     case 'p':
      if ( data->format[i + 1] == 'm' )
      {
        ++i;
        if ( data->format[i + 1] == '/' 
             && data->format[i + 2].lower() == 'p' 
             && data->format[i + 3].lower() == 'm' )
          i += 3;

        appendAMPM( result, value );
      }
      
     default:
      result += data->format[i];
    }

    ++i;
  }

  result += data->postfix;
}

QString formatNumber( Value const & value, QString format, bool & setRed,
                      KLocale const * const locale, bool insert )
{
  // need delocalized strings: dcSymbol: '.', thSymbol = ','
  if ( !g_init )
    initGlobals( locale );

  QString backup( format );
  QString result;
  BaseFormat * data = 0;
  setRed = false;

  int t = doPreScan( format, backup, locale, insert, data );

  if ( t == Number )
  {
    createNumber( result, value, format, setRed, (NumberFormat *) data );

    if ( !insert )
      delete (NumberFormat *) data;

    return result;
  }
  else if ( t == Fraction )
  {
    createFraction( result, value, format, setRed, (FractionFormat *) data );

    if ( !insert )
      delete (FractionFormat *) data;

    return result;
  }
  else if ( t == Scientific )
  {
    createScientific( result, value, format, setRed, (ScientificFormat *) data );

    if ( !insert )
      delete (ScientificFormat *) data;

    return result;
  }
  else if ( t == TimeDate )
  {
    createDateTime( result, value, format, (DateTimeFormat *) data );

    if ( !insert )
      delete (DateTimeFormat *) data;

    return result;
  }
  else if ( data != 0 )
  {
    result = data->prefix + data->postfix;

    if ( !insert )
      delete data;

    return result;
  }

  return result;
}
