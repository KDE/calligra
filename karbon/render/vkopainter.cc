/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

// kopainter/libart wrapper

#include "vkopainter.h"
#include "vstroke.h"
#include "vcolor.h"

#include <qwidget.h>
#include <qpixmap.h>
#include <qpointarray.h>

#include <art_vpath_bpath.h>
#include <art_svp_vpath_stroke.h>
#include <art_svp.h>
#include <art_affine.h>
#include <art_rgb_svp.h>

#include <X11/Xlib.h>

#include <gdk-pixbuf-xlibrgb.h>

#include <kdebug.h>
#include <math.h>

VKoPainter::VKoPainter( QWidget *target, int w, int h ) : VPainter( target, w, h ), m_target( target )
{
	kdDebug() << "w : " << w << endl;
	kdDebug() << "h : " << h << endl;
	m_width = ( w > 0 ) ? w : target->width();
	m_height= ( h > 0 ) ? h : target->height();
	m_buffer = 0L;
	resize( m_width, m_height );
	clear();

	m_stroke = 0L;

	xlib_rgb_init_with_depth( target->x11Display(), XScreenOfDisplay( target->x11Display(),
							  target->x11Screen() ), target->x11Depth() );
	gc = XCreateGC( target->x11Display(), target->handle(), 0, 0 );

}

VKoPainter::~VKoPainter()
{
	if( m_buffer )
		art_free( m_buffer );

	delete m_stroke;

	if( gc )
		XFreeGC( m_target->x11Display(), gc );
}

void
VKoPainter::resize( int w, int h )
{
	if( !m_buffer || w != m_width || h != m_height )
	{
		// TODO : realloc?
		delete m_buffer;
		m_width = w;
		m_height = h;
		m_buffer = art_new( art_u8, m_width * m_height * 3 );
		clear();
	}
}

void
VKoPainter::begin()
{
	clear();
}

void
VKoPainter::end()
{
	QPixmap pix( m_width, m_height );
	//xlib_draw_rgb_image( m_target->handle(), gc, 0, 0, m_width, m_height,
	//					 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 3 );
	xlib_draw_rgb_image( pix.handle(), gc, 0, 0, m_width, m_height,
						 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 3 );
	bitBlt( m_target, 0, 0, &pix, 0, 0, m_width, m_height );
}

void
VKoPainter::setWorldMatrix( const QWMatrix &mat )
{
	m_matrix = mat;
}

void
VKoPainter::drawPolygon( const QPointArray &pa, bool winding )
{
	ArtVpath *polygon;
	polygon = art_new( ArtVpath, pa.count() + 2 );

	// start
	polygon[ 0 ].code	= ART_MOVETO;
	polygon[ 0 ].x		= pa.point( 0 ).x();
	polygon[ 0 ].y		= pa.point( 0 ).y();

	// all segments
	int index;
	for( index = 1; index < pa.count() ; index++ )
	{
		QPoint point = pa.point( index );
		polygon[ index ].code	= ART_LINETO;
		polygon[ index ].x		= point.x();
		polygon[ index ].y		= point.y();
	}

	// close
    polygon[ index ].code	= ART_LINETO;
    polygon[ index ].x		= pa.point( 0 ).x();
    polygon[ index ].y		= pa.point( 0 ).y();

	polygon[ ++index ].code = ART_END;

	drawVPath( polygon );

	art_free( polygon );
}

void
VKoPainter::drawPolyline( const QPointArray &pa )
{
	// TODO : look at closing this thing
	ArtVpath *polyline;
	polyline = art_new( ArtVpath, pa.count() + 1 );

	// start
	polyline[ 0 ].code	= ART_MOVETO;
	polyline[ 0 ].x		= pa.point( 0 ).x();
	polyline[ 0 ].y		= pa.point( 0 ).y();

	// all segments
	int index;
	for( index = 1; index < pa.count() ; index++ )
	{
		QPoint point = pa.point( index );
		polyline[ index ].code	= ART_LINETO;
		polyline[ index ].x		= point.x();
		polyline[ index ].y		= point.y();
	}

	// close
	polyline[ index ].code = ART_END;

	drawVPath( polyline );

	art_free( polyline );
}

void
VKoPainter::drawRect( double x, double y, double w, double h )
{
	ArtVpath vpath[] = {
	{ ART_MOVETO, x, y },
	{ ART_LINETO, x, y + h },
	{ ART_LINETO, x + w, y + h },
	{ ART_LINETO, x + w, y  },
	{ ART_LINETO, x, y },
	{ ART_END, 0, 0 }
	};

	drawVPath( vpath );
}

