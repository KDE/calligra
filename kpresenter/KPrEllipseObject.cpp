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
#include "KPrObject2DIface.h"
#include "KPrEllipseObject.h"
#include "KPrGradient.h"

#include <kdebug.h>
#include <qbitmap.h>
#include <qregion.h>
#include <qdom.h>
#include <q3picture.h>
#include <qpainter.h>
#include <KoTextZoomHandler.h>
#include <KoOasisContext.h>

using namespace std;

KPrEllipseObject::KPrEllipseObject()
    : KPr2DObject()
{
}

KPrEllipseObject::KPrEllipseObject( const KoPen &_pen, const QBrush &_brush, FillType _fillType,
                                  const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                                  bool _unbalanced, int _xfactor, int _yfactor)
    : KPr2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
}

KPrEllipseObject &KPrEllipseObject::operator=( const KPrEllipseObject & )
{
    return *this;
}

DCOPObject* KPrEllipseObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPrObject2DIface( this );
    return dcop;
}

void KPrEllipseObject::paint( QPainter* _painter, KoTextZoomHandler *_zoomHandler,
                             int /* pageNum */, bool drawingShadow, bool drawContour )
{
    int ow = _zoomHandler->zoomItX( ext.width() );
    int oh = _zoomHandler->zoomItY( ext.height() );
    QSize size( _zoomHandler->zoomSize( ext ) );

    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
#warning "kde4: port it"		
        //_painter->setRasterOp( Qt::NotXorROP );
        _painter->drawEllipse( 0, 0, ow, oh );
        return;
    }

    QPen pen2 = pen.zoomedPen( _zoomHandler );
    int pw = ( pen2.style() == Qt::NoPen ) ? 1 : pen2.width();
    _painter->setPen( pen2 );

    if ( drawingShadow || getFillType() == FT_BRUSH || !gradient )
        _painter->setBrush( getBrush() );
    else {
        if ( m_redrawGradientPix || gradient->size() != size ) {
            m_redrawGradientPix = false;
            gradient->setSize( size );
            QRegion clipregion( 0, 0, ow - pw + 1, oh - pw + 1, QRegion::Ellipse );
            m_gradientPix.resize ( ow, oh );
            m_gradientPix.fill( Qt::white );
            QPainter p;
            p.begin( &m_gradientPix );
            p.setClipRegion( clipregion );
            p.drawPixmap( 0, 0, gradient->pixmap() );
            p.end();

            m_gradientPix.setMask( m_gradientPix.createHeuristicMask() );
        }

        _painter->drawPixmap( pw / 2, pw / 2, m_gradientPix, 0, 0, ow - pw + 1, oh - pw + 1 );

        _painter->setBrush( Qt::NoBrush );
    }
    _painter->drawEllipse( pw / 2, pw / 2, ow - pw + 1, oh - pw + 1 );
}

KoSize KPrEllipseObject::getRealSize() const {
    KoSize size = ext;

    if ( angle != 0.0 ) {
      float angInRad = angle * M_PI / 180;
      size.setWidth( sqrt( pow ( ext.width() * cos( angInRad ), 2) +
                           pow ( ext.height() * sin( angInRad ) ,2 ) ) );
      size.setHeight( sqrt( pow ( ext.width() * sin( angInRad ), 2) +
                            pow ( ext.height() * cos( angInRad ) ,2 ) ) );
    }

    return size;
}

bool KPrEllipseObject::saveOasisObjectAttributes( KPOasisSaveContext &/*sc*/ ) const
{
    // nothing to do
    return true;
}

const char * KPrEllipseObject::getOasisElementName() const
{
    return ext.width() == ext.height() ? "draw:circle" : "draw:ellipse";
}

