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


#include "convert.h"

#include "kspread_locale.h"

using namespace KSpread;

KSpreadValue Convert::toBool (const KSpreadValue &val, KLocale *locale)
{
  bool res = false;
  switch (val.type()) {
    case KSpreadValue::Empty:
      res = false;
    break;
    case KSpreadValue::Boolean:
      res = val.asBoolean();
    break;
    case KSpreadValue::Integer:
      res = (val.asInteger() != 0);
    break;
    case KSpreadValue::Float:
      res = (val.asFloat() != 0);
    break;
    case KSpreadValue::String:
    {
      if ((val.asString().lower() == "true") ||
          (val.asString().lower() == locale->translate("true")))
        res = true;
      else
        res = false;
    }
    break;
    case KSpreadValue::Array:
      //TODO: maybe convert to array of values of desired type?
      res = true;
    break;
    case KSpreadValue::CellRange:
      //TODO: what to do here?
      res = false;
    break;
    case KSpreadValue::Error:
      res = false;
    break;
  };
  
  return KSpreadValue (res);
}

KSpreadValue Convert::toInteger (const KSpreadValue &val, KLocale *locale)
{
  long res = 0;
  switch (val.type()) {
    case KSpreadValue::Empty:
      res = 0;
    break;
    case KSpreadValue::Boolean:
      res = val.asBoolean() ? 1 : 0;
    break;
    case KSpreadValue::Integer:
      res = val.asInteger();
    break;
    case KSpreadValue::Float:
      res = (long) val.asFloat();
    break;
    case KSpreadValue::String:
    {
      bool ok;
      res = (long) locale->readNumber (val.asString(), &ok);
      if (!ok) res = 0;
    }
    break;
    case KSpreadValue::Array:
      //TODO: maybe convert to array of values of desired type?
      res = 0;
    break;
    case KSpreadValue::CellRange:
      //TODO: what to do here?
      res = 0;
    break;
    case KSpreadValue::Error:
      res = 0;
    break;
  };
  
  return KSpreadValue (res);
}

KSpreadValue Convert::toFloat (const KSpreadValue &val, KLocale *locale)
{
  double res = 0.0;
  switch (val.type()) {
    case KSpreadValue::Empty:
      res = 0.0;
    break;
    case KSpreadValue::Boolean:
      res = val.asBoolean() ? 1.0 : 0.0;
    break;
    case KSpreadValue::Integer:
      res = val.asInteger();
    break;
    case KSpreadValue::Float:
      res = val.asFloat();
    break;
    case KSpreadValue::String:
    {
      bool ok;
      res = locale->readNumber (val.asString(), &ok);
      if (!ok) res = 0.0;
    }
    break;
    case KSpreadValue::Array:
      //TODO: maybe convert to array of values of desired type?
      res = 0.0;
    break;
    case KSpreadValue::CellRange:
      //TODO: what to do here?
      res = 0.0;
    break;
    case KSpreadValue::Error:
      res = 0.0;
    break;
  };
  
  return KSpreadValue (res);
}

KSpreadValue Convert::toString (const KSpreadValue &val, KLocale *locale)
{
  QString res;
  switch (val.type()) {
    case KSpreadValue::Empty:
      res = QString::null;
    break;
    case KSpreadValue::Boolean:
      res = val.asBoolean() ? locale->translate ("true") :
          locale->translate ("false");
    break;
    case KSpreadValue::Integer:
      res = locale->formatLong (val.asInteger());
    break;
    case KSpreadValue::Float:
      res = locale->formatNumber (val.asFloat());
    break;
    case KSpreadValue::String:
      res = val.asString();
    break;
    case KSpreadValue::Array:
      //TODO: maybe convert to array of values of desired type?
      res = QString::null;
    break;
    case KSpreadValue::CellRange:
      //TODO: what to do here?
      res = QString::null;
    break;
    case KSpreadValue::Error:
      res = QString::null;
    break;
  };
  
  return KSpreadValue (res);
}

KSpreadValue Convert::toDateTime (const KSpreadValue &val, KLocale *locale)
{
  //for everything except string, we use the float convertor
  if (val.type() != KSpreadValue::String)
    return toFloat (val, locale);
  //TODO: maybe convert arrays to arrays?
  
  //strings are special - we need to parse date/time
  //TODO... [note that KLocale can't parse DateTime both at once]
}

