/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

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

#include "aielement.h"
#include <qglobal.h>

AIElement::Private::Private()
{
    typ = AIElement::Invalid;
}

AIElement::Private::Private( Private* d )
{
  switch( d->typ )
	{
		case AIElement::Invalid:
	    break;
		case AIElement::String:
		case AIElement::Reference:
		case AIElement::Operator:
	    value.ptr = new QString( *((QString*)d->value.ptr) );
	    break;
		case AIElement::CString:
	    // QCString is explicit shared
	    value.ptr = new QCString( *((QCString*)d->value.ptr) );
	    break;
/*		case AIElement::List:
	    value.ptr = new QValueList<AIElement>( *((QValueList<AIElement>*)d->value.ptr) );
	    break; */
		case AIElement::ElementArray:
	    value.ptr = new QValueVector<AIElement>( *((QValueVector<AIElement>*)d->value.ptr) );
	    break;
		case AIElement::Block:
	    value.ptr = new QValueVector<AIElement>( *((QValueVector<AIElement>*)d->value.ptr) );
	    break;
		case AIElement::ByteArray:
	    value.ptr = new QByteArray( *((QByteArray*)d->value.ptr) );
	    break;
		case AIElement::Int:
	    value.i = d->value.i;
	    break;
		case AIElement::UInt:
	    value.u = d->value.u;
	    break;
		case AIElement::Double:
	    value.d = d->value.d;
	    break;
		case AIElement::Byte:
	    value.b = d->value.b;
	    break;
		default:
	    Q_ASSERT( 0 );
	}

  typ = d->typ;
}

AIElement::Private::~Private()
{
    clear();
}

void AIElement::Private::clear()
{
  switch( typ )
	{
		case AIElement::String:
		case AIElement::Operator:
		case AIElement::Reference:
	    delete (QString*)value.ptr;
	    break;
		case AIElement::CString:
	    delete (QCString*)value.ptr;
	    break;
/*		case AIElement::List:
	    delete (QValueList<AIElement>*)value.ptr;
	    break; */
		case AIElement::ElementArray:
	    delete (QValueVector<AIElement>*)value.ptr;
	    break;
		case AIElement::Block:
	    delete (QValueVector<AIElement>*)value.ptr;
	    break;
		case AIElement::ByteArray:
	    delete (QByteArray*)value.ptr;
	    break;
		case AIElement::Invalid:
		case AIElement::Int:
		case AIElement::UInt:
		case AIElement::Double:
		case AIElement::Byte:
	    break;
	}

  typ = AIElement::Invalid;
}

/*!
  Constructs an invalid aielement.
*/
AIElement::AIElement()
{
    d = new Private;
}

/*!
  Destroys the AIElement and the contained object.

  Note that subclasses that reimplement clear() should reimplement
  the destructor to call clear().  This destructor calls clear(), but
  because it is the destructor, AIElement::clear() is called rather than
  a subclass's clear().
*/
AIElement::~AIElement()
{
  if ( d->deref() )
	  delete d;
}

/*!
  Constructs a copy of the aielement, \a p, passed as the argument to this
  constructor. Usually this is a deep copy, but a shallow copy is made
  if the stored data type is explicitly shared, as e.g. QImage is.
*/
AIElement::AIElement( const AIElement& p )
{
  d = new Private;
  *this = p;
}

/*!
  Constructs a new aielement with a string value, \a val.
*/
AIElement::AIElement( const QString& val, Type type )
{
  d = new Private;
  d->typ = type;
  d->value.ptr = new QString( val );
}

/*!
  Constructs a new aielement with a C-string value, \a val.

  If you want to modify the QCString after you've passed it to this
  constructor, we recommend passing a deep copy (see
  QCString::copy()).
*/
AIElement::AIElement( const QCString& val )
{
  d = new Private;
  d->typ = CString;
  d->value.ptr = new QCString( val );
}

