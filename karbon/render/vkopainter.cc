/* This file is part of the KDE project.
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

// kopainter/libart wrapper

#include "vkopainter.h"
#include "vstroke.h"
#include "vfill.h"
#include "vcolor.h"
#include "vpattern.h"

#include <qpaintdevice.h>
#include <qpixmap.h>
#include <qpointarray.h>
#include <qimage.h>

#include "libart_lgpl/art_vpath.h"
#include <libart_lgpl/art_bpath.h>
#include <libart_lgpl/art_vpath_bpath.h>
#include <libart_lgpl/art_svp_vpath.h>
#include <libart_lgpl/art_svp_vpath_stroke.h>
#include <libart_lgpl/art_svp.h>
#include <libart_lgpl/art_svp_ops.h>
#include <libart_lgpl/art_affine.h>
#include <libart_lgpl/art_svp_intersect.h>
#include <libart_lgpl/art_rect_svp.h>
#include <libart_lgpl/art_pathcode.h>
#include <libart_lgpl/art_vpath_dash.h>
#include <libart_lgpl/art_rgb_affine.h>
#include "art_render_misc.h"
#include <libart_lgpl/art_render_svp.h>

#include "art_rgb_svp.h"
#include "art_render_pattern.h"

#include <X11/Xlib.h>

#include <gdk-pixbuf-xlibrgb.h>

#include <kdebug.h>
#include <math.h>

#include <koPoint.h>
#include <koRect.h>

#define INITIAL_ALLOC	300
#define ALLOC_INCREMENT	100

VKoPainter::VKoPainter( QPaintDevice *target, unsigned int w, unsigned int h, bool bDrawNodes )
: VPainter( target, w, h ), m_target( target ), m_bDrawNodes( bDrawNodes )
{
	//kdDebug() << "w : " << w << endl;
	//kdDebug() << "h : " << h << endl;
	m_width = w;//( w > 0 ) ? w : target->width();
	m_height= h;//( h > 0 ) ? h : target->height();
	m_buffer = 0L;
	m_path = 0L;
	m_index = 0;
	resize( m_width, m_height );
	clear();
	m_clipPaths.setAutoDelete( false );

	m_stroke = 0L;
	m_fill = 0L;

	xlib_rgb_init_with_depth( target->x11Display(), XScreenOfDisplay( target->x11Display(),
							  target->x11Screen() ), target->x11Depth() );

	gc = XCreateGC( target->x11Display(), target->handle(), 0, 0 );

	m_zoomFactor = 1;
}

VKoPainter::VKoPainter( unsigned char *buffer, unsigned int w, unsigned int h, bool bDrawNodes )
: VPainter( 0L, w, h ), m_buffer( buffer ), m_bDrawNodes( bDrawNodes )
{
	//kdDebug() << "w : " << w << endl;
	//kdDebug() << "h : " << h << endl;
	m_target = 0L;
	m_width = w;
	m_height= h;
	m_path = 0L;
	m_index = 0;
	clear();
	m_clipPaths.setAutoDelete( false );

	m_stroke = 0L;
	m_fill = 0L;

	gc = 0L;

	m_zoomFactor = 1;
}

VKoPainter::~VKoPainter()
{
	// If we are in target mode, we created a buffer, else if we used the other ctor
	// we didnt.
	if( m_target )
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
}

void
VKoPainter::end()
{
	//xlib_draw_rgb_image( m_target->handle(), gc, 0, 0, m_width, m_height,
	//					 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 4 );
	xlib_draw_rgb_32_image( m_target->handle(), gc, 0, 0, m_width, m_height,
						 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 4 );
	/*xlib_draw_rgb_image( pix.handle(), gc, 0, 0, m_width, m_height,
						 XLIB_RGB_DITHER_NONE, m_buffer, m_width * 3 );
	bitBlt( m_target, 0, 0, &pix, 0, 0, m_width, m_height );*/
}

