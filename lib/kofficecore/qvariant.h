/****************************************************************************
** $Id$
**
** Definition of QVariant class
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

#ifndef QVARIANT_H
#define QVARIANT_H

#ifndef QT_H
#include "qstring.h"
#include "qvaluelist.h"
#include "qstringlist.h"
#include "qshared.h"
#include "qdatastream.h"
#endif // QT_H

class QString;
class QFont;
class QPixmap;
// class QMovie;
class QBrush;
class QRect;
class QPoint;
class QImage;
class QSize;
class QColor;
class QPalette;
class QColorGroup;

/**
 * This class acts like a union. It can hold one value at the
 * time but it can hold the most common types.
 * For CORBA people: It is a poor mans CORBA::Any.
 */
class Q_EXPORT QVariant : public QShared
{
public:
    enum Type {
      Empty,
      String,
      StringList,
      IntList,
      DoubleList,
      Font,
      Pixmap,
      Brush,
      Rect,
      Size,
      Color,
      Palette,
      ColorGroup,
      Point,
      Image,
      Int,
      Bool,
      Double,
      // Movie,
      NTypes,
      Custom = 0x1000
    };

    QVariant();
    QVariant( const QVariant& );
    QVariant( QDataStream& s );
    virtual ~QVariant();

    QVariant( const QString& _v );
    QVariant( const char* _v );
    QVariant( const QStringList& _v );
    QVariant( const QValueList<int>& _v );
    QVariant( const QValueList<double>& _v );
    QVariant( const QFont& _v );
    QVariant( const QPixmap& _v );
    QVariant( const QImage& _v );
  // QVariant( const QMovie& _v );
    QVariant( const QBrush& _v );
    QVariant( const QPoint& _v );
    QVariant( const QRect& _v );
    QVariant( const QSize& _v );
    QVariant( const QColor& _v );
    QVariant( const QPalette& _v );
    QVariant( const QColorGroup& _v );
    QVariant( int _v );
    QVariant( bool _v );
    QVariant( double _v );

    QVariant& operator= ( const QVariant& );

    void setValue( const QString& );
    void setValue( const char* );
    void setValue( const QStringList& );
    void setValue( const QValueList<int>& );
    void setValue( const QValueList<double>& );
    void setValue( const QFont& );
    void setValue( const QPixmap& );
    void setValue( const QImage& );
  // void setValue( const QMovie& );
    void setValue( const QBrush& );
    void setValue( const QPoint& );
    void setValue( const QRect& );
    void setValue( const QSize& );
    void setValue( const QColor& );
    void setValue( const QPalette& );
    void setValue( const QColorGroup& );
    void setValue( int );
    void setValue( bool );
    void setValue( double );

    Type type() const { return typ; }
    virtual QString typeName() const;

    bool isEmpty() const { return ( typ == Empty ); }

    QString stringValue() const;
    QStringList stringListValue() const;
    QValueList<int> intListValue() const;
    QValueList<double> doubleListValue() const;
    QFont fontValue() const;
    QPixmap pixmapValue() const;
    QImage imageValue() const;
  // QMovie movieValue() const;
    QBrush brushValue() const;
    QPoint pointValue() const;
    QRect rectValue() const;
    QSize sizeValue() const;
    QColor colorValue() const;
    QPalette paletteValue() const;
    QColorGroup colorgroupValue() const;
    int intValue() const;
    bool boolValue() const;
    double doubleValue() const;

    virtual void load( QDataStream& );
    virtual void save( QDataStream& ) const;

    static QString typeToName( Type _typ );
    /**
     * @return QVariant::Empty if the given name is empty or unknown.
     */
    static Type nameToType( const QString& _name );

protected:
    virtual void clear();

    Type typ;
    union
    {
      int i;
      bool b;
      double d;
      void *ptr;
    } val;

private:
    static void initTypeNameMap();
};

Q_EXPORT QDataStream& operator>> ( QDataStream& s, QVariant& p );
Q_EXPORT QDataStream& operator<< ( QDataStream& s, const QVariant& p );
Q_EXPORT QDataStream& operator>> ( QDataStream& s, QVariant::Type& p );
Q_EXPORT QDataStream& operator<< ( QDataStream& s, const QVariant::Type p );

#endif

