/****************************************************************************
** $Id$
**
** Implementation of QVariant class
**
** Created : 990414
**
** Copyright (C) 1992-1999 Troll Tech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Troll Tech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** Licensees holding valid Qt Professional Edition licenses may use this
** file in accordance with the Qt Professional Edition License Agreement
** provided with the Qt Professional Edition.
**
** See http://www.troll.no/pricing.html or email sales@troll.no for
** information about the Professional Edition licensing, or see
** http://www.troll.no/qpl/ for QPL licensing information.
**
*****************************************************************************/

#include <qstring.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qimage.h>
// #include <qmovie.h>
#include <qbrush.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qpalette.h>

#include "qvariant.h"

/*!
  \class QVariant qvariant.h
  \brief Acts like a union for the most common Qt data types.

  Unfortunately one can not use C++ unions with classes that have
  constructors and destructors since the compiler and the runtime
  library could not determine which destructor to call.

  To come around this QVariant can store the most common Qt and C++
  data types. Like every union it can hold one value of a certain type
  at a time.
*/

/*!
  Creates an empty variant.
*/
QVariant::QVariant()
{
  typ = Empty;
}

/*!
  Subclasses which re-implement clear() should reimplement the
  destructor to call clear() - so that the overridden clear() is
  called.
*/
QVariant::~QVariant()
{
    clear();
}

/*!
  Constructs a deep copy of the variant passed as argument to this constructor.
*/
QVariant::QVariant( const QVariant& p ) : QShared()
{
  typ = Empty;
  *this = p;
}

/*!
  Reads the variant from the data stream.
*/
QVariant::QVariant( QDataStream& s )
{
  s >> *this;
}

