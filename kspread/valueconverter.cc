/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

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

#include "valueconverter.h"

#include "kspread_locale.h"
#include "valueparser.h"

using namespace KSpread;

ValueConverter::ValueConverter (DocInfo *docinfo) : DocBase (docinfo)
{
}

KSpreadValue ValueConverter::asBoolean (const KSpreadValue &value) const
{
  KSpreadValue val;
  bool ok;  
  switch (value.type()) {
    case KSpreadValue::Empty:
      val.setValue (false);
    break;
    case KSpreadValue::Boolean:
      val = value;
    break;
    case KSpreadValue::Integer:
      val.setValue (value.asInteger() ? true : false); 
    break;
    case KSpreadValue::Float:
      val.setValue ((value.asFloat() == 0.0) ? false : true);
    break;
    case KSpreadValue::String:
      val = parser()->tryParseBool (value.asString(), &ok);
      if (!ok)
        val.setValue (false);
    break;
    case KSpreadValue::Array:
      val = asBoolean (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (false);
    break;
  };
  
  return val;
}

KSpreadValue ValueConverter::asInteger (const KSpreadValue &value) const
{
  KSpreadValue val;
  bool ok;
  
  switch (value.type()) {
    case KSpreadValue::Empty:
      val.setValue (0);
    break;
    case KSpreadValue::Boolean:
      val.setValue (value.asBoolean() ? 1 : 0);
    break;
    case KSpreadValue::Integer:
      val = value;
    break;
    case KSpreadValue::Float:
      val.setValue (value.asInteger());
    break;
    case KSpreadValue::String:
      val.setValue ((int) parser()->tryParseNumber
          (value.asString(), &ok).asFloat());
      if (!ok)
        val.setValue (0);
    break;
    case KSpreadValue::Array:
      val = asInteger (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (0);
    break;
  };
  
  return val;
}

KSpreadValue ValueConverter::asFloat (const KSpreadValue &value) const
{
  KSpreadValue val;
  bool ok;
  
  switch (value.type()) {
    case KSpreadValue::Empty:
      val.setValue (0.0);
    break;
    case KSpreadValue::Boolean:
      val.setValue (value.asBoolean() ? 1.0 : 0.0);
    break;
    case KSpreadValue::Integer:
      val.setValue (value.asFloat ());
    break;
    case KSpreadValue::Float:
      val = value;
    break;
    case KSpreadValue::String:
      val = parser()->tryParseNumber (value.asString(), &ok);
      if (!ok)
        val.setValue (0.0);
    break;
    case KSpreadValue::Array:
      val = asFloat (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (0.0);
    break;
  };
  
  return val;
}

KSpreadValue ValueConverter::asString (const KSpreadValue &value) const
{
  // This is a simpler version of ValueFormatter... We cannot use that one,
  // as we sometimes want to generate the string differently ...

  KSpreadValue val;
  QString s;
  KSpreadValue::Format fmt;
  QChar decimal_point;
  int pos;
  switch (value.type()) {
    case KSpreadValue::Empty:
      val = QString::null;
    break;
    case KSpreadValue::Boolean:
      val.setValue (value.asBoolean() ? locale()->translate ("True") :
          locale()->translate ("False"));
    break;
    case KSpreadValue::Integer:
    {
      fmt = value.format();
      if (fmt == KSpreadValue::fmt_Percent)
        val = QString::number (value.asInteger() * 100) + " %";
      else if (fmt == KSpreadValue::fmt_DateTime)
        val = locale()->formatDateTime (value.asDateTime());
      else if (fmt == KSpreadValue::fmt_Date)
        val = locale()->formatDate (value.asDate());
      else if (fmt == KSpreadValue::fmt_Time)
        val = locale()->formatTime (value.asTime());
      else
        val = QString::number (value.asInteger());
    }
    break;
    case KSpreadValue::Float:
      fmt = value.format();
      if (fmt == KSpreadValue::fmt_DateTime)
        val = locale()->formatDateTime (value.asDateTime());
      else if (fmt == KSpreadValue::fmt_Date)
        val = locale()->formatDate (value.asDate(), true);
      else if (fmt == KSpreadValue::fmt_Time)
        val = locale()->formatTime (value.asTime());
      else
      {
        //convert the number, change decimal point from English to local
        s = QString::number (value.asFloat(), 'g', 10);
        decimal_point = locale()->decimalSymbol()[0];
        if (decimal_point && ((pos = s.find ('.')) != -1))
          s = s.replace (pos, 1, decimal_point);
        if (fmt == KSpreadValue::fmt_Percent)
          s += " %";
        val.setValue (s);
      }
    break;
    case KSpreadValue::String:
      val = value;
    break;
    case KSpreadValue::Array:
      val = asString (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (value.errorMessage ());
    break;
  };
  
  return val;
}

KSpreadValue ValueConverter::asDateTime (const KSpreadValue &value) const
{
  KSpreadValue val;
  bool ok;
  
  switch (value.type()) {
    case KSpreadValue::Empty:
      val.setValue (QDateTime::currentDateTime());
    break;
    case KSpreadValue::Boolean:
      //ignore the bool value... any better idea? ;)
      val.setValue (QDateTime::currentDateTime());
    break;
    case KSpreadValue::Integer:
      val.setValue (value.asFloat());
      val.setFormat (KSpreadValue::fmt_DateTime);
    break;
    case KSpreadValue::Float:
      val.setValue (value.asFloat());
      val.setFormat (KSpreadValue::fmt_DateTime);
    break;
    case KSpreadValue::String:
      //no DateTime parser, so we parse as Date, hoping for the best ...
      val = parser()->tryParseDate (value.asString(), &ok);
      if (!ok)
        val.setValue (QDateTime::currentDateTime());
      val.setFormat (KSpreadValue::fmt_DateTime);
    break;
    case KSpreadValue::Array:
      val = asDateTime (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (QDateTime::currentDateTime());
    break;
  };
  
  return val;
}

KSpreadValue ValueConverter::asDate (const KSpreadValue &value) const
{
  KSpreadValue val;
  bool ok;
  
  switch (value.type()) {
    case KSpreadValue::Empty:
      val.setValue (QDate::currentDate());
    break;
    case KSpreadValue::Boolean:
      //ignore the bool value... any better idea? ;)
      val.setValue (QDate::currentDate());
    break;
    case KSpreadValue::Integer:
      val.setValue (value.asFloat());
      val.setFormat (KSpreadValue::fmt_Date);
    break;
    case KSpreadValue::Float:
      val.setValue (value.asFloat());
      val.setFormat (KSpreadValue::fmt_Date);
    break;
    case KSpreadValue::String:
      val = parser()->tryParseDate (value.asString(), &ok);
      if (!ok)
        val.setValue (QDate::currentDate());
    break;
    case KSpreadValue::Array:
      val = asDate (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (QDate::currentDate());
    break;
  };
  
  return val;
}

KSpreadValue ValueConverter::asTime (const KSpreadValue &value) const
{
  KSpreadValue val;
  bool ok;
  
  switch (value.type()) {
    case KSpreadValue::Empty:
      val.setValue (QTime::currentTime());
    break;
    case KSpreadValue::Boolean:
      //ignore the bool value... any better idea? ;)
      val.setValue (QTime::currentTime());
    break;
    case KSpreadValue::Integer:
      val.setValue (value.asFloat());
      val.setFormat (KSpreadValue::fmt_Time);
    break;
    case KSpreadValue::Float:
      val.setValue (value.asFloat());
      val.setFormat (KSpreadValue::fmt_Time);
    break;
    case KSpreadValue::String:
      val = parser()->tryParseTime (value.asString(), &ok);
      if (!ok)
        val.setValue (QTime::currentTime());
    break;
    case KSpreadValue::Array:
      val = asTime (value.element (0, 0));
    break;
    case KSpreadValue::CellRange:
      /* NOTHING */
    break;
    case KSpreadValue::Error:
      val.setValue (QTime::currentTime());
    break;
  };
  
  return val;
}