/*!
  Constructs a new aielement with a C-string value of \a val if \a val
  is non-null.  The aielement creates a deep copy of \a val.

  If \a val is null, the resulting aielement has type Invalid.
*/
AIElement::AIElement( const char* val )
{
  d = new Private;
  if ( val == 0 )
	  return;
  d->typ = CString;
  d->value.ptr = new QCString( val );
}

/*!
  Constructs a new aielement with an integer value, \a val.
*/
AIElement::AIElement( int val )
{
  d = new Private;
  d->typ = Int;
  d->value.i = val;
}

/*!
  Constructs a new aielement with an unsigned integer value, \a val.
*/
AIElement::AIElement( uint val )
{
  d = new Private;
  d->typ = UInt;
  d->value.u = val;
}

/*!
  Constructs a new aielement with an byte value, \a val.
*/
AIElement::AIElement( uchar val )
{
  d = new Private;
  d->typ = Byte;
  d->value.b = val;
}


/*!
  Constructs a new aielement with a floating point value, \a val.
*/
AIElement::AIElement( double val )
{
  d = new Private;
  d->typ = Double;
  d->value.d = val;
}

/*!
  Constructs a new aielement with a list value, \a val.
*/
/* AIElement::AIElement( const QValueList<AIElement>& val )
{
  d = new Private;
  d->typ = List;
  d->value.ptr = new QValueList<AIElement>( val );
}  */

AIElement::AIElement( const QValueVector<AIElement>& val, Type type )
{
  d = new Private;
  d->typ = type;
  d->value.ptr = new QValueVector<AIElement>( val );
}

AIElement::AIElement( const QByteArray& val )
{
  d = new Private;
  d->typ = ByteArray;
  d->value.ptr = new QByteArray( val );
}

/*!
  Assigns the value of the aielement \a aielement to this aielement.

  This is a deep copy of the aielement, but note that if the aielement
  holds an explicitly shared type such as QImage, a shallow copy
  is performed.
*/
AIElement& AIElement::operator= ( const AIElement& aielement )
{
  AIElement& other = (AIElement&)aielement;

  other.d->ref();
  if ( d->deref() )
	  delete d;

  d = other.d;

  return *this;
}

/*!
  \internal
*/
void AIElement::detach()
{
  if ( d->count == 1 )
	return;

  d->deref();
  d = new Private( d );
}

/*!
  Returns the name of the type stored in the aielement.
  The returned strings describe the C++ datatype used to store the
  data: for example, "QFont", "QString", or "QValueList<AIElement>".
  An Invalid aielement returns 0.
*/
const char* AIElement::typeName() const
{
  return typeToName( d->typ );
}

/*! Convert this aielement to type Invalid and free up any resources
  used.
*/
void AIElement::clear()
{
  if ( d->count > 1 )
  {
	  d->deref();
	  d = new Private;
	  return;
  }

  d->clear();
}

static const int ntypes = 11;
static const char* const type_map[ntypes] =
{
  0,
//  "QValueList<AIElement>",
  "QString",
  "int",
  "uint",
  "double",
  "QCString",
  "Operator",
  "Reference",
  "QValueVector<AIElement>",
  "QByteArray",
  "uchar",
};

/*!
  Converts the enum representation of the storage type, \a typ, to its
  string representation.
*/
const char* AIElement::typeToName( Type typ )
{
  if ( typ >= ntypes )
	  return 0;
  return type_map[typ];
}

/*!
  Converts the string representation of the storage type gven in \a
  name, to its enum representation.

  If the string representation cannot be converted to any enum
  representation, the aielement is set to \c Invalid.
*/
AIElement::Type AIElement::nameToType( const char* name )
{
  for ( int i = 0; i < ntypes; i++ ) {
	  if ( !qstrcmp( type_map[i], name ) )
	    return (Type) i;
  }
  return Invalid;
}

