/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

#ifndef kppolylineobject_h
#define kppolylineobject_h

#include <qpointarray.h>
#include "koPointArray.h"
#include <kpobject.h>

#define RAD_FACTOR 180.0 / M_PI

class QPainter;

/******************************************************************/
/* Class: KPPolylineObject                                        */
/******************************************************************/

class KPPolylineObject : public KPObject
{
public:
    KPPolylineObject();
    KPPolylineObject( const KoPointArray &_points, const KoSize &_size, const QPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPPolylineObject() {}

    KPPolylineObject &operator=( const KPPolylineObject & );

    virtual void setPen( const QPen &_pen ) { pen = _pen; }
    virtual void setLineBegin( LineEnd _lineBegin ) { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd ) { lineEnd = _lineEnd; }

    virtual ObjType getType() const { return OT_POLYLINE; }
    virtual QString getTypeString() const { return i18n("Polyline"); }

    virtual QPen getPen() const { return pen; }
    virtual LineEnd getLineBegin() const { return lineBegin; }
    virtual LineEnd getLineEnd() const { return lineEnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load( const QDomElement &element );

    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
		       bool drawSelection, bool drawContour = FALSE );

    virtual void setSize( double _width, double _height );
    virtual void setSize( const KoSize & _size )
    { setSize( _size.width(), _size.height() ); }
    virtual void resizeBy( const KoSize &_size );
    virtual void resizeBy( double _dx, double _dy );

protected:
    float getAngle( const QPoint &p1, const QPoint &p2 );
    void paint( QPainter *_painter,KoZoomHandler*_zoomHandler, bool drawContour );

    void updatePoints( double _fx, double _fy );

    KoPointArray origPoints, points;
    KoSize origSize;
    QPen pen;
    LineEnd lineBegin, lineEnd;
};

#endif
