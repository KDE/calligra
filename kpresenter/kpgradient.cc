/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998		  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Gradients						  */
/******************************************************************/

#include "kpgradient.h"

#include <qpainter.h>

#include <kpixmapeffect.h>

/******************************************************************/
/* Class: KPGradient						  */
/******************************************************************/

/*======================= constructor ============================*/
KPGradient::KPGradient( QColor _color1, QColor _color2, BCType _bcType, QSize _size, 
			bool _unbalanced, int _xfactor, int _yfactor )
    : color1( _color1 ), color2( _color2 ), pixmap(), refCount( 0 ), unbalanced( _unbalanced ),
      xFactor( _xfactor ), yFactor( _yfactor )
{
    bcType = _bcType;
    pixmap.resize( _size );
    paint();
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
void KPGradient::paint()
{
    QPainter painter;
    
    switch ( bcType )
    {
    case BCT_PLAIN: 
	painter.begin( &pixmap );

	painter.setPen( Qt::NoPen );
	painter.setBrush( color1 );
	painter.drawRect( QRect( 0, 0, pixmap.size().width(), pixmap.size().height() ) );

	painter.end();
	break;
    case BCT_GHORZ: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::VerticalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::VerticalGradient, xFactor, yFactor ); 
    } break;
    case BCT_GVERT: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::HorizontalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::VerticalGradient, xFactor, yFactor ); 
    } break;
    case BCT_GDIAGONAL1: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::DiagonalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::DiagonalGradient, xFactor, yFactor ); 
    } break;
    case BCT_GDIAGONAL2: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::CrossDiagonalGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::CrossDiagonalGradient,  xFactor, yFactor ); 
    } break;
    case BCT_GCIRCLE: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::EllipticGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::EllipticGradient, xFactor, yFactor ); 
    } break;
    case BCT_GRECT: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::RectangleGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::RectangleGradient, xFactor, yFactor ); 
    } break;
    case BCT_GPIPECROSS: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::PipeCrossGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::PipeCrossGradient, xFactor, yFactor ); 
    } break;
    case BCT_GPYRAMID: {
	if ( !unbalanced )
	    KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::PyramidGradient );
	else
	    KPixmapEffect::unbalancedGradient( pixmap, color1, color2, KPixmapEffect::PyramidGradient, xFactor, yFactor ); 
    } break;
    }
}