/*!
  Returns the aielement as a QString if the aielement has type()
  String, CString, ByteArray, Int, Uint, Double,
  or QString::null otherwise.

  \sa asString()
*/
const QString AIElement::toString() const
{
  if ( d->typ == CString )
	  return QString::fromLatin1( toCString() );
  if ( d->typ == Int )
	  return QString::number( toInt() );
  if ( d->typ == UInt )
	  return QString::number( toUInt() );
  if ( d->typ == Double )
	  return QString::number( toDouble() );
  if ( d->typ == Byte )
	  return QString::number( toByte() );
  if ( d->typ != String )
	  return QString::null;
  return *((QString*)d->value.ptr);
}

const QString AIElement::toReference() const
{
  if ( d->typ != Reference )
	  return QString::null;
  return *((QString*)d->value.ptr);
}

const QString AIElement::toOperator() const
{
  if ( d->typ != Operator )
	  return QString::null;
  return *((QString*)d->value.ptr);
}

/*!
  Returns the aielement as a QCString if the aielement has type()
  CString or String, or a 0 otherwise.

  \sa asCString()
*/
const QCString AIElement::toCString() const
{
  if ( d->typ == CString )
	  return *((QCString*)d->value.ptr);
  if ( d->typ == String )
	  return ((QString*)d->value.ptr)->latin1();
  if ( d->typ == Operator )
	  return ((QString*)d->value.ptr)->latin1();
  if ( d->typ == Reference )
	  return ((QString*)d->value.ptr)->latin1();

  return 0;
}


/*!
  Returns the aielement as an int if the aielement has type()
  String, CString, Int, UInt, Double, Byte, or 0 otherwise.

  If \a ok is non-null, \a *ok is set to TRUE if the value could be
  converted to an int and FALSE otherwise.

  \sa asInt() canCast()
*/
int AIElement::toInt( bool * ok ) const
{
  if( d->typ == String )
	  return ((QString*)d->value.ptr)->toInt( ok );
  if ( d->typ == CString )
	  return ((QCString*)d->value.ptr)->toInt( ok );
  if ( ok )
	  *ok = canCast( UInt );
  if( d->typ == Int )
	  return d->value.i;
  if( d->typ == UInt )
	  return (int)d->value.u;
  if( d->typ == Byte )
	  return (int)d->value.b;
  if ( d->typ == Double )
	  return (int)d->value.d;
  return 0;
}

uchar AIElement::toByte( bool * ok ) const
{
  if( d->typ == String )
	  return ((QString*)d->value.ptr)->toShort( ok );
  if ( d->typ == CString )
	  return ((QCString*)d->value.ptr)->toShort( ok );
  if ( ok )
	  *ok = canCast( UInt );
  if( d->typ == Byte )
	  return d->value.b;
  if( d->typ == Int )
	  return (uchar)d->value.i;
  if( d->typ == UInt )
	  return (uchar)d->value.u;
  if ( d->typ == Double )
	  return (uchar)d->value.d;
  return 0;
}


/*!
  Returns the aielement as an unsigned int if the aielement has type()
  String, CString, UInt, Int, Double, Byte, or 0 otherwise.

  If \a ok is non-null, \a *ok is set to TRUE if the value could be
  converted to a uint and FALSE otherwise.

  \sa asUInt()
*/
uint AIElement::toUInt( bool * ok ) const
{
  if( d->typ == String )
	  return ((QString*)d->value.ptr)->toUInt( ok );
  if ( d->typ == CString )
	  return ((QCString*)d->value.ptr)->toUInt( ok );
  if ( ok )
	  *ok = canCast( UInt );
  if( d->typ == Int )
	  return d->value.i;
  if( d->typ == UInt )
	  return (int)d->value.u;
  if( d->typ == Byte )
	  return (int)d->value.b;
  if ( d->typ == Double )
	  return (int)d->value.d;

  return 0;
}

