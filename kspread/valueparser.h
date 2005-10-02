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

#ifndef KSPREAD_VALUEPARSER
#define KSPREAD_VALUEPARSER

#include <qdatetime.h>

#include "kspread_global.h"
#include "kspread_value.h"

class KSpreadCell;
class KLocale;

namespace KSpread {


/**
The ValueParser parses a text input from the user, generating
KSpreadValue in the desired format.
*/

class ValueParser {
 public:
  /** constructor */
  ValueParser (KLocale *locale);
  
  KLocale* locale();
  
  /** try to parse the text in a given cell and set value accordingly */
  void parse (const QString& str, KSpreadCell *cell);

  /** try to parse given text, don't set any cell attributes though */
  KSpreadValue parse (const QString &str);
  
  KSpreadValue tryParseBool (const QString& str, bool *ok = 0);
  KSpreadValue tryParseNumber (const QString& str, bool *ok = 0);
  KSpreadValue tryParseDate (const QString& str, bool *ok = 0);
  KSpreadValue tryParseTime (const QString& str, bool *ok = 0);
 protected:
 
  KLocale* parserLocale;

  // Try to parse the text as a bool/number/date/time/etc.
  // Helpers for parse.
  bool tryParseBool (const QString& str, KSpreadCell *cell);
  bool tryParseNumber (const QString& str, KSpreadCell *cell);
  bool tryParseDate (const QString& str, KSpreadCell *cell);
  bool tryParseTime (const QString& str, KSpreadCell *cell);
  
  /** converts a string to a date/time value */
  QDateTime readTime (const QString & intstr, bool withSeconds, bool *ok,
      bool & duration);

  /** a helper function to read integers */
  int readInt (const QString &str, uint &pos);
  FormatType fmtType;
};


}  //namespace KSpread


#endif  //KSPREAD_VALUEPARSER

