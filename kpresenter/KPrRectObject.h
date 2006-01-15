// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
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

#ifndef kprectobject_h
#define kprectobject_h

#include <KoBrush.h>

#include "KPrObject.h"

class KPrGradient;
class DCOPObject;

class KPrRectObject : public KPr2DObject
{
public:
    KPrRectObject();
    KPrRectObject( const KoPen &_pen, const QBrush &_brush, FillType _fillType,
                  const QColor &_gColor1, const QColor &_gColor2,
                  BCType _gType, int _xRnd, int _yRnd,
                  bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPrRectObject() {}
    virtual DCOPObject* dcopObject();

    KPrRectObject &operator=( const KPrRectObject & );

    virtual void setRnds( int _xRnd, int _yRnd )
        { xRnd = _xRnd; yRnd = _yRnd; m_redrawGradientPix = true; }

    virtual ObjType getType() const
        { return OT_RECT; }
    virtual QString getTypeString() const
        { return i18n("Rectangle"); }
    virtual void getRnds( int &_xRnd, int &_yRnd ) const
        { _xRnd = xRnd; _yRnd = yRnd; }

    virtual QDomDocumentFragment save( QDomDocument& doc, double offset );
    virtual double load(const QDomElement &element);
    virtual void loadOasis(const QDomElement &element, KoOasisContext &context, KPrLoadingInfo *info);

protected:
    virtual const char * getOasisElementName() const;
    virtual bool saveOasisObjectAttributes( KPOasisSaveContext &sc ) const;

    virtual void paint( QPainter *_painter,KoTextZoomHandler*_zoomHandler,
                        int /* pageNum */, bool drawingShadow, bool drawContour );

    int xRnd, yRnd;
private:
    /**
       Returns a bounding region for a rounded-corners rectangle. Useful for setting
       further clipping (e.g. to draw a pixmap on such a rectangle).
    */
    QPointArray boundingRegion( int x, int y, int w, int h, int _xRnd, int _yRnd) const;
};

#endif
