/* This file is part of the KDE project
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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

#ifndef KSPREAD_VALUECALC
#define KSPREAD_VALUECALC

#include "docbase.h"
#include "kspread_value.h"

namespace KSpread {

/**
The ValueCalc class is used to perform all sorts of calculations.
No other means of calculation should be performed, to achieve
transparency, and to ease addition of new datatypes.

Of course, for some functions, it might be impossible to apply them
on all datatypes, but since all of them can be applied on both
doubles and GnuMP-based numbers, that is not of much concern ;)
*/

class ValueCalc : public DocBase {
 public:
  ValueCalc (DocInfo *docinfo);

  /** basic arithmetic operations */
  KSpreadValue add (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue sub (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue mul (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue div (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue pow (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue add (const KSpreadValue &a, double b);
  KSpreadValue sub (const KSpreadValue &a, double b);
  KSpreadValue mul (const KSpreadValue &a, double b);
  KSpreadValue div (const KSpreadValue &a, double b);
  KSpreadValue pow (const KSpreadValue &a, double b);

  /** logarithms */
  KSpreadValue log (const KSpreadValue &number, const KSpreadValue &base);
  KSpreadValue log (const KSpreadValue &number, double base = 10);
  KSpreadValue ln (const KSpreadValue &number);

  /** basic range functions */
  KSpreadValue sum (const KSpreadValue &range);
  int count (const KSpreadValue &range);
  KSpreadValue avg (const KSpreadValue &range);
  KSpreadValue max (const KSpreadValue &range);
  KSpreadValue min (const KSpreadValue &range);

 protected:
  /** return result formatting, based on these two values */
  KSpreadValue::Format format (KSpreadValue::Format a, KSpreadValue::Format b);
};

}  //namespace KSpread


#endif // KSPREAD_VALUECALC

