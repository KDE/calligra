/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kppixmapobject_h
#define kppixmapobject_h

#include <qdatetime.h>

#include "kpobject.h"
#include "kppixmapcollection.h"
#include "kpgradient.h"

class QPixmap;

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

class KPPixmapObject : public KPObject
{
    friend class Page;

public:
    KPPixmapObject( KPPixmapCollection *_pixmapCollection );
    KPPixmapObject( KPPixmapCollection *_pixmapCollection, const QString &_filename, QDateTime _lastModified );
    virtual ~KPPixmapObject();

    KPPixmapObject &operator=( const KPPixmapObject & );

    virtual void setSize( int _width, int _height );
    virtual void setSize( QSize _size )
    { setSize( _size.width(), _size.height() ); }
    virtual void resizeBy( int _dx, int _dy );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; }
    virtual void setGColor2( QColor _gColor2 )
    { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; }
    virtual void setGType( BCType _gType )
    { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; }
    virtual QString getFileName() const
    { return key.dataKey.filename; }
    virtual KPPixmapDataCollection::Key getKey() const
    { return key.dataKey; }
    virtual void setGUnbalanced( bool b )
    { if ( gradient ) gradient->setUnbalanced( b ); unbalanced = b; }
    virtual void setGXFactor( int f )
    { if ( gradient ) gradient->setXFactor( f ); xfactor = f; }
    virtual void setGYFactor( int f )
    { if ( gradient ) gradient->setYFactor( f ); yfactor = f; }

    void setPixmap( const QString &_filename, QDateTime _lastModified )
    { setPixmap( _filename, _lastModified, orig_size ); }
    void setPixmap( const QString &_filename, QDateTime _lastModified, const QSize &_size );
    void reload()
    { setPixmap( key.dataKey.filename, key.dataKey.lastModified, key.size ); }

    virtual ObjType getType() const
    { return OT_PICTURE; }
    virtual QPen getPen() const
    { return pen; }
    virtual QBrush getBrush() const
    { return brush; }
    virtual FillType getFillType() const
    { return fillType; }
    virtual QColor getGColor1() const
    { return gColor1; }
    virtual QColor getGColor2() const
    { return gColor2; }
    virtual BCType getGType() const
    { return gType; }
    virtual bool getGUnbalanced() const
    { return unbalanced; }
    virtual int getGXFactor() const
    { return xfactor; }
    virtual int getGYFactor() const
    { return yfactor; }

    virtual void save( QTextStream& out );
    virtual void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

protected:
    KPPixmapObject()
    {; }

    KPPixmapCollection *pixmapCollection;
    QPixmap *pixmap;
    KPPixmapCollection::Key key;
    KPGradient *gradient;

    QPen pen;
    QBrush brush;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    bool unbalanced;
    int xfactor, yfactor;

};

#endif
