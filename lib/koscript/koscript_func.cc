/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "koscript_func.h"
#include "koscript_context.h"
#include "koscript_value.h"
#include "koscript_parsenode.h"
#include "koscript_util.h"
#include "koscript_method.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <klocale.h>

#include <iostream>

// Imported from scanner.ll
extern KLocale* s_koscript_locale;

using namespace std;

bool KSScriptFunction::call( KSContext& context )
{
  return m_node->eval( context );
}

static bool ksfunc_mid( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    uint len = 0xffffffff;
    if ( KSUtil::checkArgumentsCount( context, 3, "mid", false ) )
    {
        if( KSUtil::checkType( context, args[2], KSValue::DoubleType, false ) )
            len = (uint) args[2]->doubleValue();
        else if( KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
            len = (uint) args[2]->intValue();
        else
            return false;
    }
    else if ( !KSUtil::checkArgumentsCount( context, 2, "mid", true ) )
        return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
        return false;

    if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
        return false;
    int pos = args[1]->intValue();

    QString tmp = args[0]->stringValue().mid( pos, len );
    context.setValue( new KSValue(tmp));
    return true;
}

static bool ksfunc_time( KSContext& context )
{
    KSUtil::checkArgs( context, "s", "time", TRUE );

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    QTime t = s_koscript_locale->readTime( args[0]->stringValue() );
    if ( !t.isValid() )
    {
        QString tmp( i18n("Invalid time format: %1") );
        context.setException( new KSException( "ParsingError", tmp.arg( args[0]->stringValue() ), -1 ) );
        return false;
    }

    context.setValue( new KSValue( t ) );

    return true;
}

static bool ksfunc_date( KSContext& context )
{
    KSUtil::checkArgs( context, "s", "date", TRUE );

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    QDate t = s_koscript_locale->readDate( args[0]->stringValue() );
    if ( !t.isValid() )
    {
        QString tmp( i18n("Invalid date format: %1") );
        context.setException( new KSException( "ParsingError", tmp.arg( args[0]->stringValue() ), -1 ) );
        return false;
    }

    context.setValue( new KSValue( t ) );

    return true;
}

/**
 * Like QString::length for strings.
 * Returns the length of lists or maps, too.
 */
static bool ksfunc_length( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "length", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::ListType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long) args[0]->listValue().count() ) );
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::MapType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long) args[0]->mapValue().count() ) );
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long) args[0]->stringValue().length() ) );
  }
  else
  {
    QString tmp( i18n("Cannot calculate length of a %1 value.") );
    context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
    return false;
  }

  return true;
}

static bool ksfunc_lower( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgs( context, args, "s", "lower", TRUE ) )
        return FALSE;

    context.setValue( new KSValue( args[0]->stringValue().lower() ) );

    return TRUE;
}

static bool ksfunc_upper( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgs( context, args, "s", "lower", TRUE ) )
        return FALSE;

    context.setValue( new KSValue( args[0]->stringValue().upper() ) );

    return TRUE;
}

static bool ksfunc_isEmpty( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "isEmpty", true ) )
        return false;

    if ( KSUtil::checkType( context, args[0], KSValue::ListType, false ) )
    {
        context.setValue( new KSValue( args[0]->listValue().isEmpty() ) );
    }
    else if ( KSUtil::checkType( context, args[0], KSValue::MapType, false ) )
    {
        context.setValue( new KSValue( args[0]->mapValue().isEmpty() ) );
    }
    else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    {
        context.setValue( new KSValue( args[0]->stringValue().isEmpty() ) );
    }
    else
    {
        QString tmp( i18n("Cannot determine emptiness of a %1 value.") );
        context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
        return false;
    }

    return true;
}

/**
 * Like QString::toInt
 */
static bool ksfunc_toInt( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "toInt", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long)args[0]->doubleValue() ) );
    return true;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    bool ok;
    KScript::Long l = args[0]->stringValue().toLong( &ok );
    if ( ok )
    {
      context.setValue( new KSValue( l ) );
      return true;
    }
  }

  QString tmp( i18n("Cannot calculate a numerical value from a %1 value.") );
  context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
  return false;
}

/**
 * Like QString::toFloat
 */
static bool ksfunc_toFloat( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "toFloat", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
  {
    context.setValue( new KSValue( (KScript::Double)args[0]->intValue() ) );
    return true;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    bool ok;
    KScript::Double l = args[0]->stringValue().toDouble( &ok );
    if ( ok )
    {
      context.setValue( new KSValue( l ) );
      return true;
    }
  }

  QString tmp( i18n("Cannot calculate a floating point value from a %1 value.") );
  context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
  return false;
}

