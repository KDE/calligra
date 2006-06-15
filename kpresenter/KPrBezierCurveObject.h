// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef kpcubicbeziercurveobject_h
#define kpcubicbeziercurveobject_h

#include "KPrPointObject.h"

class QPainter;
class KPrBezierCurveObject;
class KPrObjectAdaptor;

class KPrBezierCurveObject : public KPrPointObject
{
public:
    KPrBezierCurveObject();
    KPrBezierCurveObject( const KoPointArray &_controlPoints, const KoPointArray &_allPoints, const KoSize & _size,
                          const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPrBezierCurveObject() {}

    virtual QDomDocumentFragment save( QDomDocument& doc,double offset );

    virtual double load( const QDomElement &element );
    virtual void loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo* info );

    virtual void flip(bool horizontal );

    static KoPointArray bezier2polyline( const KoPointArray &bezierPoints );

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    virtual void updatePoints( double _fx, double _fy );
    virtual KoPointArray getDrawingPoints() const;

    KoPointArray allPoints;

private:
    // Don't assign it
    KPrBezierCurveObject &operator=( const KPrBezierCurveObject & );
};


class KPrCubicBezierCurveObject : public KPrBezierCurveObject
{
public:
    KPrCubicBezierCurveObject();
    KPrCubicBezierCurveObject( const KoPointArray &_controlPoints, const KoPointArray &_allPoints, const KoSize & _size,
                               const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPrCubicBezierCurveObject() {}

    virtual KPrObjectAdaptor* dbusObject();

    virtual ObjType getType() const { return OT_CUBICBEZIERCURVE; }
    virtual QString getTypeString() const { return i18n( "Cubic Bezier Curve" ); }
};

class KPrQuadricBezierCurveObject : public KPrBezierCurveObject
{
public:
    KPrQuadricBezierCurveObject();
    KPrQuadricBezierCurveObject( const KoPointArray &_controlPoints, const KoPointArray &_allPoints, const KoSize & _size,
                                 const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPrQuadricBezierCurveObject() {}

    virtual KPrObjectAdaptor* dbusObject();

    virtual ObjType getType() const { return OT_CUBICBEZIERCURVE; }
    virtual QString getTypeString() const { return i18n( "Quadric Bezier Curve" ); }
};

#endif