void
VKoPainter::setPen( const VStroke &stroke )
{
	delete m_stroke;
	m_stroke =  new VStroke;
	*m_stroke = stroke;
}

// void setBrush( const VBrush & );

void
VKoPainter::setPen( const QColor &c )
{
	delete m_stroke;
	m_stroke = new VStroke;
	VColor color;
	double r = double( c.red()   ) / 255.0;
	double g = double( c.green() ) / 255.0;
	double b = double( c.blue()  ) / 255.0;
	color.setValues( &r, &g, &b );
	m_stroke->setColor( color );
}

void
VKoPainter::setPen( Qt::PenStyle style )
{
}

void
VKoPainter::setBrush( const QBrush &b )
{
}

void
VKoPainter::setBrush( const QColor &c )
{
}

void
VKoPainter::setBrush( Qt::BrushStyle style )
{
}

void
VKoPainter::save()
{
}

void
VKoPainter::restore()
{
}

void
VKoPainter::setRasterOp( Qt::RasterOp r )
{
}

void
VKoPainter::clear()
{
	memset( m_buffer, qRgba( 255, 255, 255, 255 ), m_width * m_height * 3 );
}

void
VKoPainter::clear( unsigned int color )
{
	memset( m_buffer, qRgba( qRed( color ), qGreen( color ), qBlue( color ), 255 ),
			m_width * m_height * 3 );
}

void
VKoPainter::drawVPath( ArtVpath *vec )
{
	QColor color;
	ArtSVP *svp = 0L;

	// set up world matrix
	double affine[6];
	affine[0] = m_matrix.m11();
	affine[1] = m_matrix.m12();
	affine[2] = m_matrix.m21();
	affine[3] = m_matrix.m22();
	affine[4] = m_matrix.dx();
	affine[5] = m_matrix.dy();
	/*ArtVpath *temp = art_vpath_affine_transform( vec, affine );
	vec = temp;*/

    // TODO : filling
	/*{
		fillColor = (qRed(m_fillColor.rgb()) << 24) | (qGreen(m_fillColor.rgb()) << 16) | (qBlue(m_fillColo
					r.rgb()) << 8) | (qAlpha(m_fillColor.rgb()));

		ArtSvpWriter *swr;
		ArtSVP *temp;
		temp = art_svp_from_vpath(vec);

		if(m_drawShape->getFillRule() == "evenodd")
			swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);
		else
			swr = art_svp_writer_rewind_new(ART_WIND_RULE_NONZERO);

		art_svp_intersector(temp, swr);
		svp = art_svp_writer_rewind_reap(swr);
		m_fillSVP = svp;

		art_svp_free(temp);
	}*/

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;
	// stroke
	if( m_stroke )
	{
		ArtPathStrokeCapType capStyle;
		ArtPathStrokeJoinType joinStyle;
		// TODO : non rgb support ?

		m_stroke->color().pseudoValues( r, g, b );
		a = m_stroke->opacity();

		// caps translation karbon -> art
		if( m_stroke->lineCap() == VStroke::cap_butt )
			capStyle = ART_PATH_STROKE_CAP_BUTT;
		else if( m_stroke->lineCap() == VStroke::cap_round )
			capStyle = ART_PATH_STROKE_CAP_ROUND;
		else if( m_stroke->lineCap() == VStroke::cap_square )
			capStyle = ART_PATH_STROKE_CAP_SQUARE;

		// zoom stroke width;
		double ratio = sqrt(pow(affine[0], 2) + pow(affine[3], 2)) / sqrt(2);
		svp = art_svp_vpath_stroke( vec, ART_PATH_STROKE_JOIN_ROUND, capStyle, ratio * m_stroke->lineWidth(), 5.0, 0.25 );
	}

	// render the svp to the buffer
	if( svp )
	{
		art_u32 strokeColor = ( r << 24 ) | ( g << 16 ) | ( b << 8 ) | a;
		//art_rgb_svp_alpha( svp, 0, 0, m_width, m_height, color.rgb(), alpha, m_buffer, m_width * 3, 0 );
		art_rgb_svp_alpha( svp, 0, 0, m_width, m_height, strokeColor, m_buffer, m_width * 3, 0 );
	}

	m_stroke = 0L;
}