/*!
  Creates a new variant with a string value.
*/
QVariant::QVariant( const QString& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a string value.
*/
QVariant::QVariant( const char* _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a string list value.
*/
QVariant::QVariant( const QStringList& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a integer list value.
*/
QVariant::QVariant( const QValueList<int>& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a floating point list value.
*/
QVariant::QVariant( const QValueList<double>& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a font value.
*/
QVariant::QVariant( const QFont& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a pixmap value.
*/
QVariant::QVariant( const QPixmap& _v )
{
  typ = Empty;
  setValue( _v );
}


/*!
  Creates a new variant with an image value.
*/
QVariant::QVariant( const QImage& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a brush value.
*/
QVariant::QVariant( const QBrush& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a point value.
*/
QVariant::QVariant( const QPoint& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a rect value.
*/
QVariant::QVariant( const QRect& _v )
{ 
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a size value.
*/
QVariant::QVariant( const QSize& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a color value.
*/
QVariant::QVariant( const QColor& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a color palette value.
*/
QVariant::QVariant( const QPalette& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a color group value.
*/
QVariant::QVariant( const QColorGroup& _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with an integer value.
*/
QVariant::QVariant( int _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Creates a new variant with a boolean value.
*/
QVariant::QVariant( bool _v )
{
  typ = Empty;
  setValue( _v );
}


/*!
  Creates a new variant with a floating point value.
*/
QVariant::QVariant( double _v )
{
  typ = Empty;
  setValue( _v );
}

/*!
  Assigns the value of one variant to another. This creates a deep copy.
*/
QVariant& QVariant::operator= ( const QVariant& p )
{
  clear();

  switch( p.type() )
    {
    case Empty:
      break;
    case String:
      val.ptr = new QString( p.stringValue() );
      break;
    case StringList:
      val.ptr = new QStringList( p.stringListValue() );
      break;
    case IntList:
      val.ptr = new QValueList<int>( p.intListValue() );
      break;
    case DoubleList:
      val.ptr = new QValueList<double>( p.doubleListValue() );
      break;
    case Font:
      val.ptr = new QFont( p.fontValue() );
      break;
      // case Movie:
      // val.ptr = new QMovie( p.movieValue() );
      // break;
    case Pixmap:
      val.ptr = new QPixmap( p.pixmapValue() );
      break;
    case Image:
      val.ptr = new QImage( p.imageValue() );
      break;
    case Brush:
      val.ptr = new QBrush( p.brushValue() );
      break;
    case Point:
      val.ptr = new QPoint( p.pointValue() );
      break;
    case Rect:
      val.ptr = new QRect( p.rectValue() );
      break;
    case Size:
      val.ptr = new QSize( p.sizeValue() );
      break;
    case Color:
      val.ptr = new QColor( p.colorValue() );
      break;
    case Palette:
      val.ptr = new QPalette( p.paletteValue() );
      break;
    case ColorGroup:
      val.ptr = new QColorGroup( p.colorgroupValue() );
      break;
    case Int:
      val.i = p.intValue();
      break;
    case Bool:
      val.b = p.boolValue();
      break;
    case Double:
      val.d = p.doubleValue();
      break;
    default:
      ASSERT( 0 );
    }

  typ = p.type();

  return *this;
}

/*!
  Returns the type stored in the variant currently in ASCII format.
  The returned strings describe the C++ datatype used to store the
  data, for example "QFont", "QString" or "QValueList<int>".
*/
QString QVariant::typeName() const
{
  return typeToName( typ );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QString& _value )
{
  clear();
  typ = String;
  val.ptr = new QString( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
  The Variant creates a copy of the passed string.
*/
void QVariant::setValue( const char* _value )
{
  clear();
  typ = String;
  val.ptr = new QString( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
  This function creates a copy of the list. This is very fast since
  QStringList is implicit shared.
*/
void QVariant::setValue( const QStringList& _value )
{
  clear();
  typ = StringList;
  val.ptr = new QStringList( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
  This function creates a copy of the list. This is very fast since
  QStringList is implicit shared.
*/
void QVariant::setValue( const QValueList<int>& _value )
{
  clear();
  typ = IntList;
  val.ptr = new QValueList<int>( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
  This function creates a copy of the list. This is very fast since
  QStringList is implicit shared.
*/
void QVariant::setValue( const QValueList<double>& _value )
{
  clear();
  typ = DoubleList;
  val.ptr = new QValueList<double>( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QFont& _value )
{
  clear();
  typ = Font;
  val.ptr = new QFont( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QPixmap& _value )
{
  clear();
  typ = Pixmap;
  val.ptr = new QPixmap( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QImage& _value )
{
  clear();
  typ = Image;
  val.ptr = new QImage( _value );
}

// void QVariant::setValue( const QMovie& _value )
// {
//   clear();
//   typ = Movie;
//  val.ptr = new QMovie( _value );
// }

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QBrush& _value )
{
  clear();
  typ = Brush;
  val.ptr = new QBrush( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QRect& _value )
{
  clear();
  typ = Rect;
  val.ptr = new QRect( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QPoint& _value )
{
  clear();
  typ = Point;
  val.ptr = new QPoint( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QSize& _value )
{
  clear();
  typ = Size;
  val.ptr = new QSize( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QColor& _value )
{
  clear();
  typ = Color;
  val.ptr = new QColor( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QPalette& _value )
{
  clear();
  typ = Palette;
  val.ptr = new QPalette( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( const QColorGroup& _value )
{
  clear();
  typ = ColorGroup;
  val.ptr = new QColorGroup( _value );
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( int _value )
{
  clear();
  typ = Int;
  val.i = _value;
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( bool _value )
{
  clear();
  typ = Bool;
  val.b = _value;
}

/*!
  Changes the value of this variant. The previous value is dropped.
*/
void QVariant::setValue( double _value )
{
  clear();
  typ = Double;
  val.d = _value;
}

/*!
  De-allocate, based on the type, producing an Empty variant.
*/
void QVariant::clear()
{
  switch( typ )
    {
    case Empty:
    case Int:
    case Bool:
    case Double:
      break;
    case String:
      delete (QString*)val.ptr;
      break;
    case IntList:
      delete (QValueList<int>*)val.ptr;
      break;
    case DoubleList:
      delete (QValueList<double>*)val.ptr;
      break;
    case StringList:
      delete (QStringList*)val.ptr;
      break;
    case Font:
      delete (QFont*)val.ptr;
      break;
      // case Movie:
      // delete (QMovie*)val.ptr;
      // break;
    case Pixmap:
      delete (QPixmap*)val.ptr;
      break;
    case Image:
      delete (QImage*)val.ptr;
      break;
    case Brush:
      delete (QBrush*)val.ptr;
      break;
    case Point:
      delete (QPoint*)val.ptr;
      break;
    case Rect:
      delete (QRect*)val.ptr;
      break;
    case Size:
      delete (QSize*)val.ptr;
      break;
    case Color:
      delete (QColor*)val.ptr;
      break;
    case Palette:
      delete (QPalette*)val.ptr;
      break;
    case ColorGroup:
      delete (QColorGroup*)val.ptr;
      break;
    default:
      ASSERT( 0 );
    }

  typ = Empty;
}

static QString *typ_to_name = 0;

void QVariant::initTypeNameMap()
{
    if ( typ_to_name ) return;

    typ_to_name = new QString[NTypes];

    typ_to_name[(int)Empty] = QString::null;
    typ_to_name[(int)String] = QString::fromLatin1("QString");
    typ_to_name[(int)StringList] = QString::fromLatin1("QStringList");
    typ_to_name[(int)IntList] = QString::fromLatin1("QValueList<int>");
    typ_to_name[(int)DoubleList] = QString::fromLatin1("QValueList<double>");
    typ_to_name[(int)Font] = QString::fromLatin1("QFont");
    typ_to_name[(int)Pixmap] = QString::fromLatin1("QPixmap");
    typ_to_name[(int)Image] = QString::fromLatin1("QImage");
    typ_to_name[(int)Brush] = QString::fromLatin1("QBrush");
    typ_to_name[(int)Rect] = QString::fromLatin1("QRect");
    typ_to_name[(int)Point] = QString::fromLatin1("QPoint");
    typ_to_name[(int)Size] = QString::fromLatin1("QSize");
    typ_to_name[(int)Color] = QString::fromLatin1("QColor");
    typ_to_name[(int)Palette] = QString::fromLatin1("QPalette");
    typ_to_name[(int)ColorGroup] = QString::fromLatin1("QColorGroup");
    typ_to_name[(int)Int] = QString::fromLatin1("int");
    typ_to_name[(int)Bool] = QString::fromLatin1("bool");
    typ_to_name[(int)Double] = QString::fromLatin1("double");
    //typ_to_name[(int)Movie] = QString::fromLatin1("QMovie");
}

/*!
  Converts the enum representation of the storage type to its
  string representation.
*/
QString QVariant::typeToName( QVariant::Type _typ )
{
    initTypeNameMap();
    return typ_to_name[_typ];
}

/*!
  Converts the string representation of the storage type to
  its enum representation.
*/
QVariant::Type QVariant::nameToType( const QString& _name )
{
    initTypeNameMap();
    int t = (int)NTypes;
    while ( t > (int)Empty && typ_to_name[(int)--t] != _name )
	;
    return Type(t);
}

/*!
  Internal function for loading a variant. Use the stream operators
  instead.
*/
void QVariant::load( QDataStream& s )
{
  Q_UINT32 u;
  s >> u;
  Type t = (Type)u;

  switch( t )
    {
    case Empty:
      typ = t;
      break;
    case String:
      { QString x; s >> x; setValue( x ); }
      break;
    case StringList:
      { QStringList x; s >> x; setValue( x ); }
      break;
    case IntList:
      { QValueList<int> x; s >> x; setValue( x ); }
      break;
    case DoubleList:
      { QValueList<double> x; s >> x; setValue( x ); }
      break;
    case Font:
      { QFont x; s >> x; setValue( x ); }
      break;
      // case Movie:
      // return "QMovie";
    case Pixmap:
      { QPixmap x; s >> x; setValue( x ); }
      break;
    case Image:
      { QImage x; s >> x; setValue( x ); }
      break;
    case Brush:
      { QBrush x; s >> x; setValue( x ); }
      break;
    case Rect:
      { QRect x; s >> x; setValue( x ); }
      break;
    case Point:
      { QPoint x; s >> x; setValue( x ); }
      break;
    case Size:
      { QSize x; s >> x; setValue( x ); }
      break;
    case Color:
      { QColor x; s >> x; setValue( x ); }
      break;
    case Palette:
      { QPalette x; s >> x; setValue( x ); }
      break;
    case ColorGroup:
      { QColorGroup x; s >> x; setValue( x ); }
      break;
    case Int:
      { int x; s >> x; setValue( x ); };
      break;
    case Bool:
      { Q_INT8 x; s >> x; setValue( (bool)x ); };
      break;
    case Double:
      { double x; s >> x; setValue( x ); };
      break;
    default:
      ASSERT( 0 );
    }
}

/*!
  Internal function for saving a variant. Use the stream operators
  instead.
*/
void QVariant::save( QDataStream& s ) const
{
  s << (Q_UINT32)type();

  switch( typ )
    {
    case Empty:
      s << QString();
      break;
    case String:
      s << stringValue();
      break;
    case StringList:
      s << stringListValue();
      break;
    case IntList:
      s << intListValue();
      break;
    case DoubleList:
      s << doubleListValue();
      break;
    case Font:
      s << fontValue();
      break;
      // case Movie:
      // return "QMovie";
    case Pixmap:
      s << pixmapValue();
      break;
    case Image:
      s << imageValue();
      break;
    case Brush:
      s << brushValue();
      break;
    case Point:
      s << pointValue();
      break;
    case Rect:
      s << rectValue();
      break;
    case Size:
      s << sizeValue();
      break;
    case Color:
      s << colorValue();
      break;
    case Palette:
      s << paletteValue();
      break;
    case ColorGroup:
      s << colorgroupValue();
      break;
    case Int:
      s << intValue();
      break;
    case Bool:
      s << (Q_INT8)boolValue();
      break;
    case Double:
      s << doubleValue();
      break;
    default:
      ASSERT( 0 );
    }
}

/*!
  Reads a variant from the stream.
*/
QDataStream& operator>> ( QDataStream& s, QVariant& p )
{
  p.load( s );
  return s;
}

/*!
  Writes a variant to the stream.
*/
QDataStream& operator<< ( QDataStream& s, const QVariant& p )
{
  p.save( s );
  return s;
}

/*!
  Reads a variant type in enum representation from the stream
*/
QDataStream& operator>> ( QDataStream& s, QVariant::Type& p )
{
  Q_UINT32 u;
  s >> u;
  p = (QVariant::Type) u;

  return s;
}

/*!
  Writes a variant type to the stream.
*/
QDataStream& operator<< ( QDataStream& s, const QVariant::Type p )
{
  s << (Q_UINT32)p;

  return s;
}

/*! \fn Type QVariant::type() const
  Returns the stoarge type of the value stored in the
  variant currently.
*/

/*! \fn bool QVariant::isEmpty() const
  Returns TRUE if the storage type of this variant is QVariant::Empty.
*/

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QString QVariant::stringValue() const
{
  if ( typ != String )
    return QString::null;
  return *((QString*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QStringList QVariant::stringListValue() const
{
  if ( typ != StringList )
    return QStringList();
  return *((QStringList*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QValueList<int> QVariant::intListValue() const 
{
  if ( typ != IntList )
    return QValueList<int>();
  return *((QValueList<int>*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QValueList<double> QVariant::doubleListValue() const
{
  if ( typ != IntList )
    return QValueList<double>();
  return *((QValueList<double>*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QFont QVariant::fontValue() const
{
  if ( typ != Font )
    return QFont();
  return *((QFont*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QPixmap QVariant::pixmapValue() const
{
  if ( typ != Pixmap )
    return QPixmap();
  return *((QPixmap*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QImage QVariant::imageValue() const
{
  if ( typ != Image )
    return QImage();
  return *((QImage*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QBrush QVariant::brushValue() const
{
  if( typ != Brush )
    return QBrush();
  return *((QBrush*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QPoint QVariant::pointValue() const
{
  if ( typ != Point )
    return QPoint();
  return *((QPoint*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QRect QVariant::rectValue() const
{
  if ( typ != Rect )
    return QRect();
  return *((QRect*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QSize QVariant::sizeValue() const
{
  if ( typ != Size )
    return QSize();
  return *((QSize*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QColor QVariant::colorValue() const
{
  if ( typ != Color )
    return QColor();
  return *((QColor*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QPalette QVariant::paletteValue() const
{
  if ( typ != Palette )
    return QPalette();
  return *((QPalette*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
QColorGroup QVariant::colorgroupValue() const
{
  if ( typ != ColorGroup )
    return QColorGroup();
  return *((QColorGroup*)val.ptr);
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
int QVariant::intValue() const
{
  if( typ != Int )
    return 0;
  return val.i;
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
bool QVariant::boolValue() const
{
  if ( typ != Bool )
    return false;
  return val.b;
}

/*!
  Returns the value stored in the variant. If the properties
  value does not match the return type of this function then
  this function will abort your process. So check with type()
  or typeName() first wether the variant holds the correct
  data type.
*/
double QVariant::doubleValue() const
{
  if ( typ != Double )
    return 0.0;
  return val.d;
}
