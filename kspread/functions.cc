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
};

class FunctionRepository::Private
{
public:
  QDict<Function> functions;
};

}; // namespace KSpread


using namespace KSpread;

Function::Function( const QString& name, FunctionPtr ptr )
{
  d = new Private;
  d->name = name;
  d->ptr = ptr;
}

Function::~Function()
{
  delete d;
}

QString Function::name() const
{
  return d->name;
}

KSpreadValue Function::exec( const Formula* formula, QValueVector<KSpreadValue> args )
{
  if( !d->ptr ) return KSpreadValue::errorVALUE();
  return (*d->ptr)( formula, args );
}

static KStaticDeleter<FunctionRepository> sd;
FunctionRepository* FunctionRepository::s_self = 0;

FunctionRepository* FunctionRepository::self()
{
  if( !s_self )
    sd.setObject( s_self, new FunctionRepository() );
    
  return s_self;
}

KSpreadValue function_sin( const Formula* formula, QValueVector<KSpreadValue> args )
{
  KSpreadValue result;
  if( args.count() != 1 )
    return KSpreadValue::errorVALUE();
    
  KSpreadValue angle = formula->convertToNumber( args[0] );
  if( angle.isError() )
    return KSpreadValue::errorVALUE();
  
  return KSpreadValue( sin( angle.asFloat() ) );
}

FunctionRepository::FunctionRepository()
{
  d = new Private;
  
  d->functions.setAutoDelete( true );
  
  // registerMathFunction
  
  add( new Function( "SIN", function_sin ) );
  
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
