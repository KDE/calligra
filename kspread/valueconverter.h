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

#ifndef KSPREAD_VALUECONVERTER
#define KSPREAD_VALUECONVERTER

class KLocale;

#include "kspread_value.h"

namespace KSpread {

class ValueParser;

/**
The ValueConverter class can convert KSpreadValue between various different
formats.
*/

class ValueConverter {
 public:
  /** constructor */
  ValueConverter (ValueParser *parser);
 
  KSpreadValue asBoolean (const KSpreadValue &value) const;
  KSpreadValue asInteger (const KSpreadValue &value) const;
  KSpreadValue asFloat (const KSpreadValue &value) const;
  KSpreadValue asString (const KSpreadValue &value) const;
  KSpreadValue asDateTime (const KSpreadValue &value) const;
  KSpreadValue asDate (const KSpreadValue &value) const;
  KSpreadValue asTime (const KSpreadValue &value) const;
  
  KLocale* locale();
  
 private:
   ValueParser *parser; 
};


}  //KSpread namespace

#endif  //KSPREAD_VALUECONVERTER

