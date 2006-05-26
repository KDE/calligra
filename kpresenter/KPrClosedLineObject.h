// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>
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

#ifndef kpclosedlineoject_h
#define kpclosedlineoject_h

#include <q3pointarray.h>
#include "KoPointArray.h"
#include "KPrObject.h"

class KPrGradient;
class QPainter;
class DCOPObject;
class KPrPointObject;

class KPrClosedLineObject : public KPr2DObject
{
public:
    KPrClosedLineObject();
    KPrClosedLineObject( const KoPointArray &_points, const KoSize &_size, const KoPen &_pen, const QBrush &_brush,
                        FillType _fillType, const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                        bool _unbalanced, int _xfactor, int _yfactor, const QString _type );
    /**
     * @brief Construct object form a KPrPointObject.
     *
     * This constructor is used when a KPrPointObject objects gets closed.
     * It is then changed into a KPrClosedLineObject.
     *
     * @param object form which the KPrClosedLineObject is constructed
     */
    KPrClosedLineObject( const KPrPointObject &object );

    virtual ~KPrClosedLineObject() {}
    //virtual DCOPObject* dcopObject();

    KPrClosedLineObject &operator=( const KPrClosedLineObject & );

    virtual void setSize( double _width, double _height );
    virtual void setSize( const KoSize & _size ) { setSize( _size.width(), _size.height() ); }

    virtual ObjType getType() const { return OT_CLOSED_LINE; }
    virtual QString getTypeString() const { return typeString; }


    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load( const QDomElement &element );
    virtual void loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info );

    virtual void flip(bool horizontal );
    virtual KoSize getRealSize() const;
    virtual KoPoint getRealOrig() const;

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;
    virtual void paint( QPainter *_painter,KoTextZoomHandler*_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour );

    void updatePoints( double _fx, double _fy );

    KoPointArray points;

    QString typeString;
};

#endif
