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

#include "koscript_util.h"
#include "koscript_context.h"
#include "koscript_struct.h"
#include <klocale.h>

bool KSUtil::checkArgumentsCount( KSContext& context, uint count, const QString& name, bool fatal )
{
  Q_ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( args.count() == count )
    return true;

  if ( !fatal )
    return false;

  if ( args.count() < count )
    tooFewArgumentsError( context, name );
  else
    tooManyArgumentsError( context, name );

  return false;
}

bool KSUtil::checkType( KSContext& context, KSValue* v, KSValue::Type t, bool fatal )
{
    if ( !v->implicitCast( t ) )
    {
	if ( !fatal )
	    return false;

	castingError( context, v, t );
	return false;
    }

    return true;
}

bool KSUtil::checkType( KSContext& context, const KSValue::Ptr& v, KSValue::Type t, bool fatal )
{
    if ( !v->implicitCast( t ) )
    {
	if ( !fatal )
	    return false;

	castingError( context, v, t );
	return false;
    }

    return true;
}

void KSUtil::castingError( KSContext& context, const QString& from, const QString& to )
{
  QString tmp( i18n("From %1 to %2") );
  context.setException( new KSException( "CastingError", tmp.arg( from ).arg( to ), -1 ) );
}

void KSUtil::castingError( KSContext& context, KSValue* v, KSValue::Type t )
{
  QString tmp( i18n("From %1 to %2") );
  context.setException( new KSException( "CastingError", tmp.arg( v->typeName() ).arg( KSValue::typeToName( t ) ), -1 ) );
}

void KSUtil::argumentsMismatchError( KSContext& context, const QString& methodname )
{
  QString tmp( i18n("Arguments did not match the methods %1 parameter list.") );
  context.setException( new KSException( "ParameterMismatch", tmp.arg( methodname ) ) );
}

void KSUtil::tooFewArgumentsError( KSContext& context, const QString& methodname )
{
  QString tmp( i18n("Too few arguments for method %1") );
  context.setException( new KSException( "TooFewArguments", tmp.arg( methodname ), -1 ) );
}

void KSUtil::tooManyArgumentsError( KSContext& context, const QString& methodname )
{
  QString tmp( i18n("Too many arguments for method %1") );
  context.setException( new KSException( "TooManyArguments", tmp.arg( methodname ), -1 ) );
}

bool KSUtil::checkArgs( KSContext& context, const QCString& signature, const QString& method, bool fatal )
{
    // Is the value really a list ?
    if ( !KSUtil::checkType( context, context.value(), KSValue::ListType, TRUE ) )
	return FALSE;
    return KSUtil::checkArgs( context, context.value()->listValue(), signature, method, fatal );
}

bool KSUtil::checkArgs( KSContext& context, const QValueList<KSValue::Ptr>& args,
			const QCString& signature, const QString& method, bool fatal )
{
    uint done = 0;
    uint count = args.count();
    uint len = signature.length();
    uint pos = 0;
    while ( pos < len )
    {
	// Have seen all parameters and we may skip optional parameters
	if ( done == count && signature[pos] == '|' )
	    return TRUE;
	if ( signature[pos] == '|' )
	    ++pos;
	if ( signature[pos] == 'i' )
        {
	    if ( !checkType( context, args[done], KSValue::IntType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 'f' )
        {
	    if ( !checkType( context, args[done], KSValue::DoubleType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 'b' )
        {
	    if ( !checkType( context, args[done], KSValue::BoolType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 's' )
        {
	    if ( !checkType( context, args[done], KSValue::StringType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 'c' )
        {
	    if ( !checkType( context, args[done], KSValue::CharType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == '[' )
        {
	    if ( !checkType( context, args[done], KSValue::ListType, fatal ) )
		return FALSE;
	    ++pos;
	    if ( signature[pos] == ']' ) { }
	    // TODO: check vars in the list
	}
	else if ( signature[pos] == '{' )
        {
	    if ( !checkType( context, args[done], KSValue::MapType, fatal ) )
		return FALSE;
	    ++pos;
	    if ( signature[pos] == '}' ) { }
	    // TODO: check vars in the list
	}
	else if ( signature[pos] == 'S' )
        {
	    if ( !checkType( context, args[done], KSValue::StructType, fatal ) )
		return FALSE;
	    ++pos;
	    uint x = pos;
	    while( signature[pos] != ';' && signature[pos] != 0 )
		++pos;
	    Q_ASSERT( signature[pos] == ';' );
	    if ( args[done]->structValue()->getClass()->fullName() != signature.mid( x, pos - x ).data() )
	    {
		if ( fatal )
		    castingError( context, args[done]->structValue()->getClass()->fullName(),
				  signature.mid( x, pos - x ).data() );
		return FALSE;
	    }
	    ++pos;
	}
	else
	    Q_ASSERT( 0 );

	++done;
    }

    // Too many arguments ?
    if ( done < count )
    {
	if ( fatal )
	    tooFewArgumentsError( context, method );
	return FALSE;
    }

    return TRUE;
}

bool KSUtil::checkArg( KSContext& context, const KSValue::Ptr& arg,
		       const QCString& signature, const QString& /*method*/, bool fatal )
{
    int pos = 0;

    if ( signature[pos] == 'i' )
	return checkType( context, arg, KSValue::IntType, fatal );
    else if ( signature[pos] == 'f' )
	return checkType( context, arg, KSValue::DoubleType, fatal );
    else if ( signature[pos] == 'b' )
	return checkType( context, arg, KSValue::BoolType, fatal );
    else if ( signature[pos] == 's' )
	return checkType( context, arg, KSValue::StringType, fatal );
    else if ( signature[pos] == 'c' )
	return checkType( context, arg, KSValue::CharType, fatal );
    else if ( signature[pos] == '[' )
    {
	if ( !checkType( context, arg, KSValue::ListType, fatal ) )
	    return FALSE;
	++pos;
	// TODO: check vars in the list
	return TRUE;
    }
    else if ( signature[pos] == '{' )
    {
	if ( !checkType( context, arg, KSValue::MapType, fatal ) )
	    return FALSE;
	++pos;
	// TODO: check vars in the list
	return TRUE;
    }
    else if ( signature[pos] == 'S' )
    {
	if ( !checkType( context, arg, KSValue::StructType, fatal ) )
	    return FALSE;
	++pos;
	uint x = pos;
	while( signature[pos] != ';' && signature[pos] != 0 )
	    ++pos;
	Q_ASSERT( signature[pos] == ';' );
	if ( arg->structValue()->getClass()->fullName() != signature.mid( x, pos - x ).data() )
        {
	    if ( fatal )
		castingError( context, arg->structValue()->getClass()->fullName(),
			      signature.mid( x, pos - x ).data() );
	    return FALSE;
	}
	return TRUE;
    }

    Q_ASSERT( 0 );
    return FALSE;
}
