/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread

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

// built-in logical functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_functions.h"
#include "kspread_util.h"

// prototypes (sorted alphabetically)
bool kspreadfunc_and( KSContext& context );
bool kspreadfunc_false( KSContext& context );
bool kspreadfunc_if( KSContext& context );
bool kspreadfunc_nand( KSContext& context );
bool kspreadfunc_nor( KSContext& context );
bool kspreadfunc_not( KSContext& context );
bool kspreadfunc_or( KSContext& context );
bool kspreadfunc_true( KSContext& context );
bool kspreadfunc_xor( KSContext& context );

// registers all logic functions
void KSpreadRegisterLogicFunctions()
{
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "AND",   kspreadfunc_and );
  repo->registerFunction( "FALSE", kspreadfunc_false );
  repo->registerFunction( "IF",    kspreadfunc_if );
  repo->registerFunction( "NAND",  kspreadfunc_nand );  // KSpread-specific
  repo->registerFunction( "NOR",   kspreadfunc_nor );   // KSpread-specific
  repo->registerFunction( "NOT",   kspreadfunc_not );
  repo->registerFunction( "OR",    kspreadfunc_or );
  repo->registerFunction( "TRUE",  kspreadfunc_true );
  repo->registerFunction( "XOR",   kspreadfunc_xor );   // KSpread-specific
}

// Function: FALSE
bool kspreadfunc_false( KSContext & context )
{
  context.setValue( new KSValue( FALSE ) );
  return true;
}

// Function: TRUE
bool kspreadfunc_true( KSContext & context )
{
  context.setValue( new KSValue( TRUE ) );
  return true;
}

// Function: NOT
bool kspreadfunc_not( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "NOT", true ) || !KSUtil::checkArgumentsCount( context, 1, "not", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
    return false;

  bool toto = !args[0]->boolValue();
  context.setValue( new KSValue(toto));
  return true;
}

bool kspreadfunc_or_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_or_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = (first || (*it)->boolValue());
    else
      return false;
  }

  return true;
}

// Function: OR
bool kspreadfunc_or( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_or_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

// Function: NOR
bool kspreadfunc_nor( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_or_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( !first ) );

  return b;
}


bool kspreadfunc_and_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_and_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = first && (*it)->boolValue();
    else
      return false;
  }

  return true;
}

// Function: AND
bool kspreadfunc_and( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_and_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

// Function: NAND
bool kspreadfunc_nand( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_and_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( !first ) );

  return b;
}

static bool kspreadfunc_xor_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_xor_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = first ^ (*it)->boolValue();
    else
      return false;
  }

  return true;
}

// Function: XOR
bool kspreadfunc_xor( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_xor_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

// Function: IF
bool kspreadfunc_if( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "if", true ) || !KSUtil::checkArgumentsCount( context, 3, "IF", true ))
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
      return false;

    if (  args[0]->boolValue() == true )
      context.setValue( new KSValue( *(args[1]) ) );
    else
      context.setValue( new KSValue( *(args[2]) ) );

    return true;
}
