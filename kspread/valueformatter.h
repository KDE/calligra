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

#ifndef KSPREAD_VALUEFORMATTER
#define KSPREAD_VALUEFORMATTER

#include "kspread_format.h"

#include <QDateTime>

namespace KSpread
{
class Cell;
class Value;
class ValueConverter;

/**
The ValueFormatter class generates a textual representation of
data stored in a Value, with a given formatting.
*/

class ValueFormatter {
 public:
  /** copnstructor */
  ValueFormatter (ValueConverter *converter);

  /** create a text representation of data in this cell */
  QString formatText (Cell *cell, FormatType fmtType);

  /** create a text representation of data in this Value */
  QString formatText (const Value &value,
      FormatType fmtType, int precision = -1,
      Style::FloatFormat floatFormat = Style::OnlyNegSigned,
      const QString &prefix = QString::null,
      const QString &postfix = QString::null,
      const QString &currencySymbol = QString::null);

  /** create a date format */
  QString dateFormat (const QDate &_date, FormatType fmtType);

  /** create a time format */
  QString timeFormat (const QDateTime &_time, FormatType fmtType);

 protected:

  ValueConverter* converter;

  /** determine the formatting type that should be used to format this value
  in a cell with a given format type */
  FormatType determineFormatting (const Value &value,
      FormatType fmtType);

  /** create a number format */
  QString createNumberFormat (double value, int precision, FormatType fmt,
      bool alwaysSigned, const QString& currencySymbol);

  /** create a fraction format */
  QString fractionFormat (double value, FormatType fmtType);

  QString errorFormat (Cell *cell);

  /** Remove trailing zeros and the decimal point if necessary
  unless the number has no decimal point */
  void removeTrailingZeros (QString &str, QChar decimal_point);
};


}  //namespace KSpread


#endif  //KSPREAD_VALUEFORMATTER
