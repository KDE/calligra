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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kpgradient.h>
#include <qpainter.h>
#include <kpixmapeffect.h>
#include <kdebug.h>
#include <kozoomhandler.h>
#include <koRect.h>
#include <koSize.h>
#include <koPoint.h>

/******************************************************************/
/* Class: KPGradient						  */
/******************************************************************/

/*======================= constructor ============================*/
KPGradient::KPGradient( const QColor &_color1, const QColor &_color2, BCType _bcType,
                        const KoSize &_size, bool _unbalanced, int _xfactor, int _yfactor )
    : color1( _color1 ), color2( _color2 ), pixmap(), refCount( 0 ), unbalanced( _unbalanced ),
      xFactor( _xfactor ), yFactor( _yfactor )
{
    bcType = _bcType;
    gradientSize=_size;

    pixmap.resize( _size.toQSize() );
}


void KPGradient::init(const QColor &c1, const QColor &c2, BCType _type,
                      bool _unbalanced, int xf, int yf) {
    color1=c1;
    color2=c2;
    bcType=_type;
    unbalanced=_unbalanced;
    xFactor=xf;
    yFactor=yf;
}

/*====================== add reference ==========================*/
void KPGradient::addRef()
{
    ++refCount;
}

/*====================== remove reference =======================*/
bool KPGradient::removeRef()
{
    return ( --refCount == 0 );
}

/*====================== paint ===================================*/
void KPGradient::paint(QPainter */*_painter*/, KoZoomHandler*_zoomHandler)
{
    QPainter painter;
    pixmap.resize ( _zoomHandler->zoomItX(gradientSize.width()),_zoomHandler->zoomItY(gradientSize.height()) );
    switch ( bcType ) {
    case BCT_PLAIN:
	painter.begin( &pixmap );

	painter.setPen( Qt::NoPen );
	painter.setBrush( color1 );
	painter.drawRect( QRect( 0, 0, _zoomHandler->zoomItX(gradientSize.width()), _zoomHandler->zoomItY(gradientSize.height() ) ));

	painter.end();
	break;
    case BCT_GHORZ: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::VerticalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::VerticalGradient,
					       xFactor, yFactor );
    } break;
    case BCT_GVERT: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::HorizontalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::HorizontalGradient,
					       xFactor, yFactor );
    } break;
    case BCT_GDIAGONAL1: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::DiagonalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::DiagonalGradient,
					       xFactor, yFactor );
    } break;
    case BCT_GDIAGONAL2: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::CrossDiagonalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2,
					       KPixmapEffect::CrossDiagonalGradient,  xFactor, yFactor );
    } break;
    case BCT_GCIRCLE: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::EllipticGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2,
					       KPixmapEffect::EllipticGradient, xFactor, yFactor );
    } break;
    case BCT_GRECT: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::RectangleGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2,
					       KPixmapEffect::RectangleGradient, xFactor, yFactor );
    } break;
    case BCT_GPIPECROSS: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::PipeCrossGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2,
					       KPixmapEffect::PipeCrossGradient, xFactor, yFactor );
    } break;
    case BCT_GPYRAMID: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::PyramidGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2,
					       KPixmapEffect::PyramidGradient, xFactor, yFactor );
    } break;
    }
}

void KPGradient::setSize( const KoSize &_size )
{
        gradientSize=_size;
}
