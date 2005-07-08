/* This file is part of the KDE project
   Copyright (C) 2003,2004 Ariya Hidayat <ariya@kde.org>

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

  // TODO: work on arrays if need be

  if( !d->ptr ) return KSpreadValue::errorVALUE();
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
