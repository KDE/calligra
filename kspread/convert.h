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

#ifndef KSPREAD_CONVERT
#define KSPREAD_CONVERT

#include "kspread_value.h"

class KLocale;

/*
This file contains two classes, the former of them can convert between
various data types of KSpreadValue, the latter can generate a textual
representation of data in KSpreadValue with a given formatting.

Methods contained here were originally scattered around kspread_cell.*,
kspread_util.*, formula.* and probably other places as well

*/

// KSpread namespace
namespace KSpread {

/**
The Convert class contains various methods used to convert between
various data types of KSpreadValue. Things like number to string,
string to bool, and so on */

class Convert {
 public:
  static KSpreadValue toBool (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toInteger (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toFloat (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toString (const KSpreadValue &val, KLocale *locale);
  static KSpreadValue toDateTime (const KSpreadValue &val, KLocale *locale);
};


/** the ValueParser parses a text input from the user, generating
KSpreadValue in the desired format */

class ValueParser {
  //TODO: copy stuff from KSpreadCell and kspread_util.*
};

/** The ValueFormatter class generates a textual representation of
data stored in a KSpreadValue, with a given formatting */

class ValueFormatter {
  //TODO: copy stuff from KSpreadCell and kspread_util.*
};


};  //end of KSpread namespace

#endif //KSPREAD_CONVERT