/*!
  Returns the aielement as a double if the aielement has type()
  String, CString, Double, Int, UInt, Byte, or 0.0 otherwise.

  If \a ok is non-null, \a *ok is set to TRUE if the value could be
  converted to a double and FALSE otherwise.

  \sa asDouble()
*/
double AIElement::toDouble( bool * ok ) const
{
  if( d->typ == String )
	  return ((QString*)d->value.ptr)->toDouble( ok );
  if ( d->typ == CString )
	  return ((QCString*)d->value.ptr)->toDouble( ok );
  if ( ok )
	  *ok = canCast( Double );
  if ( d->typ == Double )
	  return d->value.d;
  if ( d->typ == Int )
	  return (double)d->value.i;
  if ( d->typ == UInt )
	  return (double)d->value.u;
  if ( d->typ == Byte )
	  return (double)d->value.b;
  return 0.0;
}

/*!
  Returns the aielement as a QValueList<AIElement> if the aielement has type()
  List or StringList, or an empty list otherwise.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \code
    QValueList<AIElement> list = myAIElement.toList();
    QValueList<AIElement>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  \sa asList()
*/
/* const QValueList<AIElement> AIElement::toList() const
{
  if ( d->typ == List )
	  return *((QValueList<AIElement>*)d->value.ptr);
  return QValueList<AIElement>();
} */

const QValueVector<AIElement> AIElement::toElementArray() const
{
  if ( d->typ == ElementArray )
	  return *((QValueVector<AIElement>*)d->value.ptr);
  return QValueVector<AIElement>();
}

const QValueVector<AIElement> AIElement::toBlock() const
{
  if ( d->typ == Block )
	  return *((QValueVector<AIElement>*)d->value.ptr);
  return QValueVector<AIElement>();
}


const QByteArray AIElement::toByteArray() const
{
  if ( d->typ == ByteArray )
	  return *((QByteArray*)d->value.ptr);
  return QByteArray();
}

