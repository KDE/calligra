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
    KPCubicBezierCurveObject( const QPointArray &_controlPoints, const QPointArray &_allPoints, QSize _size,
                              QPen _pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPCubicBezierCurveObject() {}

    KPCubicBezierCurveObject &operator=( const KPCubicBezierCurveObject & );

    virtual void setPen( QPen _pen ) { pen = _pen; }
    virtual void setLineBegin( LineEnd _lineBegin ) { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd ) { lineEnd = _lineEnd; }

    virtual ObjType getType() const { return OT_CUBICBEZIERCURVE; }
    virtual QString getTypeString() const { return i18n("Cubic Bezier Curve"); }

    virtual QPen getPen() const { return pen; }
    virtual LineEnd getLineBegin() const { return lineBegin; }
    virtual LineEnd getLineEnd() const { return lineEnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load( const QDomElement &element );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    virtual void setSize( int _width, int _height );
    virtual void resizeBy( QSize _size );
    virtual void resizeBy( int _dx, int _dy );

protected:
    float getAngle( QPoint p1, QPoint p2 );
    void paint( QPainter *_painter );

    void updatePoints( double _fx, double _fy );

    QPointArray getCubicBezierPointsFrom( const QPointArray &_pointArray );

    QPointArray origControlPoints, controlPoints;
    QPointArray origAllPoints, allPoints;
    QSize origSize;
    QPen pen;
    LineEnd lineBegin, lineEnd;
};

#endif
