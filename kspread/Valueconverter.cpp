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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "Valueconverter.h"

#include "Locale.h"
#include "Valueparser.h"

using namespace KSpread;

ValueConverter::ValueConverter (ValueParser* p) : parser( p )
{
}

KLocale* ValueConverter::locale()
{
  return parser->locale();
}

Value ValueConverter::asBoolean (const Value &value) const
{
  Value val;
  bool ok;
  switch (value.type()) {
    case Value::Empty:
      val.setValue (false);
    break;
    case Value::Boolean:
      val = value;
    break;
    case Value::Integer:
      val.setValue (value.asInteger() ? true : false);
    break;
    case Value::Float:
      val.setValue ((value.asFloat() == 0.0) ? false : true);
    break;
    case Value::String:
      val = parser->tryParseBool (value.asString(), &ok);
      if (!ok)
        val.setValue (false);
    break;
    case Value::Array:
      val = asBoolean (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val.setValue (false);
    break;
  };

  return val;
}

Value ValueConverter::asInteger (const Value &value) const
{
  Value val;
  bool ok;

  switch (value.type()) {
    case Value::Empty:
      val.setValue (0);
    break;
    case Value::Boolean:
      val.setValue (value.asBoolean() ? 1 : 0);
    break;
    case Value::Integer:
      val = value;
    break;
    case Value::Float:
      val.setValue (value.asInteger());
    break;
    case Value::String:
      val.setValue ((int) parser->tryParseNumber
          (value.asString(), &ok).asFloat());
      if (!ok)
        val.setValue (0);
    break;
    case Value::Array:
      val = asInteger (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val.setValue (0);
    break;
  };

  return val;
}

Value ValueConverter::asFloat (const Value &value) const
{
  Value val;
  bool ok;

  switch (value.type()) {
    case Value::Empty:
      val.setValue (0.0);
    break;
    case Value::Boolean:
      val.setValue (value.asBoolean() ? 1.0 : 0.0);
    break;
    case Value::Integer:
      val.setValue (value.asFloat ());
    break;
    case Value::Float:
      val = value;
    break;
    case Value::String:
      val = parser->tryParseNumber (value.asString(), &ok);
      if (!ok)
        val.setValue (0.0);
    break;
    case Value::Array:
      val = asFloat (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val.setValue (0.0);
    break;
  };

  return val;
}

Value ValueConverter::asNumeric (const Value &value) const
{
  // as of now, we simply go to float; later on, further datatypes
  // may be possible as a result of this
  return asFloat (value);
}

Value ValueConverter::asString (const Value &value) const
{
  // This is a simpler version of ValueFormatter... We cannot use that one,
  // as we sometimes want to generate the string differently ...

  Value val;
  QString s;
  Value::Format fmt;
  QChar decimal_point;
  int pos;
  switch (value.type()) {
    case Value::Empty:
      val = QString();
    break;
    case Value::Boolean:
      val.setValue (value.asBoolean() ? ki18n("True").toString(parser->locale()) :
        ki18n("False").toString(parser->locale()));
    break;
    case Value::Integer:
    {
      fmt = value.format();
      if (fmt == Value::fmt_Percent)
        val = QString::number (value.asInteger() * 100) + " %";
      else if (fmt == Value::fmt_DateTime)
        val = parser->locale()->formatDateTime (value.asDateTime());
      else if (fmt == Value::fmt_Date)
        val = parser->locale()->formatDate (value.asDate());
      else if (fmt == Value::fmt_Time)
        val = parser->locale()->formatTime (value.asTime());
      else
        val = QString::number (value.asInteger());
    }
    break;
    case Value::Float:
      fmt = value.format();
      if (fmt == Value::fmt_DateTime)
        val = parser->locale()->formatDateTime (value.asDateTime());
      else if (fmt == Value::fmt_Date)
        val = parser->locale()->formatDate (value.asDate(), true);
      else if (fmt == Value::fmt_Time)
        val = parser->locale()->formatTime (value.asTime());
      else
      {
        //convert the number, change decimal point from English to local
        s = QString::number (value.asFloat(), 'g', 10);
        decimal_point = parser->locale()->decimalSymbol()[0];
        if (!decimal_point.isNull() && ((pos = s.indexOf('.')) != -1))
          s = s.replace (pos, 1, decimal_point);
        if (fmt == Value::fmt_Percent)
          s += " %";
        val.setValue (s);
      }
    break;
    case Value::String:
      val = value;
    break;
    case Value::Array:
      val = asString (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val.setValue (value.errorMessage ());
    break;
  };

  return val;
}

Value ValueConverter::asDateTime (const Value &value) const
{
  Value val;
  bool ok;

  switch (value.type()) {
    case Value::Empty:
      val.setValue (QDateTime::currentDateTime());
    break;
    case Value::Boolean:
      //ignore the bool value... any better idea? ;)
      val.setValue (QDateTime::currentDateTime());
    break;
    case Value::Integer:
      val.setValue (value.asFloat());
      val.setFormat (Value::fmt_DateTime);
    break;
    case Value::Float:
      val.setValue (value.asFloat());
      val.setFormat (Value::fmt_DateTime);
    break;
    case Value::String:
      //no DateTime parser, so we parse as Date, hoping for the best ...
      val = parser->tryParseDate (value.asString(), &ok);
      if (!ok)
        val = Value::errorVALUE();
      val.setFormat (Value::fmt_DateTime);
    break;
    case Value::Array:
      val = asDateTime (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
    break;
  };

  return val;
}

Value ValueConverter::asDate (const Value &value) const
{
  Value val;
  bool ok;

  switch (value.type()) {
    case Value::Empty:
      val.setValue (QDate::currentDate());
    break;
    case Value::Boolean:
      //ignore the bool value... any better idea? ;)
      val.setValue (QDate::currentDate());
    break;
    case Value::Integer:
      val.setValue (value.asFloat());
      val.setFormat (Value::fmt_Date);
    break;
    case Value::Float:
      val.setValue (value.asFloat());
      val.setFormat (Value::fmt_Date);
    break;
    case Value::String:
      val = parser->tryParseDate (value.asString(), &ok);
      if (!ok)
        val = Value::errorVALUE();
    break;
    case Value::Array:
      val = asDate (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
    break;
  };

  return val;
}

Value ValueConverter::asTime (const Value &value) const
{
  Value val;
  bool ok;

  switch (value.type()) {
    case Value::Empty:
      val.setValue (QTime::currentTime());
    break;
    case Value::Boolean:
      //ignore the bool value... any better idea? ;)
      val.setValue (QTime::currentTime());
    break;
    case Value::Integer:
      val.setValue (value.asInteger());
      val.setFormat (Value::fmt_Time);
    break;
    case Value::Float:
      val.setValue (value.asFloat());
      val.setFormat (Value::fmt_Time);
    break;
    case Value::String:
      val = parser->tryParseTime (value.asString(), &ok);
      if (!ok)
        val = Value::errorVALUE();
    break;
    case Value::Array:
      val = asTime (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
    break;
  };

  return val;
}

