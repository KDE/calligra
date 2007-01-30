/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2006 Brad Hards <bradh@frogmouth.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
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

#include "Functions.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

using namespace KSpread;

// prototypes (sorted alphabetically)
Value func_bitand (valVector args, ValueCalc *calc, FuncExtra *);
Value func_bitor (valVector args, ValueCalc *calc, FuncExtra *);

// registers all bitops functions
void RegisterBitopsFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("BITAND", func_bitand);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("BITOR", func_bitor);
  f->setParamCount (2);
  repo->add (f);
}

// Function: BITAND
Value func_bitand (valVector args, ValueCalc *, FuncExtra *)
{
    if ( args[0].type() == Value::Integer )
        kDebug()<< "its an integer" << endl;
    else if ( args[0].type() == Value::Float )
        kDebug()<< "its a float" << endl;
    else
        kDebug()<< "beats me: " << args[0].type() <<endl;

  unsigned long long x = ( unsigned long long ) args[0].asFloat();
  unsigned long long y = ( unsigned long long ) args[1].asFloat();
  double result = ( double ) ( x & y );
  return Value (result);
}

// Function: BITOR
Value func_bitor (valVector args, ValueCalc *, FuncExtra *)
{
  unsigned long x = args[0].asInteger();
  unsigned long y = args[1].asInteger();
  double result = ( double) ( x | y );
  return Value (result);
}
