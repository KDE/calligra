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
KPGradient::KPGradient( QColor _color1, QColor _color2, BCType _bcType, QSize _size )
    : color1( _color1 ), color2( _color2 ), pixmap(), refCount( 0 )
{
    bcType = _bcType;
    pixmap.resize( _size );
    paint();
}

/*====================== add reference ==========================*/
void KPGradient::addRef()
{
#ifdef SHOW_INFO
    debug( "Refs of '( %d, %d, %d ) ( %d, %d, %d )': %d", color1.red(), color1.green(), color1.blue(),
	   color2.red(), color2.green(), color2.blue(), ++refCount );
#else
    ++refCount;
#endif
}

/*====================== remove reference =======================*/
bool KPGradient::removeRef()
{
#ifdef SHOW_INFO
    debug( "Refs of '( %d, %d, %d ) ( %d, %d, %d )': %d", color1.red(), color1.green(), color1.blue(),
	   color2.red(), color2.green(), color2.blue(), --refCount );
    return refCount == 0;
#else
    return ( --refCount == 0 );
#endif
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
    case BCT_GHORZ:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::VerticalGradient );
	break;
    case BCT_GVERT:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::HorizontalGradient );
	break;
    case BCT_GDIAGONAL1:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::DiagonalGradient );
	break;
    case BCT_GDIAGONAL2:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::CrossDiagonalGradient );
	break;
    case BCT_GCIRCLE:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::EllipticGradient );
	break;
    case BCT_GRECT:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::RectangleGradient );
	break;
    case BCT_GPIPECROSS:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::PipeCrossGradient );
	break;
    case BCT_GPYRAMID:
	KPixmapEffect::gradient( pixmap, color1, color2, KPixmapEffect::PyramidGradient );
	break;
    }
}

