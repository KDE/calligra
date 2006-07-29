// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "KPrGradient.h"
#include <qpainter.h>
#include <kpixmapeffect.h>
#include <kdebug.h>
#include <KoTextZoomHandler.h>

KPrGradient::KPrGradient( const QColor &_color1, const QColor &_color2, BCType _bcType,
                        bool _unbalanced, int _xfactor, int _yfactor )
    : color1( _color1 ), color2( _color2 ), bcType( _bcType ),
      m_pixmap(), refCount( 0 ),
      xFactor( _xfactor ), yFactor( _yfactor ),
      unbalanced( _unbalanced ), m_bDirty( true )
{
    //m_pixmap.resize( _size );
}

void KPrGradient::setParameters(const QColor &c1, const QColor &c2, BCType _type,
                               bool _unbalanced, int xf, int yf) {
    color1=c1;
    color2=c2;
    bcType=_type;
    unbalanced=_unbalanced;
    xFactor=xf;
    yFactor=yf;
    m_bDirty = true;
}

void KPrGradient::addRef()
{
    ++refCount;
}

bool KPrGradient::removeRef()
{
    return ( --refCount == 0 );
}

void KPrGradient::paint()
{
    QPainter painter;
    switch ( bcType ) {
    case BCT_PLAIN:
        painter.begin( &m_pixmap );

        painter.setPen( Qt::NoPen );
        painter.setBrush( color1 );
        painter.drawRect( m_pixmap.rect() );

        painter.end();
        break;
    case BCT_GHORZ: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::VerticalGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2, KPixmapEffect::VerticalGradient,
                                               xFactor, yFactor );
    } break;
    case BCT_GVERT: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::HorizontalGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2, KPixmapEffect::HorizontalGradient,
                                               xFactor, yFactor );
    } break;
    case BCT_GDIAGONAL1: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::DiagonalGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2, KPixmapEffect::DiagonalGradient,
                                               xFactor, yFactor );
    } break;
    case BCT_GDIAGONAL2: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::CrossDiagonalGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2,
                                               KPixmapEffect::CrossDiagonalGradient,  xFactor, yFactor );
    } break;
    case BCT_GCIRCLE: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::EllipticGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2,
                                               KPixmapEffect::EllipticGradient, xFactor, yFactor );
    } break;
    case BCT_GRECT: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::RectangleGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2,
                                               KPixmapEffect::RectangleGradient, xFactor, yFactor );
    } break;
    case BCT_GPIPECROSS: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::PipeCrossGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2,
                                               KPixmapEffect::PipeCrossGradient, xFactor, yFactor );
    } break;
    case BCT_GPYRAMID: {
        if ( !unbalanced )
            KPixmapEffect::gradient( m_pixmap, color1, color2, KPixmapEffect::PyramidGradient );
        else
            KPixmapEffect::unbalancedGradient( m_pixmap, color1, color2,
                                               KPixmapEffect::PyramidGradient, xFactor, yFactor );
    } break;
    }
    m_bDirty = false;
}

const QPixmap& KPrGradient::pixmap() const
{
    if ( m_bDirty )
        const_cast<KPrGradient *>(this)->paint();
    return m_pixmap;
}
