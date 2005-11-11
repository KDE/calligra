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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// built-in logical functions

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

using namespace KSpread;

// prototypes (sorted alphabetically)
Value func_and (valVector args, ValueCalc *calc, FuncExtra *);
Value func_false (valVector args, ValueCalc *calc, FuncExtra *);
Value func_if (valVector args, ValueCalc *calc, FuncExtra *);
Value func_nand (valVector args, ValueCalc *calc, FuncExtra *);
Value func_nor (valVector args, ValueCalc *calc, FuncExtra *);
Value func_not (valVector args, ValueCalc *calc, FuncExtra *);
Value func_or (valVector args, ValueCalc *calc, FuncExtra *);
Value func_true (valVector args, ValueCalc *calc, FuncExtra *);
Value func_xor (valVector args, ValueCalc *calc, FuncExtra *);

// registers all logic functions
void RegisterLogicFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("FALSE", func_false);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("TRUE", func_true);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("NOT", func_not);
  f->setParamCount (1);
  repo->add (f);
  f = new Function ("AND", func_and);
  f->setParamCount (2, -1);
  repo->add (f);
  f = new Function ("NAND", func_nand);
  f->setParamCount (2, -1);
  repo->add (f);
  f = new Function ("NOR", func_nor);
  f->setParamCount (2, -1);
  repo->add (f);
  f = new Function ("OR", func_or);
  f->setParamCount (2, -1);
  repo->add (f);
  f = new Function ("XOR", func_xor);
  f->setParamCount (2, -1);
  repo->add (f);
  f = new Function ("IF", func_if);
  f->setParamCount (3);
  repo->add (f);
}

// Function: FALSE
Value func_false (valVector, ValueCalc *, FuncExtra *)
{
  return Value (false);
}

// Function: TRUE
Value func_true (valVector, ValueCalc *, FuncExtra *)
{
  return Value (true);
}

// helper for most logical functions
bool asBool (Value val, ValueCalc *calc)
{
  return calc->conv()->asBoolean (val).asBoolean ();
}

// Function: NOT
Value func_not (valVector args, ValueCalc *calc, FuncExtra *)
{
  bool val = asBool (args[0], calc) ? false : true;
  return Value (val);
}

// Function: OR
Value func_or (valVector args, ValueCalc *calc, FuncExtra *)
{
  int cnt = args.count();
  for (int i = 0; i < cnt; ++i)
    if (asBool (args[i], calc))
      // if any value is true, return true
      return Value (true);
  // nothing is true -> return false
  return Value (false);
}

// Function: NOR
Value func_nor (valVector args, ValueCalc *calc, FuncExtra *)
{
  // OR in reverse
  int cnt = args.count();
  for (int i = 0; i < cnt; ++i)
    if (asBool (args[i], calc))
      // if any value is true, return false
      return Value (false);
  // nothing is true -> return true
  return Value (true);
}

// Function: AND
Value func_and (valVector args, ValueCalc *calc, FuncExtra *)
{
  int cnt = args.count();
  for (int i = 0; i < cnt; ++i)
    if (!asBool (args[i], calc))
      // if any value is false, return false
      return Value (false);
  // nothing is false -> return true
  return Value (true);
}

// Function: NAND
Value func_nand (valVector args, ValueCalc *calc, FuncExtra *)
{
  // AND in reverse
  int cnt = args.count();
  for (int i = 0; i < cnt; ++i)
    if (!asBool (args[i], calc))
      // if any value is false, return true
      return Value (true);
  // nothing is false -> return false
  return Value (false);
}

// Function: XOR
Value func_xor (valVector args, ValueCalc *calc, FuncExtra *)
{
  // exclusive OR - exactly one value must be true
  int cnt = args.count();
  int count = 0;
  for (int i = 0; i < cnt; ++i)
    if (asBool (args[i], calc))
      count++;
  return Value (count == 1);
}

// Function: IF
Value func_if (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (asBool (args[0], calc))
    return args[1];
  else
    return args[2];
}
