/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>
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

#include "formula.h"
#include "functions.h"
#include "valuecalc.h"

#include <qdict.h>
#include <qvaluevector.h>

#include <kstaticdeleter.h>

#include <math.h>

namespace KSpread
{

class Function::Private
{
public:
  QString name;
  FunctionPtr ptr;
  int paramMin, paramMax;
  bool acceptArray;
};

class FunctionRepository::Private
{
public:
  QDict<Function> functions;
};

} // namespace KSpread


using namespace KSpread;

Function::Function( const QString& name, FunctionPtr ptr )
{
  d = new Private;
  d->name = name;
  d->ptr = ptr;
  d->acceptArray = false;
  d->paramMin = 1;
  d->paramMax = 1;
}

Function::~Function()
{
  delete d;
}

QString Function::name() const
{
  return d->name;
}

void Function::setParamCount (int min, int max)
{
  d->paramMin = min;
  d->paramMax = (max == 0) ? min : max;
}

bool Function::paramCountOkay (int count)
{
  // less than needed
  if (count < d->paramMin) return false;
  // no upper limit
  if (d->paramMax == -1) return true;
  // more than needed
  if (count > d->paramMax) return false;
  // okay otherwise
  return true;
}

void Function::setAcceptArray (bool accept) {
  d->acceptArray = accept;
}

KSpreadValue Function::exec (QValueVector<KSpreadValue> args, ValueCalc *calc)
{
  // check number of parameters
  if (!paramCountOkay (args.count()))
    return KSpreadValue::errorVALUE();

  // do we need to perform array expansion ?
  bool mustExpandArray = false;
  if (!d->acceptArray)
    for (unsigned int i = 0; i < args.count(); ++i) {
      if (args[i].isArray())
        mustExpandArray = true;
    }

  if( !d->ptr ) return KSpreadValue::errorVALUE();
  
  // perform the actual array expansion if need be
  
  if (mustExpandArray) {
    // compute number of rows/cols of the result
    int rows = 0;
    int cols = 0;
    for (unsigned int i = 0; i < args.count(); ++i) {
      int x = (args[i].type() == KSpreadValue::Array) ? args[i].rows() : 1;
      if (x > rows) rows = x;
      x = (args[i].type() == KSpreadValue::Array) ? args[i].columns() : 1;
      if (x > cols) cols = x;
    }
    // allocate the resulting array
    KSpreadValue res (cols, rows);
    // perform the actual computation for each element of the array
    for (int row = 0; row < rows; ++row)
      for (int col = 0; col < cols; ++col) {
        // fill in the parameter vector
        QValueVector<KSpreadValue> vals (args.count());
        for (unsigned int i = 0; i < args.count(); ++i) {
          int r = args[i].rows();
          int c = args[i].columns();
          vals[i] = args[i].isArray() ?
              args[i].element (col % c, row % r): args[i];
        }
        
        res.setElement (col, row, exec (vals, calc));
      }
    return res;
  }
  else
    // call the function
    return (*d->ptr) (args, calc);
}

static KStaticDeleter<FunctionRepository> fr_sd;
FunctionRepository* FunctionRepository::s_self = 0;

FunctionRepository* FunctionRepository::self()
{
  if( !s_self )
    fr_sd.setObject( s_self, new FunctionRepository() );
    
  return s_self;
}

KSpreadValue function_sin (QValueVector<KSpreadValue> args, ValueCalc *calc)
{
  return calc->sin (args[0]);
}

FunctionRepository::FunctionRepository()
{
  d = new Private;
  
  d->functions.setAutoDelete( true );
  
  // registerMathFunction
  
  Function *fn;
  
  fn = new Function ("SIN", function_sin);
  add (fn);
  
  // load desc/help from XML file
}

FunctionRepository::~FunctionRepository()
{
  delete d;
}

void FunctionRepository::add( Function* function )
{
  if( !function ) return;
  d->functions.insert( function->name().upper(), function );
}

Function* FunctionRepository::function( const QString& name )
{
  return d->functions.find( name.upper() );
}
