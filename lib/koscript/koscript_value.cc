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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koscript_value.h"
#include "koscript_func.h"
#include "koscript_context.h"
#include "koscript_struct.h"
#include "koscript_property.h"
#include "koscript_method.h"

#include <klocale.h>

// Imported from scanner.ll
extern KLocale* s_koscript_locale;

KSValue* KSValue::s_null = 0;

KSValue::KSValue()
{
  typ = Empty;
  m_mode = Temp;
}

KSValue::KSValue( Type _type )
{
  typ = _type;
  m_mode = Temp;

  switch( typ )
    {
    case DateType:
      val.ptr = new QDate;
      break;
    case TimeType:
      val.ptr = new QTime;
      break;
    case StringType:
      val.ptr = new QString;
      break;
    case ListType:
      val.ptr = new QValueList<Ptr>;
      break;
    case MapType:
      val.ptr = new QMap<QString,Ptr>;
      break;
    case CharRefType:
      val.ptr = new KScript::CharRef( 0, 0 );
      break;
    case CharType:
      val.c = 0;
      break;
    case FunctionType:
    case MethodType:
    case PropertyType:
    case ModuleType:
    case StructType:
    case StructClassType:
      val.ptr = 0;
      break;
    case StructBuiltinMethodType:
      val.sm = 0;
      break;
    case IntType:
    case BoolType:
    case DoubleType:
    case Empty:
      // Do nothing
      break;
    case NTypes:
      Q_ASSERT( 0 );
    }
}

KSValue::KSValue( const KSValue& p ) : QShared()
{
  typ = Empty;
  *this = p;
}

KSValue::~KSValue()
{
    clear();
}

KSValue& KSValue::operator= ( const KSValue& p )
{
  clear();

  switch( p.type() )
    {
    case Empty:
      break;
    case DateType:
      val.ptr = new QDate( p.dateValue() );
      break;
    case TimeType:
      val.ptr = new QTime( p.timeValue() );
      break;
    case StringType:
      val.ptr = new QString( p.stringValue() );
      break;
    case ListType:
      val.ptr = new QValueList<Ptr>( p.listValue() );
      break;
    case MapType:
      val.ptr = new QMap<QString,Ptr>( p.mapValue() );
      break;
    case IntType:
      val.i = p.intValue();
      break;
    case BoolType:
      val.b = p.boolValue();
      break;
    case DoubleType:
      val.d = p.doubleValue();
      break;
    case CharType:
      val.c = p.charValue().unicode();
      break;
    case CharRefType:
      val.ptr = new KScript::CharRef( p.charRefValue() );
      break;
    case StructBuiltinMethodType:
      val.sm = p.val.sm;
      break;
    case FunctionType:
    case MethodType:
    case PropertyType:
    case ModuleType:
    case StructClassType:
      val.ptr = p.val.ptr;
      ((QShared*)val.ptr)->ref();
      break;
    case StructType:
      val.ptr = ((KSStruct*)p.val.ptr)->clone();
      break;
    case NTypes:
      Q_ASSERT( 0 );
    }

  typ = p.type();
  m_mode = p.mode();

  return *this;
}

QString KSValue::typeName() const
{
  return typeToName( typ );
}

void KSValue::setValue( const QDate& _value )
{
  clear();
  typ = DateType;
  val.ptr = new QDate( _value );
}

void KSValue::setValue( const QTime& _value )
{
  clear();
  typ = TimeType;
  val.ptr = new QTime( _value );
}

void KSValue::setValue( const QString& _value )
{
  clear();
  typ = StringType;
  val.ptr = new QString( _value );
}

void KSValue::setValue( const QValueList<Ptr>& _value )
{
  clear();
  typ = ListType;
  val.ptr = new QValueList<Ptr>( _value );
}

void KSValue::setValue( const QMap<QString,Ptr>& _value )
{
  clear();
  typ = MapType;
  val.ptr = new QMap<QString,Ptr>( _value );
}

void KSValue::setValue( KScript::Long _value )
{
  clear();
  typ = IntType;
  val.i = _value;
}

void KSValue::setValue( KScript::Boolean _value )
{
  clear();
  typ = BoolType;
  val.b = _value;
}

void KSValue::setValue( KScript::Double _value )
{
  clear();
  typ = DoubleType;
  val.d = _value;
}

