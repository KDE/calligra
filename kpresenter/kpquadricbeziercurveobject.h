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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kpquadricbeziercurveobject_h
#define kpquadricbeziercurveobject_h

#include "kppointobject.h"

class QPainter;
class KoZoomHandler;
class DCOPObject;

class KPQuadricBezierCurveObject : public KPPointObject
{
public:
    KPQuadricBezierCurveObject();
    KPQuadricBezierCurveObject( const KoPointArray &_controlPoints,
                                const KoPointArray &_allPoints, const KoSize &_size,
                                const KPPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );
    virtual ~KPQuadricBezierCurveObject() {}

    KPQuadricBezierCurveObject &operator=( const KPQuadricBezierCurveObject & );
    virtual DCOPObject* dcopObject();

    virtual ObjType getType() const { return OT_QUADRICBEZIERCURVE; }
    virtual QString getTypeString() const { return i18n("Quadric Bezier Curve"); }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual void loadOasis( const QDomElement &element, KoOasisContext & context,  KPRLoadingInfo *info );

    virtual double load( const QDomElement &element );

    virtual void flip(bool horizontal );
    virtual void closeObject(bool close);
    bool isClosed()const;

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    virtual void updatePoints( double _fx, double _fy );
    virtual KoPointArray getDrawingPoints() const;

    KoPointArray getQuadricBezierPointsFrom( const KoPointArray &_pointArray );

    KoPointArray allPoints;
};

#endif
