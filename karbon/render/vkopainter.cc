/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

// kopainter/libart wrapper

#include "vkopainter.h"
#include "vstroke.h"
#include "vfill.h"
#include "vcolor.h"

#include <qpaintdevice.h>
#include <qpixmap.h>
#include <qpointarray.h>
#include <qimage.h>

#include <art_vpath.h>
#include <art_bpath.h>
#include <art_vpath_bpath.h>
#include <art_svp_vpath.h>
#include <art_svp_vpath_stroke.h>
#include <art_svp.h>
#include <art_svp_ops.h>
#include <art_affine.h>
#include <art_rgb_svp.h>
#include <art_rect_svp.h>
#include <art_svp_intersect.h>
#include <art_pathcode.h>
#include <art_vpath_dash.h>
#include <art_rgb_affine.h>

#include <X11/Xlib.h>

#include <gdk-pixbuf-xlibrgb.h>

#include <kdebug.h>
#include <math.h>

#include <koPoint.h>

VKoPainter::VKoPainter( QPaintDevice *target, unsigned int w, unsigned int h ) : VPainter( target, w, h ), m_target( target )
{
	kdDebug() << "w : " << w << endl;
	kdDebug() << "h : " << h << endl;
	m_width = w;//( w > 0 ) ? w : target->width();
	m_height= h;//( h > 0 ) ? h : target->height();
	m_buffer = 0L;
	m_path = 0L;
	m_index = 0;
	resize( m_width, m_height );
	clear();

	m_stroke = 0L;
	m_fill = 0L;

	xlib_rgb_init_with_depth( target->x11Display(), XScreenOfDisplay( target->x11Display(),
							  target->x11Screen() ), target->x11Depth() );

	gc = XCreateGC( target->x11Display(), target->handle(), 0, 0 );

	m_zoomFactor = 1;
}

VKoPainter::~VKoPainter()
{
	art_free( m_buffer );

	delete m_stroke;
	delete m_fill;
	if( m_path )
		art_free( m_path );

	if( gc )
		XFreeGC( m_target->x11Display(), gc );
}

