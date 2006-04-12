// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004-2005 Thorsten Zachmann  <zachmann@kde.org>

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

#ifndef kppointobject_h
#define kppointobject_h

#include "KPrObject.h"
#include <KoStyleStack.h>

class KPrPointObject : public KPrShadowObject,  public KPrStartEndLine
{
public:
    KPrPointObject();
    KPrPointObject( const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd );

    virtual KoSize getRealSize() const;
    virtual KoPoint getRealOrig() const;

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );

    virtual double load( const QDomElement &element );
    virtual void loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo* info );

    virtual void setLineBegin( LineEnd _lineBegin ) { lineBegin = _lineBegin; }
    virtual void setLineEnd( LineEnd _lineEnd ) { lineEnd = _lineEnd; }

    virtual LineEnd getLineBegin() const { return lineBegin; }
    virtual LineEnd getLineEnd() const { return lineEnd; }

    virtual void setSize( double _width, double _height );
    virtual void setSize( const KoSize & _size )
        { setSize( _size.width(), _size.height() ); }

    virtual void flip( bool horizontal );

    /**
     * @brief Get the points of the object
     *
     * @return points of the object
     */
    virtual const KoPointArray getPoints() const { return getDrawingPoints(); }

protected:
    virtual const char * getOasisElementName() const;

    void loadOasisMarker( KoOasisContext & context );
    virtual void fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const;

    virtual void paint( QPainter *_painter,KoTextZoomHandler*_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour = false );
    virtual void updatePoints( double _fx, double _fy );
    virtual KoPointArray getDrawingPoints() const;

    KoPointArray points;
};

#endif
