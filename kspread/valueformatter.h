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

#ifndef KSPREAD_VALUEFORMATTER
#define KSPREAD_VALUEFORMATTER

#include "kspread_global.h"

#include <qdatetime.h>

class KSpreadCell;

namespace KSpread {


/**
The ValueFormatter class generates a textual representation of
data stored in a KSpreadValue, with a given formatting
It follows the Singleton pattern.
*/

class ValueFormatter {
 public:
   /** returns an instance of this class */
  static ValueFormatter *self ();
   /** destructor */
   ~ValueFormatter ();

  /** create a text representation of data in this cell */
  QString formatText (KSpreadCell *cell, FormatType fmtType);
 

  /** create a number format */
  QString createNumberFormat (KLocale *locale, double value, int precision,
      const QString &currencySymbol, FormatType fmt, bool alwaysSigned);
  
  /** create a date format */
  QString dateFormat (KLocale* locale, const QDate &_date, FormatType fmtType);
  
  /** create a time format */
  QString timeFormat (KLocale* locale, const QDateTime &_time,
      FormatType fmtType);
  
  /** create a fraction format */
  QString fractionFormat (double value, FormatType fmtType);
  
  QString errorFormat (KSpreadCell *cell);

 protected:
  ValueFormatter ();
  static ValueFormatter *_self;
};


};  //namespace KSpread


#endif  //KSPREAD_VALUEFORMATTER