void
VKoPainter::blit( const KoRect &r )
{
	//kdDebug() << "m_width : " << m_width << endl;
	//kdDebug() << "m_height : " << m_height << endl;
	int x		= QMAX( 0, r.x() );
	int y		= QMAX( 0, r.y() );
	int width	= QMIN( int( m_width ),		r.x() + r.width() );
	int height	= QMIN( int( m_height ),	r.y() + r.height() );
	xlib_draw_rgb_32_image( m_target->handle(), gc, x, y, width - x, height - y,
							XLIB_RGB_DITHER_NONE, m_buffer + (x * 4) + (y * m_width * 4), m_width * 4 );
}

void
VKoPainter::clear()
{
	if ( m_buffer )
		memset( m_buffer, qRgba( 255, 255, 255, 255 ), m_width * m_height * 4 );
}

void
VKoPainter::clear( const QColor &c )
{
	unsigned int color = c.rgb();
	if ( m_buffer )
		memset( m_buffer, qRgba( qRed( color ), qGreen( color ), qBlue( color ), 100 ), m_width * m_height * 4 );
}

void
VKoPainter::clear( const KoRect &r, const QColor &c )
{
	unsigned int color = c.rgb();
	int x		= QMAX( 0, r.x() );
	int y		= QMAX( 0, r.y() );
	int width	= QMIN( int( m_width ),		r.x() + r.width() );
	int height	= QMIN( int( m_height ),	r.y() + r.height() );
	if ( m_buffer )
	{
		for( unsigned int i = y;i < height;i++)
			memset( m_buffer + int( x * 4) + int( i * ( m_width * 4 ) ),
					qRgba( qRed( color ), qGreen( color ), qBlue( color ), 100 ), int( width * 4 ) );
	}
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
VKoPainter::ensureSpace( unsigned int newindex )
{
	if( m_index == 0 )
	{
		if( !m_path )
			m_path = art_new( ArtBpath, INITIAL_ALLOC );
		m_alloccount = INITIAL_ALLOC;
	}
	else if( newindex > m_alloccount )
	{
		m_alloccount += ALLOC_INCREMENT;
		m_path = art_renew( m_path, ArtBpath, m_alloccount );
	}
}

void 
VKoPainter::moveTo( const KoPoint &p )
{
	ensureSpace( m_index + 1 );

	m_path[ m_index ].code = ART_MOVETO;

	m_path[ m_index ].x3 = p.x() * m_zoomFactor;
	m_path[ m_index ].y3 = p.y() * m_zoomFactor;

	m_index++;
}

void 
VKoPainter::lineTo( const KoPoint &p )
{
	ensureSpace( m_index + 1 );

	m_path[ m_index ].code = ART_LINETO;
	m_path[ m_index ].x3	= p.x() * m_zoomFactor;
	m_path[ m_index ].y3	= p.y() * m_zoomFactor;

	m_index++;
}

void
VKoPainter::curveTo( const KoPoint &p1, const KoPoint &p2, const KoPoint &p3 )
{
	ensureSpace( m_index + 1 );

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
	if( m_index == 0 ) return;

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
		ensureSpace( m_index + 1 );

		m_path[ m_index ].code = ART_LINETO;
		m_path[ m_index ].x3	= m_path[ find ].x3;
		m_path[ m_index ].y3	= m_path[ find ].y3;

		m_index++;
		m_path[ m_index ].code = ART_END;
	}
	else
		m_path[ m_index++ ].code = ART_END;

	if( m_fill && m_fill->type() != VFill::none )
	{
		ArtVpath *path;
		path = art_bez_path_to_vec( m_path , 0.25 );

		drawVPath( path );
	}

	m_index--;
	//art_free( path );
}

void
VKoPainter::strokePath()
{
	if( m_index == 0 ) return;

	if( m_stroke && m_stroke->lineWidth() == 0 )
		return;
	if( m_path[ m_index ].code != ART_END)
		m_path[ m_index ].code = ART_END;

	ArtVpath *path;
	path = art_bez_path_to_vec( m_path , 0.25 );

	//m_index = 0;

	drawVPath( path );
	//art_free( path );
}

