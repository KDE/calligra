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

#ifndef kppolygonobject_h
#define kppolygonobject_h

#include <q3pointarray.h>
#include "KoPointArray.h"
#include "KPrObject.h"

class KPrGradient;
class QPainter;
class KPrObjectAdaptor;
class KoPen;

class KPrPolygonObject : public KPr2DObject
{
public:
    KPrPolygonObject();
    KPrPolygonObject( const KoPointArray &_points, const KoSize &_size, const KoPen &_pen, const QBrush &_brush,
                     FillType _fillType, const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                     bool _unbalanced, int _xfactor, int _yfactor,
                     bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue );

    virtual ~KPrPolygonObject() {}
    virtual KPrObjectAdaptor* dbusObject();

    KPrPolygonObject &operator=( const KPrPolygonObject & );

    virtual void setSize( double _width, double _height );
    virtual void setSize( const KoSize & _size )
        { setSize( _size.width(), _size.height() ); }

    virtual ObjType getType() const { return OT_POLYGON; }
    virtual QString getTypeString() const { return i18n( "Polygon" ); }

    void setCheckConcavePolygon(bool _concavePolygon) { checkConcavePolygon = _concavePolygon; drawPolygon(); }
    void setCornersValue(int _cornersValue) { cornersValue = _cornersValue; drawPolygon(); }
    void setSharpnessValue(int _sharpnessValue) { sharpnessValue = _sharpnessValue; drawPolygon(); }
    bool getCheckConcavePolygon() const { return checkConcavePolygon; }
    int getCornersValue() const { return cornersValue; }
    int getSharpnessValue() const { return sharpnessValue; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );

    virtual double load( const QDomElement &element );
    virtual void loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info );

    virtual void flip( bool horizontal );
    virtual KoSize getRealSize() const;
    virtual KoPoint getRealOrig() const;

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    virtual void paint( QPainter *_painter,KoTextZoomHandler*_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour );

    void updatePoints( double _fx, double _fy );

    void drawPolygon();

    KoPointArray points;
    bool checkConcavePolygon;
    int cornersValue;
    int sharpnessValue;
};

#endif
