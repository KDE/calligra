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

#ifndef kpcubicbeziercurveobject_h
#define kpcubicbeziercurveobject_h

#include <qpointarray.h>
#include "koPointArray.h"
#include <kpobject.h>

#define RAD_FACTOR 180.0 / M_PI

class QPainter;

/******************************************************************/
/* Class: KPCubicBezierCurveObject                                */
/******************************************************************/

class KPCubicBezierCurveObject : public KPObject
{
public:
    KPCubicBezierCurveObject();
    KPCubicBezierCurveObject( const KoPointArray &_controlPoints, const KoPointArray &_allPoints, const QSize & _size,
                              const QPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPCubicBezierCurveObject() {}

    KPCubicBezierCurveObject &operator=( const KPCubicBezierCurveObject & );

    virtual void setPen( const QPen &_pen ) { pen = _pen; }
    virtual void setLineBegin( LineEnd _lineBegin ) { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd ) { lineEnd = _lineEnd; }

    virtual ObjType getType() const { return OT_CUBICBEZIERCURVE; }
    virtual QString getTypeString() const { return i18n("Cubic Bezier Curve"); }

    virtual QPen getPen() const { return pen; }
    virtual LineEnd getLineBegin() const { return lineBegin; }
    virtual LineEnd getLineEnd() const { return lineEnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc,int offset );
    virtual int load( const QDomElement &element );

    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler );

    virtual void setSize( double _width, double _height );
    virtual void resizeBy( const KoSize &_size );
    virtual void resizeBy( double _dx, double _dy );

protected:
    float getAngle( const QPoint &p1, const QPoint &p2 );
    void paint( QPainter *_painter,KoZoomHandler*_zoomHandler );

    void updatePoints( double _fx, double _fy );

    KoPointArray getCubicBezierPointsFrom( const KoPointArray &_pointArray );

    KoPointArray origControlPoints, controlPoints;
    KoPointArray origAllPoints, allPoints;
    QSize origSize;
    QPen pen;
    LineEnd lineBegin, lineEnd;
};

#endif
