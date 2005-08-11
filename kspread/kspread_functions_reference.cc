/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// built-in reference functions

#include "kspread_cell.h"
#include "kspread_sheet.h"
#include "kspread_util.h"
#include "kspread_value.h"

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

using namespace KSpread;

// prototypes (sorted alphabetically)
KSpreadValue func_address (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_areas (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_choose (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_column (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_columns (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_indirect (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_lookup (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_row (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_rows (valVector args, ValueCalc *calc, FuncExtra *);

// registers all reference functions
void KSpreadRegisterReferenceFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;
  
  f = new Function ("ADDRESS",  func_address);
  f->setParamCount (2, 5);
  repo->add (f);
  f = new Function ("AREAS",    func_areas);
  f->setParamCount (1);
  f->setNeedsExtra (true);
  repo->add (f);
  f = new Function ("CHOOSE",   func_choose);
  f->setParamCount (2, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("COLUMN",   func_column);
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("COLUMNS",  func_columns);
  f->setParamCount (1);
  f->setAcceptArray ();
  f->setNeedsExtra (true);
  repo->add (f);
  f = new Function ("INDIRECT", func_indirect);
  f->setParamCount (1, 2);
  f->setNeedsExtra (true);
  repo->add (f);
  f = new Function ("LOOKUP",   func_lookup);
  f->setParamCount (3);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("ROW",      func_row);
  f->setParamCount (0, 1);
  repo->add (f);
  f = new Function ("ROWS",     func_rows);
  f->setParamCount (1);
  f->setAcceptArray ();
  f->setNeedsExtra (true);
  repo->add (f);
}

// Function: ADDRESS
KSpreadValue func_address (valVector args, ValueCalc *calc, FuncExtra *)
{
  bool r1c1 = false;
  QString sheetName;
  int absNum = 1;
  if (args.count() > 2)
    absNum = calc->conv()->asInteger (args[2]).asInteger();
  if (args.count() > 3)
    r1c1 = !(calc->conv()->asBoolean (args[3]).asBoolean());
  if (args.count() == 5)
    sheetName = calc->conv()->asString (args[4]).asString();

  QString result;
  int row = calc->conv()->asInteger (args[0]).asInteger();
  int col = calc->conv()->asInteger (args[1]).asInteger();

  if ( !sheetName.isEmpty() )
  {
    result += sheetName;
    result += "!";
  }

  if ( r1c1 )
  {
    // row first
    bool abs = false;
    if ( absNum == 1 || absNum == 2 )
      abs = true;

    result += 'R';
    if ( !abs )
      result += '[';
    result += QString::number( row );

    if ( !abs )
      result += ']';

    // column
    abs = false;
    if ( absNum == 1 || absNum == 3 )
      abs = true;

    result += 'C';
    if ( !abs )
      result += '[';
    result += QString::number( col );

    if ( !abs )
      result += ']';
  }
  else
  {
    bool abs = false;
    if ( absNum == 1 || absNum == 3 )
      abs = true;

    if ( abs )
      result += '$';

    result += KSpreadCell::columnName( col );

    abs = false;
    if ( absNum == 1 || absNum == 2 )
      abs = true;

    if ( abs )
      result += '$';

    result += QString::number( row );
  }
  
  return KSpreadValue (result);
}

bool checkRef( QString const & ref )
{
  KSpreadRange r( ref );
  if ( !r.isValid() )
  {
    KSpreadPoint p( ref );
    if ( !p.isValid() )
      return false;
  }
  return true;
}

// Function: AREAS
KSpreadValue func_areas (valVector args, ValueCalc *calc, FuncExtra *e)
{
  if (e) {
    if ((e->ranges[0].col1 != -1) && (e->ranges[0].row1 != -1) &&
        (e->ranges[0].col2 != -1) && (e->ranges[0].row2 != -1))
      // we have a range reference - return 1
      return 1;
  }
  
  QString s = calc->conv()->asString (args[0]).asString();
  if ( s[0] != '(' || s[s.length() - 1] != ')' )
    return KSpreadValue::errorVALUE();

  int l = s.length();

  int num = 0;
  QString ref;
  for ( int i = 1; i < l; ++i )
  {
    if ( s[i] == ',' || s[i] == ')' )
    {
      if ( !checkRef( ref ) )
        return KSpreadValue::errorVALUE();
      else
      {
        ++num;
        ref = "";
      }
    }
    else
      ref += s[i];
  }

  return KSpreadValue (num);
}

// Function: CHOOSE
KSpreadValue func_choose (valVector args, ValueCalc *calc, FuncExtra *)
{
  int cnt = args.count () - 1;
  int num = calc->conv()->asInteger (args[0]).asInteger();
  if ((num <= 0) || (num > cnt))
    return KSpreadValue::errorVALUE();
  return args[num];
}

// Function: LOOKUP
KSpreadValue func_lookup (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue num = calc->conv()->asNumeric (args[0]);
  if (num.isArray())
    return KSpreadValue::errorVALUE();
  KSpreadValue lookup = args[1];
  KSpreadValue rr = args[2];
  unsigned cols = lookup.columns();
  unsigned rows = lookup.rows();
  if ((cols != rr.columns()) || (rows != rr.rows()))
    return KSpreadValue::errorVALUE();
  KSpreadValue res;
  
  // now traverse the array and perform comparison
  for (unsigned r = 0; r < rows; ++r)
    for (unsigned c = 0; c < cols; ++c)
    {
      // update the result, return if we cross the line
      KSpreadValue le = lookup.element (c, r);
      if (calc->lower (lookup, le) || calc->equal (lookup, le))
        res = rr.element (c, r);
      else
        return res;
    }
  return res;
}

// Function: COLUMN
KSpreadValue func_column (valVector args, ValueCalc *, FuncExtra *e)
{
  int col = e ? e->mycol : 0;
  if (e && args.count())
    col = e->ranges[0].col1;
  if (col > 0)
    return KSpreadValue (col);
  return KSpreadValue::errorVALUE();
}

// Function: ROW
KSpreadValue func_row (valVector args, ValueCalc *, FuncExtra *e)
{
  int row = e ? e->myrow : 0;
  if (e && args.count())
    row = e->ranges[0].row1;
  if (row > 0)
    return KSpreadValue (row);
  return KSpreadValue::errorVALUE();
}

// Function: COLUMNS
KSpreadValue func_columns (valVector, ValueCalc *, FuncExtra *e)
{
  int col1 = e->ranges[0].col1;
  int col2 = e->ranges[0].col2;
  if ((col1 == -1) || (col2 == -1))
    return KSpreadValue::errorVALUE();
  return KSpreadValue (col2 - col1 + 1);
}

// Function: ROWS
KSpreadValue func_rows (valVector, ValueCalc *, FuncExtra *e)
{
  int row1 = e->ranges[0].row1;
  int row2 = e->ranges[0].row2;
  if ((row1 == -1) || (row2 == -1))
    return KSpreadValue::errorVALUE();
  return KSpreadValue (row2 - row1 + 1);
}


// Function: INDIRECT
KSpreadValue func_indirect (valVector args, ValueCalc *calc, FuncExtra *e)
{
  bool r1c1 = false;
  QString ref = calc->conv()->asString (args[4]).asString();
  if (args.count() == 2)
    r1c1 = !(calc->conv()->asBoolean (args[1]).asBoolean());

  if (ref.isEmpty())
    return KSpreadValue::errorVALUE();

  if ( r1c1 )
  {
    // TODO: translate the r1c1 style to a1 style
    ref = ref;
  }

  KSpreadPoint p (ref, e->sheet->workbook(), e->sheet);

  if ( !p.isValid() )
    return KSpreadValue::errorVALUE();

  KSpreadCell * cell = p.cell();
  if (cell)
    return cell->value();
  return KSpreadValue::errorVALUE();
}

