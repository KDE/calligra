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

// built-in trigonometric functions

#include "functions.h"
#include "valuecalc.h"

using namespace KSpread;

// prototypes (sort alphabetically)
KSpreadValue func_acos (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_acosh (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_acot (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_asinh (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_asin (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_atan (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_atan2 (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_atanh (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_cos (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_cosh (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_degrees (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_radians (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sin (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sinh (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_tan (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_tanh (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_pi (valVector args, ValueCalc *calc, FuncExtra *);

// registers all trigonometric functions
void KSpreadRegisterTrigFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("ACOS",   func_acos);
  repo->add (f);
  f = new Function ("ACOSH",  func_acosh);
  repo->add (f);
  f = new Function ("ACOT",   func_acot);
  repo->add (f);
  f = new Function ("ASIN",   func_asin);
  repo->add (f);
  f = new Function ("ASINH",  func_asinh);
  repo->add (f);
  f = new Function ("ATAN",   func_atan);
  repo->add (f);
  f = new Function ("ATAN2",  func_atan2);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("ATANH",  func_atanh);
  repo->add (f);
  f = new Function ("COS",    func_cos);
  repo->add (f);
  f = new Function ("COSH",   func_cosh);
  repo->add (f);
  f = new Function ("DEGREES",func_degrees);
  repo->add (f);
  f = new Function ("RADIANS",func_radians);
  repo->add (f);
  f = new Function ("SIN",    func_sin);
  repo->add (f);
  f = new Function ("SINH",   func_sinh);
  repo->add (f);
  f = new Function ("TAN",    func_tan);
  repo->add (f);
  f = new Function ("TANH",   func_tanh);
  repo->add (f);
  f = new Function ("PI",     func_pi);
  repo->add (f);
}

// Function: sin
KSpreadValue func_sin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->sin (args[0]);
}

// Function: cos
KSpreadValue func_cos (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->cos (args[0]);
}

// Function: tan
KSpreadValue func_tan (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->tg (args[0]);
}

// Function: atan
KSpreadValue func_atan (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->atg (args[0]);
}

// Function: asin
KSpreadValue func_asin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->asin (args[0]);
}

// Function: acos
KSpreadValue func_acos (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->acos (args[0]);
}

KSpreadValue func_acot (valVector args, ValueCalc *calc, FuncExtra *)
{
  // PI/2 - atg (val)
  return calc->sub (calc->div (calc->pi(), 2), calc->atg (args[0]));
}

// Function: asinh
KSpreadValue func_asinh (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->asinh (args[0]);
}

// Function: acosh
KSpreadValue func_acosh (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->acosh (args[0]);
}

// Function: atanh
KSpreadValue func_atanh (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->atgh (args[0]);
}

// Function: tanh
KSpreadValue func_tanh (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->tgh (args[0]);
}

// Function: sinh
KSpreadValue func_sinh (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->sinh (args[0]);
}

// Function: cosh
KSpreadValue func_cosh (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->cosh (args[0]);
}

// Function: DEGREES
KSpreadValue func_degrees (valVector args, ValueCalc *calc, FuncExtra *)
{
  // val * 180 / pi
  return calc->div (calc->mul (args[0], 180.0), calc->pi());
}

// Function: RADIANS
KSpreadValue func_radians (valVector args, ValueCalc *calc, FuncExtra *)
{
  // val * pi / 180
  return calc->mul (calc->div (args[0], 180.0), calc->pi());
}

// Function: PI
KSpreadValue func_pi (valVector, ValueCalc *calc, FuncExtra *)
{
  return calc->pi();
}

// Function: atan2
KSpreadValue func_atan2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->atan2 (args[1], args[0]);
}