#define Q_VARIANT_AS( f ) Q##f& AIElement::as##f() { \
   if ( d->typ != f ) *this = AIElement( to##f() ); else detach(); return *((Q##f*)d->value.ptr);}

Q_VARIANT_AS(String)
Q_VARIANT_AS(CString)

/*!
  Returns the aielement's value as int reference.
*/
int& AIElement::asInt()
{
  detach();
  if ( d->typ != Int ) {
	  int i = toInt();
	  d->clear();
 	  d->value.i = i;
	  d->typ = Int;
  }
  return d->value.i;
}

/*!
  Returns the aielement's value as unsigned int reference.
*/
uint& AIElement::asUInt()
{
  detach();
  if ( d->typ != UInt ) {
	  uint u = toUInt();
	  d->clear();
	  d->value.u = u;
	  d->typ = UInt;
  }
  return d->value.u;
}

/*!
  Returns the aielement's value as double reference.
*/
double& AIElement::asDouble()
{
  if ( d->typ != Double ) {
	  double dbl = toDouble();
	  d->clear();
	  d->value.d = dbl;
	  d->typ = Double;
  }
  return d->value.d;
}

/*!
  Returns the aielement's value as byte reference.
*/
uchar& AIElement::asByte()
{
  detach();
  if ( d->typ != Byte ) {
	  uchar b = toByte();
	  d->clear();
 	  d->value.b = b;
	  d->typ = Byte;
  }
  return d->value.b;
}


/*!
  Returns the aielement's value as aielement list reference.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \code
    QValueList<AIElement> list = myAIElement.asList();
    QValueList<AIElement>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
/* QValueList<AIElement>& AIElement::asList()
{
  if ( d->typ != List )
	  *this = AIElement( toList() );
  return *((QValueList<AIElement>*)d->value.ptr);
}  */

QValueVector<AIElement>& AIElement::asElementArray()
{
  if ( d->typ != ElementArray )
	  *this = AIElement( toElementArray() );
  return *((QValueVector<AIElement>*)d->value.ptr);
}

QValueVector<AIElement>& AIElement::asBlock()
{
  if ( d->typ != Block )
	  *this = AIElement( toBlock() );
  return *((QValueVector<AIElement>*)d->value.ptr);
}


QByteArray& AIElement::asByteArray()
{
  if ( d->typ != ByteArray )
	  *this = AIElement( toByteArray() );
  return *((QByteArray*)d->value.ptr);
}

/*!
  Returns TRUE if the aielement's type can be cast to the requested
  type, \p t. Such casting is done automatically when calling the
  toInt(), ... or asInt(), ... methods.

  The following casts are done automatically:
  <ul>
  <li> CString => String
  <li> Double => String, Int, UInt
  <li> Int => String, Double, UInt
  <li> String => CString, Int, Uint, Double
  <li> UInt => String, Double, Int
  </ul>
*/
bool AIElement::canCast( Type t ) const
{
  if ( d->typ == t )
	  return TRUE;
  if ( t == Int && ( d->typ == String || d->typ == Double || d->typ == UInt || d->typ == Byte) )
	  return TRUE;
  if ( t == UInt && ( d->typ == String || d->typ == Double || d->typ == Int || d->typ == Byte) )
	  return TRUE;
  if ( t == Double && ( d->typ == String || d->typ == Int || d->typ == UInt || d->typ == Byte) )
	  return TRUE;
  if ( t == CString && d->typ == String )
	  return TRUE;
  if ( t == String && ( d->typ == CString || d->typ == Int || d->typ == UInt || d->typ == Double || d->typ == Byte) )
	  return TRUE;

  return FALSE;
}

/*!
  \brief Casts the aielement to the requested type.
  
  If the cast cannot be
  done, the aielement is set to the default value of the requested type
  (e.g. an empty string if the requested type \p t is
  AIElement::String, an empty point array if the requested type \p t is
  AIElement::PointArray, etc).

  \returns TRUE if the current type of the
  aielement was successfully casted; otherwise returns FALSE.

  \see canCast()
*/

bool AIElement::cast( Type t )
{
  switch ( t ) {
/*    case AIElement::List:
	    asList();
	    break; */
    case AIElement::ElementArray:
	    asElementArray();
	    break;
    case AIElement::Block:
	    asBlock();
	    break;
    case AIElement::String:
	    asString();
	    break;
    case AIElement::Int:
	    asInt();
	    break;
    case AIElement::UInt:
	    asUInt();
	    break;
    case AIElement::Double:
	    asDouble();
	    break;
    case AIElement::CString:
	    asCString();
	    break;
    case AIElement::Byte:
	    asByte();
	    break;
    case AIElement::ByteArray:
	    asByteArray();
	    break;
    default:
	    case AIElement::Invalid:
	    (*this) = AIElement();
  }
  return canCast( t );
}

/*!  Compares this AIElement with \a v and returns TRUE if they are
  equal; otherwise returns FALSE.
*/

bool AIElement::operator==( const AIElement &v ) const
{
  if ( !v.canCast( type() ) )
	  return FALSE;
  switch( d->typ ) {
/*    case List:
	     return v.toList() == toList(); */
    case ElementArray:
	     return v.toElementArray() == toElementArray();
    case Block:
	     return v.toBlock() == toBlock();
    case ByteArray:
	     return v.toByteArray() == toByteArray();

    case String:
	     return v.toString() == toString();
    case Operator:
	     return v.toOperator() == toOperator();
    case Reference:
	     return v.toReference() == toReference();
    case CString:
	     return v.toCString() == toCString();
    case Int:
	     return v.toInt() == toInt();
    case UInt:
	     return v.toUInt() == toUInt();
    case Byte:
	     return v.toByte() == toByte();
    case Invalid:
	     break;
    }
    return FALSE;
}

/*!  Compares this AIElement with \a v and returns TRUE if they are
  not equal; otherwise returns FALSE.
*/

bool AIElement::operator!=( const AIElement &v ) const
{
    return !( v == *this );
}
