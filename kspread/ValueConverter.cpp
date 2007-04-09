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
   Boston, MA 02110-1301, USA.
*/

#include "Localization.h"
#include "ValueParser.h"

#include "ValueConverter.h"

using namespace KSpread;

ValueConverter::ValueConverter( const ValueParser* parser )
    : m_parser( parser )
{
}

const Doc* ValueConverter::doc() const
{
  return m_parser->doc();
}

const KLocale* ValueConverter::locale() const
{
  return m_parser->locale();
}

Value ValueConverter::asBoolean (const Value &value) const
{
  Value val;
  bool ok;
  switch (value.type()) {
    case Value::Empty:
      val = Value(false);
    break;
    case Value::Boolean:
      val = value;
    break;
    case Value::Integer:
      val = Value(value.asInteger() ? true : false);
    break;
    case Value::Float:
      val = Value((value.asFloat() == 0.0) ? false : true);
    break;
    case Value::Complex:
      val = Value((value.asComplex().real() == complex<double>( 0.0, 0.0 ) ) ? false : true);
    break;
    case Value::String:
      val = m_parser->tryParseBool (value.asString(), &ok);
      if (!ok)
        val = Value(false);
    break;
    case Value::Array:
      val = asBoolean (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val = Value(false);
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
      val = Value(0);
    break;
    case Value::Boolean:
      val = Value(value.asBoolean() ? 1 : 0);
    break;
    case Value::Integer:
      val = value;
    break;
    case Value::Float:
      val = Value(value.asInteger());
    break;
    case Value::Complex:
      val = Value(value.asInteger());
    break;
    case Value::String:
      val = Value((int) m_parser->tryParseNumber
          (value.asString(), &ok).asFloat());
      if (!ok)
        val = Value(0);
    break;
    case Value::Array:
      val = asInteger (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val = Value(0);
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
      val = Value(0.0);
    break;
    case Value::Boolean:
      val = Value(value.asBoolean() ? 1.0 : 0.0);
    break;
    case Value::Integer:
      val = Value(value.asFloat ());
    break;
    case Value::Float:
      val = value;
    break;
    case Value::Complex:
      val = Value(value.asFloat());
    break;
    case Value::String:
      val = m_parser->tryParseNumber (value.asString(), &ok);
      if (!ok)
        val = Value(0.0);
    break;
    case Value::Array:
      val = asFloat (value.element (0, 0));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val = Value(0.0);
    break;
  };

  return val;
}

Value ValueConverter::asComplex( const Value &value ) const
{
    Value val;
    bool ok;

    switch (value.type())
    {
        case Value::Empty:
            val = Value( complex<double>( 0.0, 0.0 ) );
            break;
        case Value::Boolean:
            val = Value( complex<double>( value.asBoolean() ? 1.0 : 0.0, 0.0 ) );
            break;
        case Value::Integer:
        case Value::Float:
            val = Value( complex<double>( value.asFloat(), 0.0 ) );
            break;
        case Value::Complex:
            val = value;
            break;
        case Value::String:
            val = m_parser->tryParseNumber (value.asString(), &ok);
            if (!ok)
                val = Value( complex<double>( 0.0, 0.0 ) );
            break;
        case Value::Array:
            val = asComplex( value.element( 0, 0 ) );
            break;
        case Value::CellRange:
            /* NOTHING */
            break;
        case Value::Error:
            val = Value( complex<double>( 0.0, 0.0 ) );
            break;
    };

    return val;
}

Value ValueConverter::asNumeric (const Value &value) const
{
    Value val;
    bool ok;

    switch (value.type())
    {
        case Value::Empty:
            val = Value(0.0);
            break;
        case Value::Boolean:
            val = Value(value.asBoolean() ? 1.0 : 0.0);
            break;
        case Value::Integer:
        case Value::Float:
        case Value::Complex:
            val = value;
            break;
        case Value::String:
            val = m_parser->tryParseNumber (value.asString(), &ok);
            if (!ok)
                val = Value(0.0);
            break;
        case Value::Array:
            val = asNumeric( value.element( 0, 0 ) );
            break;
        case Value::CellRange:
            /* NOTHING */
            break;
        case Value::Error:
            val = Value(0.0);
            break;
    };

    return val;
}

Value ValueConverter::asString (const Value &value) const
{
  // This is a simpler version of ValueFormatter... We cannot use that one,
  // as we sometimes want to generate the string differently ...

  Value val;
  QString s;
  Value::Format fmt;
  int pos;
  switch (value.type()) {
    case Value::Empty:
      val = Value(QString());
    break;
    case Value::Boolean:
      val = Value(value.asBoolean() ? ki18n("True").toString(m_parser->locale()) :
        ki18n("False").toString(m_parser->locale()));
    break;
    case Value::Integer:
    {
      fmt = value.format();
      if (fmt == Value::fmt_Percent)
        val = Value(QString::number (value.asInteger() * 100) + " %");
      else if (fmt == Value::fmt_DateTime)
        val = Value(m_parser->locale()->formatDateTime (value.asDateTime( doc() )));
      else if (fmt == Value::fmt_Date)
        val = Value(m_parser->locale()->formatDate (value.asDate( doc() )));
      else if (fmt == Value::fmt_Time)
        val = Value(m_parser->locale()->formatTime (value.asTime( doc() )));
      else
        val = Value(QString::number (value.asInteger()));
    }
    break;
    case Value::Float:
      fmt = value.format();
      if (fmt == Value::fmt_DateTime)
        val = Value(m_parser->locale()->formatDateTime (value.asDateTime( doc() )));
      else if (fmt == Value::fmt_Date)
        val = Value(m_parser->locale()->formatDate (value.asDate( doc() ), KLocale::ShortDate));
      else if (fmt == Value::fmt_Time)
        val = Value(m_parser->locale()->formatTime (value.asTime( doc() )));
      else
      {
        //convert the number, change decimal point from English to local
        s = QString::number (value.asFloat(), 'g', 10);
        const QString decimalSymbol = m_parser->locale()->decimalSymbol();
        if (!decimalSymbol.isNull() && ((pos = s.indexOf('.')) != -1))
          s = s.replace( pos, 1, decimalSymbol );
        if (fmt == Value::fmt_Percent)
          s += " %";
        val = Value(s);
      }
    break;
    case Value::Complex:
      fmt = value.format();
      if (fmt == Value::fmt_DateTime)
        val = Value(m_parser->locale()->formatDateTime (value.asDateTime( doc() )));
      else if (fmt == Value::fmt_Date)
        val = Value(m_parser->locale()->formatDate (value.asDate( doc() ), KLocale::ShortDate));
      else if (fmt == Value::fmt_Time)
        val = Value(m_parser->locale()->formatTime (value.asTime( doc() )));
      else
      {
        //convert the number, change decimal point from English to local
        const QString decimalSymbol = m_parser->locale()->decimalSymbol();
        QString real = QString::number (value.asComplex().real(), 'g', 10);
        if (!decimalSymbol.isNull() && ((pos = real.indexOf('.')) != -1))
            real = real.replace( pos, 1, decimalSymbol );
        QString imag = QString::number (value.asComplex().imag(), 'g', 10);
        if (!decimalSymbol.isNull() && ((pos = imag.indexOf('.')) != -1))
            imag = imag.replace( pos, 1, decimalSymbol );
        s = real;
        if ( value.asComplex().imag() >= 0.0 )
            s += '+';
        // TODO Stefan: Some prefer 'j'. Configure option? Spec?
        s += imag + 'i';
        // NOTE Stefan: Never recognized a complex percentage anywhere. ;-)
//         if (fmt == Value::fmt_Percent)
//           s += " %";
        val = Value(s);
      }
    break;
    case Value::String:
      val = value;
    break;
    case Value::Array:
      val = Value(asString (value.element (0, 0)));
    break;
    case Value::CellRange:
      /* NOTHING */
    break;
    case Value::Error:
      val = Value(value.errorMessage ());
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
      val = Value( QDateTime::currentDateTime(), doc() );
    break;
    case Value::Boolean:
      //ignore the bool value... any better idea? ;)
      val = Value( QDateTime::currentDateTime(), doc() );
    break;
    case Value::Integer:
    case Value::Float:
    case Value::Complex:
      val = Value(value.asFloat());
      val.setFormat (Value::fmt_DateTime);
    break;
    case Value::String:
      //no DateTime m_parser, so we parse as Date, hoping for the best ...
      val = m_parser->tryParseDate (value.asString(), &ok);
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
      val = Value( QDate::currentDate(), doc() );
    break;
    case Value::Boolean:
      //ignore the bool value... any better idea? ;)
      val = Value( QDate::currentDate(), doc() );
    break;
    case Value::Integer:
    case Value::Float:
    case Value::Complex:
      val = Value(value.asFloat());
      val.setFormat (Value::fmt_Date);
    break;
    case Value::String:
      val = m_parser->tryParseDate (value.asString(), &ok);
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
      val = Value( QTime::currentTime(), doc() );
    break;
    case Value::Boolean:
      //ignore the bool value... any better idea? ;)
      val = Value( QTime::currentTime(), doc() );
    break;
    case Value::Integer:
    case Value::Float:
    case Value::Complex:
      val = Value(value.asFloat());
      val.setFormat (Value::fmt_Time);
    break;
    case Value::String:
      val = m_parser->tryParseTime (value.asString(), &ok);
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