void
VKoPainter::resize( unsigned int w, unsigned int h )
{
	if( !m_buffer || w != m_width || h != m_height )
	{
		// TODO : realloc?
		art_free( m_buffer );
		m_buffer = 0;
		m_width = w;
		m_height = h;
		if ( m_width != 0 && m_height != 0 )
			m_buffer = art_new( art_u8, m_width * m_height * 4 );
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
	//					 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 4 );
	xlib_draw_rgb_32_image( pix.handle(), gc, 0, 0, m_width, m_height,
						 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 4 );
	bitBlt( m_target, 0, 0, &pix, 0, 0, m_width, m_height );
	/*xlib_draw_rgb_image( pix.handle(), gc, 0, 0, m_width, m_height,
						 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 3 );
	bitBlt( m_target, 0, 0, &pix, 0, 0, m_width, m_height );*/
}

void
VKoPainter::blit( const QRect &r )
{
	QPixmap pix( m_width, m_height );
	xlib_draw_rgb_32_image( pix.handle(), gc, r.x(), r.y(), r.width(), r.height(),
							XLIB_RGB_DITHER_NONE, m_buffer + (r.x() * 4) + (r.y() *  m_width * 4), m_width * 4 );
	bitBlt( m_target, r.x(), r.y(), &pix, r.x(), r.y(), r.width(), r.height() );
}

void
VKoPainter::setWorldMatrix( const QWMatrix &mat )
{
	m_matrix = mat;
}

void
VKoPainter::setZoomFactor( double zoomFactor )
{
	m_zoomFactor = zoomFactor;
}

void 
VKoPainter::moveTo( const KoPoint &p )
{
	if( m_index == 0)
	{
		if( !m_path )
			m_path = art_new( ArtBpath, 500 );

		m_path[ m_index ].code = ART_MOVETO;
	}
	else
		m_path[ m_index ].code = ART_MOVETO_OPEN;

	m_path[ m_index ].x3	= p.x() * m_zoomFactor;
	m_path[ m_index ].y3	= p.y() * m_zoomFactor;

	m_index++;
}

void 
VKoPainter::lineTo( const KoPoint &p )
{
	m_path[ m_index ].code = ART_LINETO;
	m_path[ m_index ].x3	= p.x() * m_zoomFactor;
	m_path[ m_index ].y3	= p.y() * m_zoomFactor;

	m_index++;
}

void
VKoPainter::curveTo( const KoPoint &p1, const KoPoint &p2, const KoPoint &p3 )
{
	m_path[ m_index ].code = ART_CURVETO;
	m_path[ m_index ].x1	= p1.x() * m_zoomFactor;
	m_path[ m_index ].y1	= p1.y() * m_zoomFactor;
	m_path[ m_index ].x2	= p2.x() * m_zoomFactor;
	m_path[ m_index ].y2	= p2.y() * m_zoomFactor;
	m_path[ m_index ].x3	= p3.x() * m_zoomFactor;
	m_path[ m_index ].y3	= p3.y() * m_zoomFactor;

	m_index++;
}

void
VKoPainter::newPath()
{
	m_index = 0;
}

void
VKoPainter::fillPath()
{
	// find begin of last subpath
    int find = -1;
    for( int i = m_index - 1; i >= 0; i-- )
    {
        if( m_path[i].code == ART_MOVETO_OPEN || m_path[i].code == ART_MOVETO )
        {
            find = i;
            break;
        }
	}

	// for now, always close
    if( find != -1 && ( m_path[ find ].x3 != m_path[ m_index - 1 ].x3 ||
						m_path[ find ].y3 != m_path[ m_index - 1 ].y3 ) )
	{
		m_path[ m_index ].code = ART_LINETO;
		m_path[ m_index ].x3	= m_path[ find ].x3;
		m_path[ m_index ].y3	= m_path[ find ].y3;

		m_index++;
		m_path[ m_index ].code = ART_END;
	}
	else
		m_path[ m_index++ ].code = ART_END;


	ArtVpath *path;
	path = art_bez_path_to_vec( m_path , 0.25 );

	drawVPath( path );

	m_index--;
	//art_free( path );
}

void
VKoPainter::strokePath()
{
	if( m_path[ m_index ].code != ART_END)
		m_path[ m_index ].code = ART_END;

	ArtVpath *path;
	path = art_bez_path_to_vec( m_path , 0.25 );

	//m_index = 0;

	drawVPath( path );
	//art_free( path );
}

void
VKoPainter::setPen( const VStroke &stroke )
{
	delete m_stroke;
	m_stroke =  new VStroke;
	*m_stroke = stroke;
}

void
VKoPainter::setPen( const QColor &c )
{
	delete m_stroke;
	m_stroke = new VStroke;
	VColor color;
	float r = static_cast<float>( c.red()   ) / 255.0;
	float g = static_cast<float>( c.green() ) / 255.0;
	float b = static_cast<float>( c.blue()  ) / 255.0;
	color.setValues( &r, &g, &b );
	m_stroke->setColor( color );
}

void
VKoPainter::setPen( Qt::PenStyle style )
{
	if( style == Qt::NoPen )
	{
		delete m_stroke;
		m_stroke = 0L;
	}
}

void
VKoPainter::setBrush( const QColor &c )
{
	delete m_fill;
	m_fill = new VFill;
	VColor color;
	float r = static_cast<float>( c.red()   ) / 255.0;
	float g = static_cast<float>( c.green() ) / 255.0;
	float b = static_cast<float>( c.blue()  ) / 255.0;
	color.setValues( &r, &g, &b );
	m_fill->setColor( color );
}

void
VKoPainter::setBrush( Qt::BrushStyle style )
{
	if( style == Qt::NoBrush )
	{
		delete m_fill;
		m_fill = 0L;
	}
}

void
VKoPainter::setBrush( const VFill &fill )
{
	delete m_fill;
	m_fill =  new VFill;
	*m_fill = fill;
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
	if ( m_buffer )
		memset( m_buffer, qRgba( 255, 255, 255, 255 ), m_width * m_height * 4 );
}

void
VKoPainter::clear( unsigned int color )
{
	if ( m_buffer )
		memset( m_buffer, qRgba( qRed( color ), qGreen( color ), qBlue( color ), 255 ),
			m_width * m_height * 4 );
}

void
VKoPainter::drawVPath( ArtVpath *vec )
{
	QColor color;
	ArtSVP *strokeSvp = 0L;
	ArtSVP *fillSvp = 0L;

	// set up world matrix
	double affine[6];
	affine[0] = 1;//m_zoomFactor;//m_matrix.m11();
	affine[1] = 0;//m_matrix.m12();
	affine[2] = 0;//m_matrix.m21();
	affine[3] = 1;//m_zoomFactor;//m_matrix.m22();
	affine[4] = m_matrix.dx();
	affine[5] = m_matrix.dy();
	ArtVpath *temp = art_vpath_affine_transform( vec, affine );
	vec = temp;

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;
	art_u32 fillColor;
    // filling
	if( m_fill && m_fill->type() == fill_fill )
	{
		m_fill->color().pseudoValues( r, g, b );
		a = qRound( 255 * m_fill->color().opacity() );
		fillColor = ( 0 << 24 ) | ( b << 16 ) | ( g << 8 ) | r;

		ArtSvpWriter *swr;
		ArtSVP *temp;
		temp = art_svp_from_vpath( vec );

		if( m_fill->fillRule() == fillrule_evenOdd )
			swr = art_svp_writer_rewind_new( ART_WIND_RULE_ODDEVEN );
		else
			swr = art_svp_writer_rewind_new( ART_WIND_RULE_NONZERO );

		art_svp_intersector( temp, swr );
		fillSvp = art_svp_writer_rewind_reap( swr );

		art_svp_free( temp );
	}

	art_u32 strokeColor;
	// stroke
	if( m_stroke )
	{
		ArtPathStrokeCapType capStyle;
		ArtPathStrokeJoinType joinStyle;
		// TODO : non rgb support ?

		m_stroke->color().pseudoValues( r, g, b );
		a = qRound( 255 * m_stroke->color().opacity() );
		strokeColor = ( 0 << 24 ) | ( b << 16 ) | ( g << 8 ) | r;

		double ratio = m_zoomFactor;//sqrt(pow(affine[0], 2) + pow(affine[3], 2)) / sqrt(2);
		if( m_stroke->dashPattern().array().count() > 0 )
		{
			// there are dashes to be rendered
			ArtVpathDash dash;
			dash.offset = m_stroke->dashPattern().offset() * ratio;
			dash.n_dash = m_stroke->dashPattern().array().count();
			double *dashes = new double[ dash.n_dash ];
			for( int i = 0; i < dash.n_dash; i++ )
				dashes[i] = m_stroke->dashPattern().array()[i] * ratio;

			dash.dash = dashes;
			// get the dashed VPath and use that for the stroke render operation
			ArtVpath *vec2 = art_vpath_dash( vec, &dash );
			art_free( vec );

			vec = vec2;
			delete [] dashes;
		}
		// caps translation karbon -> art
		if( m_stroke->lineCap() == cap_butt )
			capStyle = ART_PATH_STROKE_CAP_BUTT;
		else if( m_stroke->lineCap() == cap_round )
			capStyle = ART_PATH_STROKE_CAP_ROUND;
		else if( m_stroke->lineCap() == cap_square )
			capStyle = ART_PATH_STROKE_CAP_SQUARE;

		// join translation karbon -> art
		if( m_stroke->lineJoin() == join_miter )
			joinStyle = ART_PATH_STROKE_JOIN_MITER;
		else if( m_stroke->lineJoin() == join_round )
			joinStyle = ART_PATH_STROKE_JOIN_ROUND;
		else if( m_stroke->lineJoin() == join_bevel )
			joinStyle = ART_PATH_STROKE_JOIN_BEVEL;

		// zoom stroke width;
		strokeSvp = art_svp_vpath_stroke( vec, ART_PATH_STROKE_JOIN_ROUND/*joinStyle*/, capStyle, ratio * m_stroke->lineWidth(), 5.0, 0.25 );
	}

	// render the svp to the buffer
	if( strokeSvp )
	{
		// get SVP bbox
		ArtDRect bbox;
		art_drect_svp( &bbox, strokeSvp );

		// clamp to viewport
		int x0 = int( bbox.x0 );
		x0 = QMAX( x0, 0 );
		x0 = QMIN( x0, m_width );
		int y0 = int( bbox.y0 );
		y0 = QMAX( y0, 0 );
		y0 = QMIN( y0, m_height );
		int x1 = int( bbox.x1 ) + 1;
		x1 = QMAX( x1, 0 );
		x1 = QMIN( x1, m_width );
		int y1 = int( bbox.y1 ) + 1;
		y1 = QMAX( y1, 0 );
		y1 = QMIN( y1, m_height );

		art_rgb_svp_alpha( strokeSvp, x0, y0, x1, y1, strokeColor, a, m_buffer + x0 * 4 + y0 * m_width * 4, m_width * 4, 0 );
		art_svp_free( strokeSvp );
	}

	if( fillSvp )
	{
		// get SVP bbox
		ArtDRect bbox;
		art_drect_svp( &bbox, fillSvp );

		// clamp to viewport
		int x0 = int( bbox.x0 );
		x0 = QMAX( x0, 0 );
		x0 = QMIN( x0, m_width );
		int y0 = int( bbox.y0 );
		y0 = QMAX( y0, 0 );
		y0 = QMIN( y0, m_height );
		int x1 = int( bbox.x1 ) + 1;
		x1 = QMAX( x1, 0 );
		x1 = QMIN( x1, m_width );
		int y1 = int( bbox.y1 ) + 1;
		y1 = QMAX( y1, 0 );
		y1 = QMIN( y1, m_height );

		art_rgb_svp_alpha( fillSvp, x0, y0, x1, y1, fillColor, a, m_buffer + x0 * 4 + y0 * m_width * 4, m_width * 4, 0 );
		art_svp_free( fillSvp );
	}

	delete m_stroke;
	m_stroke = 0L;
	delete m_fill;
	m_fill = 0L;

	art_free( vec );
}

void
VKoPainter::drawImage( const QImage &image )
{
	// set up world matrix
	double affine[6];
	affine[0] = m_zoomFactor;//m_matrix.m11();
	affine[1] = m_matrix.m12();
	affine[2] = m_matrix.m21();
	affine[3] = m_zoomFactor;//m_matrix.m22();
	affine[4] = m_matrix.dx();
	affine[5] = m_matrix.dy();
	art_rgb_affine(	m_buffer, 0, 0, m_width, m_height, m_width * 4,
					image.bits(), image.width(), image.height(), image.width() * 4,
					affine, ART_FILTER_NEAREST, 0L );
}

