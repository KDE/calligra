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

#ifndef KSPREAD_VALUECONVERTER
#define KSPREAD_VALUECONVERTER

class KLocale;

#include "Value.h"

namespace KSpread {

class ValueParser;

/**
The ValueConverter class can convert Value between various different
formats.
*/

class ValueConverter {
 public:
  /** constructor */
  ValueConverter (ValueParser *parser);
 
  Value asBoolean (const Value &value) const;
  Value asInteger (const Value &value) const;
  Value asFloat (const Value &value) const;
  /** convert to a numeric value - not necessarily float */
  Value asNumeric (const Value &value) const;
  Value asString (const Value &value) const;
  Value asDateTime (const Value &value) const;
  Value asDate (const Value &value) const;
  Value asTime (const Value &value) const;
  
  KLocale* locale();
  
 private:
   ValueParser *parser; 
};


}  //KSpread namespace

#endif  //KSPREAD_VALUECONVERTER