void KSValue::setValue( const KScript::Char& _value )
{
  clear();
  typ = CharType;
  val.c = _value.unicode();
}

void KSValue::setValue( const KScript::CharRef& _value )
{
  clear();
  typ = CharRefType;
  val.ptr = new KScript::CharRef( _value );
}

void KSValue::setValue( KSFunction* _value )
{
  clear();
  typ = FunctionType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSMethod* _value )
{
  clear();
  typ = MethodType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSProperty* _value )
{
  clear();
  typ = PropertyType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSModule* _value )
{
  clear();
  typ = ModuleType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSStruct* _value )
{
  clear();
  typ = StructType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSStructClass* _value )
{
  clear();
  typ = StructClassType;
  // Do not call ref() since we take over ownership
  val.ptr = _value;
}

void KSValue::setValue( KSStructBuiltinMethod _value )
{
  clear();
  typ = StructBuiltinMethodType;
  val.sm = _value;
}

void KSValue::clear()
{
  switch( typ )
    {
    case Empty:
    case IntType:
    case BoolType:
    case DoubleType:
    case CharType:
    case StructBuiltinMethodType:
      break;
    case FunctionType:
      if ( val.ptr )
	if ( functionValue()->deref() )
	  delete ((KSFunction*)val.ptr);
      break;
    case PropertyType:
      if ( val.ptr )
	if ( propertyValue()->deref() )
	  delete ((KSProperty*)val.ptr);
      break;
    case MethodType:
      if ( val.ptr )
	if ( methodValue()->deref() )
	  delete ((KSMethod*)val.ptr);
      break;
    case ModuleType:
      if ( val.ptr )
	if ( moduleValue()->deref() )
	  delete ((KSModule*)val.ptr);
      break;
    case StructType:
      if ( val.ptr )
	if ( structValue()->deref() )
	  delete ((KSStruct*)val.ptr);
      break;
    case StructClassType:
      if ( val.ptr )
	if ( structClassValue()->deref() )
	  delete ((KSStructClass*)val.ptr);
      break;
    case StringType:
      delete (QString*)val.ptr;
      break;
    case DateType:
      delete (QDate*)val.ptr;
      break;
    case TimeType:
      delete (QTime*)val.ptr;
      break;
    case ListType:
      delete (QValueList<Ptr>*)val.ptr;
      break;
    case MapType:
      delete (QMap<QString,Ptr>*)val.ptr;
      break;
    case CharRefType:
      delete (KScript::CharRef*)val.ptr;
      break;
    case NTypes:
      Q_ASSERT(0);
      break;
    }

  typ = Empty;
}

static QString *typ_to_name = 0;

void KSValue::initTypeNameMap()
{
    if ( typ_to_name ) return;

    typ_to_name = new QString[(int)NTypes];

    typ_to_name[(int)Empty] = QString::fromLatin1("<none>");
    typ_to_name[(int)StringType] = QString::fromLatin1("String");
    typ_to_name[(int)IntType] = QString::fromLatin1("Integer");
    typ_to_name[(int)BoolType] = QString::fromLatin1("Boolean");
    typ_to_name[(int)DoubleType] = QString::fromLatin1("Double");
    typ_to_name[(int)ListType] = QString::fromLatin1("List");
    typ_to_name[(int)MapType] = QString::fromLatin1("Map");
    typ_to_name[(int)CharType] = QString::fromLatin1("Char");
    typ_to_name[(int)CharRefType] = QString::fromLatin1("Char");
    typ_to_name[(int)FunctionType] = QString::fromLatin1("Function");
    typ_to_name[(int)MethodType] = QString::fromLatin1("Method");
    typ_to_name[(int)PropertyType] = QString::fromLatin1("Property");
    typ_to_name[(int)ModuleType] = QString::fromLatin1("Module");
    typ_to_name[(int)StructType] = QString::fromLatin1("Struct");
    typ_to_name[(int)StructClassType] = QString::fromLatin1("StructClass");
    typ_to_name[(int)StructBuiltinMethodType] = QString::fromLatin1("StructBuiltinMethod");
    typ_to_name[(int)DateType] = QString::fromLatin1("Date");
    typ_to_name[(int)TimeType] = QString::fromLatin1("Time");
}

QString KSValue::typeToName( KSValue::Type _typ )
{
    initTypeNameMap();
    return typ_to_name[_typ];
}