/**
 * Like QString::arg
 *
 * Syntax: arg( string, value )
 * Syntax: string.arg( value )
 */
static bool ksfunc_arg( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "arg", true ) )
        return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, TRUE ) )
        return FALSE;

    QString str = args[0]->stringValue();

    if ( KSUtil::checkType( context, args[1], KSValue::StringType, FALSE ) )
        context.setValue( new KSValue( str.arg( args[1]->stringValue() ) ) );
    else if ( KSUtil::checkType( context, args[1], KSValue::IntType, FALSE ) )
        context.setValue( new KSValue( str.arg( args[1]->intValue() ) ) );
    else if ( KSUtil::checkType( context, args[1], KSValue::DoubleType, FALSE ) )
        context.setValue( new KSValue( str.arg( args[1]->doubleValue() ) ) );
    else if ( KSUtil::checkType( context, args[1], KSValue::CharType, FALSE ) )
        context.setValue( new KSValue( str.arg( args[1]->charValue() ) ) );
    else context.setValue( new KSValue( str.arg( args[1]->toString( context ) ) ) );

    return TRUE;
}

static bool ksfunc_stringListSplit( KSContext &context )
{
  QValueList<KSValue::Ptr> &args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "arg", true ) );

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, TRUE ) )
    return false;

  QString sep = args[0]->stringValue();
  QString str = args[1]->stringValue();

  QStringList strLst = QStringList::split( sep, str );

  KSValue *v = new KSValue( KSValue::ListType );

  QStringList::ConstIterator it = strLst.begin();
  QStringList::ConstIterator end = strLst.end();
  for (; it != end; ++it )
    v->listValue().append( new KSValue( *it ) );

  context.setValue( v );
  return true;
}

static bool ksfunc_print( KSContext& context )
{
  // We know that the context always holds a list of parameters
  QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = context.value()->listValue().end();

  if ( it == end )
    cout << endl;

  for( ; it != end; ++it )
    cout << (*it)->toString( context ).local8Bit();

  // context.value()->clear();
  context.setValue( 0 );

  return true;
}

static bool ksfunc_println( KSContext& context )
{
  // We know that the context always holds a list of parameters
  QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = context.value()->listValue().end();

  if ( it == end )
    cout << endl;

  for( ; it != end; ++it )
    cout << (*it)->toString( context ).local8Bit() << endl;

  // context.value()->clear();
  context.setValue( 0 );

  return true;
}

KSModule::Ptr ksCreateModule_KScript( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "koscript" );

// ariya: for the time being, let's just disable KoScript built-in functions
// because many have the same name as KSpread functions
#if 0
  module->addObject( "time", new KSValue( new KSBuiltinFunction( module, "time", ksfunc_time ) ) );
  module->addObject( "date", new KSValue( new KSBuiltinFunction( module, "date", ksfunc_date ) ) );
  module->addObject( "print", new KSValue( new KSBuiltinFunction( module, "print", ksfunc_print ) ) );
  module->addObject( "println", new KSValue( new KSBuiltinFunction( module, "println", ksfunc_println ) ) );
  module->addObject( "length", new KSValue( new KSBuiltinFunction( module, "length", ksfunc_length ) ) );
  module->addObject( "arg", new KSValue( new KSBuiltinFunction( module, "arg", ksfunc_arg ) ) );
  module->addObject( "mid", new KSValue( new KSBuiltinFunction( module, "mid", ksfunc_mid ) ) );
  module->addObject( "upper", new KSValue( new KSBuiltinFunction( module, "upper", ksfunc_upper ) ) );
  module->addObject( "lower", new KSValue( new KSBuiltinFunction( module, "lower", ksfunc_lower ) ) );
  module->addObject( "isEmpty", new KSValue( new KSBuiltinFunction( module, "isEmpty", ksfunc_isEmpty ) ) );
  module->addObject( "toInt", new KSValue( new KSBuiltinFunction( module, "toInt", ksfunc_toInt ) ) );
  module->addObject( "toFloat", new KSValue( new KSBuiltinFunction( module, "toFloat", ksfunc_toFloat ) ) );
  module->addObject( "stringListSplit", new KSValue( new KSBuiltinFunction( module, "stringListSplit", ksfunc_stringListSplit ) ) );
#endif

  return module;
}