void
VKoPainter::setClipPath()
{
	ArtVpath *path;
	path = art_bez_path_to_vec( m_path , 0.25 );
	m_clipPaths.append( art_svp_from_vpath( path ) );
	art_free( path );
}

void
VKoPainter::resetClipPath()
{
	art_svp_free( m_clipPaths.current() );
	m_clipPaths.remove();
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

	float r = static_cast<float>( c.red()   ) / 255.0;
	float g = static_cast<float>( c.green() ) / 255.0;
	float b = static_cast<float>( c.blue()  ) / 255.0;

	VColor color;
	color.set( r, g, b );
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

	float r = static_cast<float>( c.red()   ) / 255.0;
	float g = static_cast<float>( c.green() ) / 255.0;
	float b = static_cast<float>( c.blue()  ) / 255.0;

	VColor color;
	color.set( r, g, b );
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
VKoPainter::setRasterOp( Qt::RasterOp  )
{
}

KoRect
VKoPainter::boundingBox() const
{
	return m_bbox;
}

void
VKoPainter::clampToViewport( int &x0, int &y0, int &x1, int &y1 )
{
	// clamp to viewport
	x0 = QMAX( x0, 0 );
	x0 = QMIN( x0, int( m_width ) );
	y0 = QMAX( y0, 0 );
	y0 = QMIN( y0, int ( m_height ) );
	x1 = QMAX( x1, 0 );
	x1 = QMIN( x1, int( m_width ) );
	y1 = QMAX( y1, 0 );
	y1 = QMIN( y1, int( m_height ) );
}

void
VKoPainter::clampToViewport( const ArtSVP &svp, int &x0, int &y0, int &x1, int &y1 )
{
	// get SVP bbox
	ArtDRect bbox;
	art_drect_svp( &bbox, &svp );
	// Remove comments if we really decide for SVP bbox usage
	//m_bbox = KoRect( bbox.x0, bbox.y0, bbox.x1 - bbox.x0, bbox.y1 - bbox.y0 );

	// clamp to viewport
	x0 = int( bbox.x0 );
	x0 = QMAX( x0, 0 );
	x0 = QMIN( x0, int( m_width ) );
	y0 = int( bbox.y0 );
	y0 = QMAX( y0, 0 );
	y0 = QMIN( y0, int ( m_height ) );
	x1 = int( bbox.x1 ) + 1;
	x1 = QMAX( x1, 0 );
	x1 = QMIN( x1, int( m_width ) );
	y1 = int( bbox.y1 ) + 1;
	y1 = QMAX( y1, 0 );
	y1 = QMIN( y1, int( m_height ) );
}

void
VKoPainter::drawVPath( ArtVpath *vec )
{
	ArtSVP *strokeSvp = 0L;
	ArtSVP *fillSvp = 0L;

	// set up world matrix
	double affine[6];
	affine[0] = m_matrix.m11();
	affine[1] = 0;//m_matrix.m12();
	affine[2] = 0;//m_matrix.m21();
	affine[3] = m_matrix.m22();
	affine[4] = m_matrix.dx();
	affine[5] = m_matrix.dy();
	ArtVpath *temp = art_vpath_affine_transform( vec, affine );
	vec = temp;

	int af = 0;
	int as = 0;
	art_u32 fillColor = 0;
    // filling

	QColor color;

	if( m_fill && m_fill->type() != VFill::none )
	{
		color = m_fill->color();
		af = qRound( 255 * m_fill->color().opacity() );
		fillColor = ( 0 << 24 ) | ( color.blue() << 16 ) | ( color.green() << 8 ) | color.red();

		ArtSvpWriter *swr;
		ArtSVP *temp;
		temp = art_svp_from_vpath( vec );

		if( m_fill->fillRule() == VFill::evenOdd )
			swr = art_svp_writer_rewind_new( ART_WIND_RULE_ODDEVEN );
		else
			swr = art_svp_writer_rewind_new( ART_WIND_RULE_NONZERO );

		art_svp_intersector( temp, swr );
		fillSvp = art_svp_writer_rewind_reap( swr );

		art_svp_free( temp );
	}

	art_u32 strokeColor = 0;
	// stroke
	if( m_stroke && m_stroke->type() != VStroke::none )
	{
		ArtPathStrokeCapType capStyle = ART_PATH_STROKE_CAP_BUTT;
		ArtPathStrokeJoinType joinStyle = ART_PATH_STROKE_JOIN_MITER;
		// TODO : non rgb support ?

		color = m_stroke->color();
		as = qRound( 255 * m_stroke->color().opacity() );
		strokeColor = ( 0 << 24 ) | ( color.blue() << 16 ) | ( color.green() << 8 ) | color.red();

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
		if( m_stroke->lineCap() == VStroke::capRound )
			capStyle = ART_PATH_STROKE_CAP_ROUND;
		else if( m_stroke->lineCap() == VStroke::capSquare )
			capStyle = ART_PATH_STROKE_CAP_SQUARE;

		// join translation karbon -> art
		if( m_stroke->lineJoin() == VStroke::joinRound )
			joinStyle = ART_PATH_STROKE_JOIN_ROUND;
		else if( m_stroke->lineJoin() == VStroke::joinBevel )
			joinStyle = ART_PATH_STROKE_JOIN_BEVEL;

		// zoom stroke width;
		strokeSvp = art_svp_vpath_stroke( vec, joinStyle, capStyle, ratio * m_stroke->lineWidth(), 5.0, 0.25 );
	}

	int x0, y0, x1, y1;

	// render the svp to the buffer
	if( strokeSvp )
	{
		if( m_stroke && m_stroke->type() == VStroke::grad )
			applyGradient( strokeSvp, false );
		else if( m_stroke && m_stroke->type() == VStroke::patt )
			applyPattern( strokeSvp, false );
		else
		{
			clampToViewport( *strokeSvp, x0, y0, x1, y1 );
			if( x0 != x1 && y0 != y1 )
				art_rgb_svp_alpha_( strokeSvp, x0, y0, x1, y1, strokeColor, as, m_buffer + x0 * 4 + y0 * m_width * 4, m_width * 4, 0 );
		}
		art_svp_free( strokeSvp );
	}

	if( fillSvp )
	{
		if( m_fill && m_fill->type() == VFill::grad )
			applyGradient( fillSvp, true );
		else if( m_fill && m_fill->type() == VFill::patt )
			applyPattern( fillSvp, true );
		else
		{
			clampToViewport( *fillSvp, x0, y0, x1, y1 );
			if( x0 != x1 && y0 != y1 )
				art_rgb_svp_alpha_( fillSvp, x0, y0, x1, y1, fillColor, af, m_buffer + x0 * 4 + y0 * m_width * 4, m_width * 4, 0 );
		}
		art_svp_free( fillSvp );
	}

	//delete m_stroke;
	//m_stroke = 0L;
	//delete m_fill;
	//m_fill = 0L;

	art_free( vec );
}

void
VKoPainter::applyPattern( ArtSVP *svp, bool fill )
{
	int x0, y0, x1, y1;
	clampToViewport( *svp, x0, y0, x1, y1 );

	ArtRender *render = 0L;

	VPattern pat = fill ? m_fill->pattern() : m_stroke->pattern();

	ArtPattern *pattern = new ArtPattern;

	double dx = ( pat.vector().x() - pat.origin().x() ) * m_zoomFactor;
	double dy = ( pat.vector().y() - pat.origin().y() ) * m_zoomFactor;

	pattern->twidth = pat.tileWidth();
	pattern->theight = pat.tileHeight();
	pattern->buffer = pat.pixels();
	pattern->opacity = fill ? short( m_fill->color().opacity() * 255.0 ) : short( m_stroke->color().opacity() * 255.0 );
	pattern->angle = atan2( dy, dx );

	if( x0 != x1 && y0 != y1 )
	{
		render = art_render_new( x0, y0, x1, y1, m_buffer + 4 * int(x0) + m_width * 4 * int(y0), m_width * 4, 3, 8, ART_ALPHA_PREMUL, 0 );
		art_render_svp( render, svp );
		art_render_pattern( render, pattern, ART_FILTER_HYPER );
	}

	if( render )
		art_render_invoke( render );
}

void
VKoPainter::applyGradient( ArtSVP *svp, bool fill )
{
	int x0, y0, x1, y1;
	clampToViewport( *svp, x0, y0, x1, y1 );

	ArtRender *render = 0L;

	VGradient gradient = fill ? m_fill->gradient() : m_stroke->gradient();

	if( gradient.type() == VGradient::linear )
	{
		ArtGradientLinear *linear = new ArtGradientLinear();

		// TODO : make variable
		if( gradient.repeatMethod() == VGradient::none )
			linear->spread = ART_GRADIENT_PAD;
		else if( gradient.repeatMethod() == VGradient::repeat )
			linear->spread = ART_GRADIENT_REPEAT;
		else if( gradient.repeatMethod() == VGradient::reflect )
			linear->spread = ART_GRADIENT_REFLECT;

		double dx = ( gradient.vector().x() - gradient.origin().x() ) * m_zoomFactor;
		double _y1 = gradient.vector().y();
		_y1 = m_matrix.m22() * _y1 + m_matrix.dy() / m_zoomFactor;
		double _y2 = gradient.origin().y();
		_y2 = m_matrix.m22() * _y2 + m_matrix.dy() / m_zoomFactor;
		//kdDebug() << "_y1 : " << _y1 << ", _y2 " << _y2 << endl;
		double dy = ( _y1 - _y2 ) * m_zoomFactor;
		double scale = 1.0 / ( dx * dx + dy * dy );

		linear->a = dx * scale;
		linear->b = dy * scale;
		linear->c = -( ( gradient.origin().x() * m_zoomFactor + m_matrix.dx() ) * linear->a +
					   ( _y2 * m_zoomFactor ) * linear->b );
		//kdDebug() << "linear->a" << linear->a << endl;
		//kdDebug() << "linear->b" << linear->b << endl;
		//kdDebug() << "linear->c" << linear->c << endl;

		// get stop array
		int offsets = -1;
		linear->stops = buildStopArray( gradient, offsets );
		linear->n_stops = offsets;

		if( x0 != x1 && y0 != y1 )
		{
			render = art_render_new( x0, y0, x1, y1, m_buffer + 4 * int(x0) + m_width * 4 * int(y0), m_width * 4, 3, 8, ART_ALPHA_PREMUL, 0 );
			art_render_svp( render, svp );
			art_karbon_render_gradient_linear( render, linear, ART_FILTER_HYPER );
		}
	}
	else if( gradient.type() == VGradient::radial )
	{
		//kdDebug() << "x1 : " << x1 << ", x0 " << x0 << endl;
		//kdDebug() << "y1 : " << y1 << ", y0 " << y0 << endl;
		ArtGradientRadial *radial = new ArtGradientRadial();

		// TODO : make variable
		if( gradient.repeatMethod() == VGradient::none )
			radial->spread = ART_GRADIENT_PAD;
		else if( gradient.repeatMethod() == VGradient::repeat )
			radial->spread = ART_GRADIENT_REPEAT;
		else if( gradient.repeatMethod() == VGradient::reflect )
			radial->spread = ART_GRADIENT_REFLECT;

		radial->affine[0] = m_matrix.m11();
		radial->affine[1] = m_matrix.m12();
		radial->affine[2] = m_matrix.m21();
		radial->affine[3] = m_matrix.m22();
		radial->affine[4] = m_matrix.dx();
		radial->affine[5] = m_matrix.dy();

		double cx = gradient.origin().x() * m_zoomFactor;
		double cy = gradient.origin().y() * m_zoomFactor;
		double fx = cx; // TODO : fx, fy should be able to be different
		double fy = cy;
		double r = sqrt( pow( gradient.vector().x() - gradient.origin().x(), 2 ) +
						 pow( gradient.vector().y() - gradient.origin().y(), 2 ) );
		r *= m_zoomFactor;

		radial->fx = (fx - cx) / r;
		radial->fy = (fy - cy) / r;

		double aff1[6], aff2[6];
		art_affine_scale( aff1, r, r);
		art_affine_translate( aff2, cx, cy );
		art_affine_multiply( aff1, aff1, aff2 );
		art_affine_multiply( aff1, aff1, radial->affine );
		art_affine_invert( radial->affine, aff1 );

		// get stop array
		int offsets = -1;
		radial->stops = buildStopArray( gradient, offsets );
		radial->n_stops = offsets;

		if( x0 != x1 && y0 != y1 )
		{
			render = art_render_new( x0, y0, x1, y1, m_buffer + 4 * x0 + m_width * 4 * y0, m_width * 4, 3, 8, ART_ALPHA_PREMUL, 0 );
			art_render_svp( render, svp );
			art_karbon_render_gradient_radial( render, radial, ART_FILTER_HYPER );
		}
	}
	else if( gradient.type() == VGradient::conic )
	{
		ArtGradientConical *conical = new ArtGradientConical();

		// TODO : make variable
		if( gradient.repeatMethod() == VGradient::none )
			conical->spread = ART_GRADIENT_PAD;
		else if( gradient.repeatMethod() == VGradient::repeat )
			conical->spread = ART_GRADIENT_REPEAT;
		else if( gradient.repeatMethod() == VGradient::reflect )
			conical->spread = ART_GRADIENT_REFLECT;

		double cx = gradient.origin().x() * m_zoomFactor;
		double cy = gradient.origin().y() * m_zoomFactor;
		cy = m_matrix.m22() * cy + m_matrix.dy();
		double r = sqrt( pow( gradient.vector().x() - gradient.origin().x(), 2 ) +
						 pow( gradient.vector().y() - gradient.origin().y(), 2 ) );
		r *= m_zoomFactor;

		conical->cx = cx;
		conical->cy = cy;
		conical->r  = r;

		// get stop array
		int offsets = -1;
		conical->stops = buildStopArray( gradient, offsets );
		conical->n_stops = offsets;

		if( x0 != x1 && y0 != y1 )
		{
			render = art_render_new( x0, y0, x1, y1, m_buffer + 4 * x0 + m_width * 4 * y0, m_width * 4, 3, 8, ART_ALPHA_PREMUL, 0 );
			art_render_svp( render, svp );
			art_karbon_render_gradient_conical( render, conical, ART_FILTER_HYPER );
		}
	}
	if( render )
		art_render_invoke( render );
}

ArtGradientStop *
VKoPainter::buildStopArray( VGradient &gradient, int &offsets )
{
	// TODO : make this generic
	QPtrVector<VColorStop> colorStops = gradient.colorStops();
	offsets = colorStops.count();

	QMemArray<ArtGradientStop> *stopArray = new QMemArray<ArtGradientStop>();
	stopArray->resize( offsets * 2 - 1 );

	for( int offset = 0 ; offset < offsets ; offset++ )
	{
		double ramp = colorStops[ offset ]->rampPoint;
		//double mid  = colorStops[ offset ]->midPoint;
		(*stopArray)[ offset * 2 ].offset = ramp;
		//kdDebug() << " (*stopArray)[ offset * 2 ].offset : " <<  (*stopArray)[ offset * 2 ].offset << endl;

		QColor qStopColor = colorStops[ offset ]->color;
		int r = qRed( qStopColor.rgb() );
		int g = qGreen( qStopColor.rgb() );
		int b = qBlue( qStopColor.rgb() );
		art_u32 stopColor = (r << 24) | (g << 16) | (b << 8) | qAlpha(qStopColor.rgb());

		ArtPixMaxDepth color[ 4 ];
		color[ 0 ] = ART_PIX_MAX_FROM_8( (stopColor >> 24) & 0xff );
		color[ 1 ] = ART_PIX_MAX_FROM_8( (stopColor >> 16) & 0xff );
		color[ 2 ] = ART_PIX_MAX_FROM_8( (stopColor >> 8) & 0xff );
		color[ 3 ] = ART_PIX_MAX_FROM_8( (stopColor) & 0xff );

		(*stopArray)[ offset * 2 ].color[ 0 ] = color[ 0 ];
		(*stopArray)[ offset * 2 ].color[ 1 ] = color[ 1 ];
		(*stopArray)[ offset * 2 ].color[ 2 ] = color[ 2 ];
		(*stopArray)[ offset * 2 ].color[ 3 ] = color[ 3 ];

		if( offset + 1 != offsets )
		{
			//kdDebug() << " colorStops[ offset ].midPoint : " <<  colorStops[ offset ].midPoint << endl;
			//kdDebug() << " colorStops[ offset + 1 ].rampPoint : " <<  colorStops[ offset + 1 ].rampPoint << endl;
			(*stopArray)[ offset * 2 + 1 ].offset = ramp + ( colorStops[ offset + 1 ]->rampPoint - ramp ) * colorStops[ offset ]->midPoint;
			//kdDebug() << "(*stopArray)[ offset * 2  ].offset : " << (*stopArray)[ offset * 2  ].offset << endl;
			//kdDebug() << "(*stopArray)[ offset * 2 + 1 ].offset : " << (*stopArray)[ offset * 2 + 1 ].offset << endl;

			QColor qStopColor2 = colorStops[ offset + 1 ]->color;
			stopColor = int(r + ((qRed(qStopColor2.rgb()) - r)) * 0.5) << 24 |
						int(g + ((qGreen(qStopColor2.rgb()) - g)) * 0.5) << 16 |
						int(b + ((qBlue(qStopColor2.rgb()) - b)) * 0.5) << 8 |
						qAlpha(qStopColor2.rgb());

			color[ 0 ] = ART_PIX_MAX_FROM_8( (stopColor >> 24) & 0xff );
			color[ 1 ] = ART_PIX_MAX_FROM_8( (stopColor >> 16) & 0xff );
			color[ 2 ] = ART_PIX_MAX_FROM_8( (stopColor >> 8) & 0xff );
			color[ 3 ] = ART_PIX_MAX_FROM_8( (stopColor) & 0xff );

			(*stopArray)[ offset * 2 + 1 ].color[ 0 ] = color[ 0 ];
			(*stopArray)[ offset * 2 + 1 ].color[ 1 ] = color[ 1 ];
			(*stopArray)[ offset * 2 + 1 ].color[ 2 ] = color[ 2 ];
			(*stopArray)[ offset * 2 + 1 ].color[ 3 ] = color[ 3 ];
		}
	}

	offsets = offsets * 2 - 1;
	return stopArray->data();
}

void
VKoPainter::drawNode( const KoPoint& p, int width )
{
	if( !m_bDrawNodes ) return;

	KoPoint _p( m_matrix.map( QPoint( p.x() * m_zoomFactor, p.y() * m_zoomFactor ) ) );
	int x1 = _p.x() - width;
	int x2 = _p.x() + width;
	int y1 = _p.y() - width;
	int y2 = _p.y() + width;

	clampToViewport( x1, y1, x2, y2 );

	int baseindex = 4 * x1 + ( m_width * 4 * y1 );

	QColor color = m_fill->color();
	for( int i = 0; i < y2 - y1; i++ )
	{
		for( int j = 0; j < x2 - x1; j++ )
		{
			m_buffer[ baseindex + 4 * j + ( m_width * 4 * i ) ] = color.red();
			m_buffer[ baseindex + 4 * j + ( m_width * 4 * i ) + 1 ] = color.green();
			m_buffer[ baseindex + 4 * j + ( m_width * 4 * i ) + 2 ] = color.blue();
			m_buffer[ baseindex + 4 * j + ( m_width * 4 * i ) + 3 ] = 0xFF;
		}
	}
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