KSValue::Type KSValue::nameToType( const QString& _name )
{
    initTypeNameMap();

    int t = (int)NTypes;
    while ( t > (int)Empty && typ_to_name[(int)--t] != _name )
	;
    return Type(t);
}

bool KSValue::cast( Type _typ )
{
  if ( typ == _typ )
    return true;

  switch( typ )
    {
    case Empty:
      return false;
    case IntType:
      if ( _typ == DoubleType )
      {
	KScript::Double d = (KScript::Double)val.i;
	val.d = d;
	typ = _typ;
	return true;
      }
      return false;
    case BoolType:
      if ( _typ == StringType )
      {
	KScript::Boolean b = val.b;
	if ( b )
	  setValue( "TRUE" );
	else
	  setValue( "FALSE" );
	typ = _typ;
	return true;
      }
      else if ( _typ == IntType )
      {
        KScript::Boolean b = val.b;
        setValue( b ? 1 : 0 );
        typ = _typ;
      }
      return false;
      break;
    case DoubleType:
      if ( _typ == IntType )
      {
	KScript::Long i = (KScript::Long)val.d;
	val.i = i;
	typ = _typ;
	return true;
      }
      return false;
    case StringType:
      if ( _typ == BoolType )
      {
	  setValue( !stringValue().isEmpty() );
	  return TRUE;
      }
      return false;
    case CharRefType:
      if ( _typ != CharType )
	return false;
      typ = _typ;
      return true;
    case PropertyType:
      /* {
	KSValue* v = propertyValue()->object()->member( propertyValue()->name(), FALSE );
	if ( !v )
	  return false;
	if ( !v->cast( _typ ) )
	  return false;
	*this = *v;
      }
      break; */
    case DateType:
    case TimeType:
    case ListType:
    case MapType:
    case CharType:
    case FunctionType:
    case MethodType:
    case StructBuiltinMethodType:
    case StructType:
    case StructClassType:
    case ModuleType:
      // They can be casted to nothing
      return false;
    case NTypes:
      Q_ASSERT(0);
      break;
    }

  typ = _typ;

  return true;
}

QString KSValue::toString( KSContext& context )
{
  switch( typ )
    {
    case Empty:
      return QString( "<none>" );
      break;
    case FunctionType:
      return QString( "<function>" );
      break;
    case PropertyType:
      return QString( "<property>" );
      break;
    case TimeType:
	return s_koscript_locale->formatTime( timeValue(), true );
    case DateType:
	return s_koscript_locale->formatDate( dateValue(), true );
    case StructClassType:
      return ( QString( "<struct class " ) + structClassValue()->name() + ">" );
      break;
    case ModuleType:
      return ( QString( "<module " ) + moduleValue()->name() + ">" );
      break;
    case StructType:
      {
	QString tmp( "{ Struct %1 { " );
	tmp = tmp.arg( structValue()->getClass()->name() );
	const QStringList& lst = structValue()->getClass()->vars();
	QStringList::ConstIterator it2 = lst.begin();
	for( ; it2 != lst.end(); ++it2 )
	{
	  QString s("( %1, %2 ), ");
	  KSValue::Ptr ptr = ((KSStruct*)val.ptr)->member( context, *it2 );
	  s = s.arg( *it2 ).arg( ptr->toString( context ) );
	  tmp += s;
	}
	tmp += "} }";
	return tmp;
      }
      break;
    case MethodType:
      return QString( "<method>" );
      break;
    case StructBuiltinMethodType:
      return QString( "<struct builtin method>" );
      break;
    case IntType:
      {
	QString tmp;
	tmp.setNum( val.i );
	return tmp;
      }
      break;
    case BoolType:
      {
	if ( val.b )
	  return QString( "TRUE" );
	else
	  return QString( "FALSE" );
      }
      break;
    case DoubleType:
      {
	QString tmp;
	tmp.setNum( val.d );
	return tmp;
      }
      break;
    case StringType:
      return *((QString*)val.ptr);
      break;
    case ListType:
      {
	QString tmp( "[ " );
	QValueList<Ptr>* lst = (QValueList<Ptr>*)val.ptr;
	QValueList<Ptr>::Iterator it = lst->begin();
	QValueList<Ptr>::Iterator end = lst->end();
	for( ; it != end; ++it )
	{
	  tmp += (*it)->toString( context );
	  tmp += ", ";
	}
	tmp.truncate( tmp.length() - 1 );
	tmp[ tmp.length() - 1 ] = ' ';
	tmp += "]";
	return tmp;
      }
      break;
    case MapType:
      {
	QString tmp( "{ " );
	QMap<QString,Ptr>* lst = (QMap<QString,Ptr>*)val.ptr;
	QMap<QString,Ptr>::Iterator it = lst->begin();
	QMap<QString,Ptr>::Iterator end = lst->end();
	for( ; it != end; ++it )
	{
	  tmp += "( ";
	  tmp += it.key();
	  tmp += ", ";
	  tmp += it.data()->toString( context );
	  tmp += " ), ";
	}
	tmp.truncate( tmp.length() - 1 );
	tmp[ tmp.length() - 1 ] = ' ';
	tmp += "}";
	return tmp;
      }
      break;
    case CharRefType:
    case CharType:
      {
	QString tmp( "'%1'" );
	return tmp.arg( charValue() );
      }
      break;
    case NTypes:
      Q_ASSERT(0);
      break;
    }

  // Never reached
  return QString::null;
}

void KSValue::suck( KSValue* v )
{
  if ( v->mode() != Temp )
  {
    *this = *v;
    return;
  }

  clear();

  typ = v->type();
  val = v->val;

  v->typ = Empty;
}

bool KSValue::operator==( const KSValue& v ) const
{
  return ( val.ptr == v.val.ptr && typ == v.typ );
}

bool KSValue::cmp( const KSValue& v ) const
{
  if ( typ != v.typ )
    return false;

  switch( typ )
    {
    case Empty:
      return true;
    case StringType:
      return ( stringValue() == v.stringValue() );
    case DateType:
      return ( dateValue() == v.dateValue() );
    case TimeType:
      return ( timeValue() == v.timeValue() );
    case KSValue::IntType:
      return ( val.i == v.val.i );
    case BoolType:
      return ( val.b == v.val.b );
    case DoubleType:
      return ( val.d == v.val.d );
    case ListType:
      return ( listValue() == v.listValue() );
    case MapType:
      {
	QMap<QString,KSValue::Ptr>::ConstIterator it, it2, end, end2;
	it = mapValue().begin();
	it2 = v.mapValue().begin();
	end = mapValue().end();
	end2 = v.mapValue().end();
	while( it != end && it2 != end2 )
	{
	  if ( it.key() != it2.key() || !it2.data()->cmp( *it.data() ) )
	    return false;
	  ++it;
	  ++it2;
	}
	return ( it == end && it2 == end2 );
      }
    case CharType:
      return ( val.c == v.val.c );
    case CharRefType:
      return ( ((KScript::Char)charRefValue()) == ((KScript::Char)v.charRefValue()) );
    case FunctionType:
    case MethodType:
    case PropertyType:
    case ModuleType:
    case StructType:
    case StructClassType:
      return ( val.ptr == v.val.ptr );
    case StructBuiltinMethodType:
      return ( val.sm == v.val.sm );
    case NTypes:
      Q_ASSERT( 0 );
    }

  // Never reached
  return false;
}

bool KSValue::implicitCast( Type _typ ) const
{
    if ( typ == _typ )
	return true;

    switch( typ )
    {
    case Empty:
	return false;
    case IntType:
	if ( _typ == BoolType )
	    return TRUE;
	if ( _typ == DoubleType )
	    return TRUE;
	return false;
    case BoolType:
	return FALSE;
    case DoubleType:
	if ( _typ == IntType )
	    return TRUE;
	if ( _typ == BoolType )
		return TRUE;
      return false;
    case StringType:
	// Laurent fix N("foo") return 0 and not "False" bug #86873
      	//if ( _typ == BoolType )
        //return TRUE;
	return false;
    case CharRefType:
	if ( _typ == CharType )
	    return TRUE;
    case DateType:
    case TimeType:
    case PropertyType:
    case ListType:
    case MapType:
    case CharType:
    case FunctionType:
    case MethodType:
    case StructBuiltinMethodType:
    case StructType:
    case StructClassType:
    case ModuleType:
      // They can be casted to nothing
      return false;
    case NTypes:
      Q_ASSERT(0);
      break;
    }

    return FALSE;
}
