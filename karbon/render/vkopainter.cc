/* This file is part of the KDE project.
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#include <X11/Xlib.h>

#include <gdk-pixbuf-xlibrgb.h>

#include <kdebug.h>
#include <kglobal.h>
#include <math.h>
#include <stdlib.h>

#include <koPoint.h>
#include <koRect.h>

#include "agg_basics.h"
#include "agg_color_rgba8.h"
#include "agg_path_storage.h"
#include "agg_rendering_buffer.h"
#include "agg_renderer_rgba32_basic.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer.h"
#include "agg_conv_curve.h"
#include "agg_conv_stroke.h"
#include "agg_conv_dash.h"
#include "agg_conv_transform.h"
#include "agg_affine_matrix.h"
#include "agg_span_bgra32_image.h"
#include "agg_gradient_attr.h"
#include "agg_gradient_functions.h"
#include "agg_rounded_rect.h"

unsigned char default_gamma[] =
{
0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,
9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18,
19, 19, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28,
29, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 37, 38,
39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49,
49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60,
60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 71, 71,
72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 83, 84,
85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
100,101,101,102,103,104,105,106,107,108,109,110,111,112,114,115,
116,117,118,119,120,121,122,123,124,126,127,128,129,130,131,132,
134,135,136,137,139,140,141,142,144,145,146,147,149,150,151,153,
154,155,157,158,159,161,162,164,165,166,168,169,171,172,174,175,
177,178,180,181,183,184,186,188,189,191,192,194,195,197,199,200,
202,204,205,207,209,210,212,214,215,217,219,220,222,224,225,227,
229,230,232,234,236,237,239,241,242,244,246,248,249,251,253,255
};


#define pix_format agg::pix_format_rgba32
typedef agg::renderer_rgba32_solid renderer_solid;
typedef agg::renderer_rgba32_util  renderer_util;
template<class F> class renderer_gradient :
public agg::renderer_rgba32_gradient<F>
{
public:
    renderer_gradient(agg::rendering_buffer& rbuf) :
            agg::renderer_rgba32_gradient<F>(rbuf) {}
};
typedef agg::renderer_rgba32_gouraud renderer_gouraud;

using namespace agg;

typedef conv_curve<path_storage> curved;
typedef conv_stroke<curved>                     curved_stroked;
typedef conv_transform<curved_stroked>          curved_stroked_trans;
typedef conv_dash<curved> 			curved_dashed;
typedef conv_stroke<curved_dashed> 		curved_dashed_stroked;
typedef conv_transform<curved_dashed_stroked>   curved_dashed_stroked_trans;


class gradient_polymorphic_wrapper_base
{
public:
	virtual int calculate(int x, int y, int) const = 0;
};

template<class GradientF>
class gradient_polymorphic_wrapper : public gradient_polymorphic_wrapper_base
{
public:
	virtual int calculate(int x, int y, int) const;
private:
	GradientF m_gradient;
};

template<class GradientF> int
gradient_polymorphic_wrapper<GradientF>::calculate(int x, int y, int d) const
{
	return m_gradient.calculate(x, y, d);
}

VKoPainter::VKoPainter( QPaintDevice *target, unsigned int w, unsigned int h, bool bDrawNodes )
: VPainter( target, w, h ), m_target( target ), m_bDrawNodes( bDrawNodes )
{
	//kdDebug() << "w : " << w << endl;
	//kdDebug() << "h : " << h << endl;
	m_width = w;//( w > 0 ) ? w : target->width();
	m_height= h;//( h > 0 ) ? h : target->height();
	m_buffer = 0L;
	m_path = new path_storage();
	m_buf = new rendering_buffer();
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

VKoPainter::VKoPainter( unsigned char *buffer, unsigned int w, unsigned int h, bool bDrawNodes )
: VPainter( 0L, w, h ), m_buffer( buffer ), m_bDrawNodes( bDrawNodes )
{
	//kdDebug() << "w : " << w << endl;
	//kdDebug() << "h : " << h << endl;
	m_target = 0L;
	m_width = w;
	m_height= h;
	m_index = 0;
	m_buf = new rendering_buffer();
	m_path = new path_storage();
	clear();

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
		free( m_buffer );

	delete m_stroke;
	delete m_fill;

	if( gc )
		XFreeGC( m_target->x11Display(), gc );
}

void
VKoPainter::resize( unsigned int w, unsigned int h )
{
	if( !m_buffer || w != m_width || h != m_height )
	{
		// TODO : realloc?
		free( m_buffer );
		m_buffer = 0;
		m_width = w;
		m_height = h;
		if ( m_width != 0 && m_height != 0 )
			m_buffer = (unsigned char *)malloc( m_width * m_height * 4 );
		m_buf->attach( m_buffer, m_width, m_height, m_width * 4 );
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
	int x		= KMAX( 0, int( r.x() ) );
	int y		= KMAX( 0, int( r.y() ) );
	int width	= KMIN( m_width,	(unsigned int)KMAX( 0, int( r.x() + r.width() ) ) );
	int height	= KMIN( m_height,	(unsigned int)KMAX( 0, int( r.y() + r.height() ) ) );
	xlib_draw_rgb_32_image( m_target->handle(), gc, x, y, width - x, height - y,
							XLIB_RGB_DITHER_NONE, m_buffer + (x * 4) + (y * m_width * 4), m_width * 4 );
}

void
VKoPainter::clear()
{
	if( m_buffer )
		memset( m_buffer, qRgba( 255, 255, 255, 255 ), m_width * m_height * 4 );
}

void
VKoPainter::clear( const QColor &c )
{
	if( m_buffer )
		memset( m_buffer, c.rgb(), m_width * m_height * 4 );
}

void
VKoPainter::clear( const KoRect &r, const QColor &c )
{
	unsigned int color = c.rgb();
	int x		= KMAX( 0, int( r.x() ) );
	int y		= KMAX( 0, int( r.y() ) );
	int width	= KMIN( m_width,	(unsigned int)KMAX( 0, int( r.x() + r.width() ) ) );
	int height	= KMIN( m_height,	(unsigned int)KMAX( 0, int( r.y() + r.height() ) ) );
	if( m_buffer )
	{
		for( int i = y;i < height;i++)
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
VKoPainter::moveTo( const KoPoint &p )
{
	m_path->move_to( p.x() * m_zoomFactor, p.y() * m_zoomFactor );
}

void 
VKoPainter::lineTo( const KoPoint &p )
{
	m_path->line_to( p.x() * m_zoomFactor, p.y() * m_zoomFactor );
}

void
VKoPainter::curveTo( const KoPoint &p1, const KoPoint &p2, const KoPoint &p3 )
{
	m_path->curve4( p1.x() * m_zoomFactor, p1.y() * m_zoomFactor, p2.x() * m_zoomFactor, p2.y() * m_zoomFactor, p3.x() * m_zoomFactor, p3.y() * m_zoomFactor );
}

void
VKoPainter::newPath()
{
	m_path->remove_all();
	m_path->add_new_path();
}

void
VKoPainter::setFillRule( VFillRule fillRule )
{
	m_fillRule = fillRule;
}

void
buildStopArray( VGradient &gradient, rgba8 *color_profile, int opa )
{
	// TODO : make this generic
	QPtrVector<VColorStop> colorStops = gradient.colorStops();
	int offsets = colorStops.count();

	unsigned int index = 0;
	double rstep, gstep, bstep;
	int r, g, b, steps;
	int oldr, oldg, oldb;
	for( int offset = 0; offset < offsets; offset++ )
	{
		double ramp = colorStops[ offset ]->rampPoint;
		steps = ramp * 256 - index;
		QColor qStopColor = colorStops[ offset ]->color;
		r = qRed( qStopColor.rgb() );
		g = qGreen( qStopColor.rgb() );
		b = qBlue( qStopColor.rgb() );
		if( offset == 0 )
		{
			rstep = gstep = bstep = 0;
			oldr = r;
			oldg = g;
			oldb = b;
		}
		else
		{
			rstep = ( r - oldr ) / steps;
			gstep = ( g - oldg ) / steps;
			bstep = ( b - oldb ) / steps;
		}
		for( int i = 0; i < steps;i++ )
		{
			//kdDebug() << "Setting : " << index + i << endl;
			color_profile[index + i].r = oldr + rstep * i;
			color_profile[index + i].g = oldg + gstep * i;
			color_profile[index + i].b = oldb + bstep * i;
			color_profile[index + i].a = opa;
		}
		index += steps;
		oldr = r;
		oldg = g;
		oldb = b;
	}
	for( ;index < 256; index++ )
	{
		//kdDebug() << "Setting : " << index << endl;
		color_profile[index].r = r;
		color_profile[index].g = g;
		color_profile[index].b = b;
		color_profile[index].a = opa;
	}
}

void
VKoPainter::fillPath()
{
	if( m_fill && m_fill->type() != VFill::none )
	{
		agg::renderer_rgba32_solid ren(*m_buf);
		rasterizer<scanline_u8, agg::gamma8> ras;
		typedef conv_curve<path_storage> curved;
		typedef conv_transform<curved> curved_trans;
		curved c(*m_path);

		affine_matrix mat( m_matrix.m11(), 0, 0, m_matrix.m22(), m_matrix.dx(), m_matrix.dy() );

		ras.reset();
		curved_trans cs(c, mat);

		ras.filling_rule( m_fillRule == evenOdd ? fill_even_odd : fill_non_zero);

		if( m_fill->type() == VFill::solid )
		{
			QColor col = m_fill->color();
			rgba8 color( col.red(), col.green(), col.blue() );
			color.opacity( m_fill->color().opacity() );
			ren.attribute(color);

			ras.add_path(cs);
			ras.render(ren);
		}
		else if( m_fill->type() == VFill::grad )
		{
			VGradient gradient = m_fill->gradient();
			float opa = m_fill->color().opacity();
			// TODO : make variable
			/*if( gradient.repeatMethod() == VGradient::none )
				linear->spread = ART_GRADIENT_PAD;
			else if( gradient.repeatMethod() == VGradient::repeat )
				linear->spread = ART_GRADIENT_REPEAT;
			else if( gradient.repeatMethod() == VGradient::reflect )
				linear->spread = ART_GRADIENT_REFLECT;*/

			double _x1 = gradient.origin().x();
			double _x2 = gradient.vector().x();
			double _y1 = gradient.origin().y();
			double _y2 = gradient.vector().y();
			_y1 = m_matrix.m22() * _y1 + m_matrix.dy() / m_zoomFactor;
			_y2 = m_matrix.m22() * _y2 + m_matrix.dy() / m_zoomFactor;

			rgba8 color_profile[256];
			buildStopArray( gradient, color_profile, opa * 255 );

			affine_matrix mtx_g1;
			//mtx_g1 *= agg::scaling_matrix(m_scale_x, m_scale_y);
			mtx_g1 *= agg::rotation_matrix( atan2( _y2 - _y1, _x2 - _x1 ) );
			mtx_g1 *= agg::translation_matrix( _x1, _y1 );
			//mtx_g1 *= resizing_matrix();
			mtx_g1.invert();

			gradient_polymorphic_wrapper<agg::gradient_x> gr_x;
			gradient_polymorphic_wrapper<agg::gradient_circle> gr_circle;
			gradient_polymorphic_wrapper_base* gr_ptr;
			if( gradient.type() == VGradient::linear )
				gr_ptr = &gr_x;
			else
				gr_ptr = &gr_circle;
			typedef renderer_gradient <gradient_polymorphic_wrapper_base> span_renderer_type;

			span_renderer_type r1( *m_buf );
			span_renderer_type::attr_type g1( mtx_g1, *gr_ptr, color_profile, default_gamma, 0, 150 );//abs( _x2 - _x1 ) );
			r1.attribute( g1 );

			ras.add_path( cs );
			ras.render( r1 );
		}
	}
}

void
VKoPainter::strokePath()
{
	if( m_stroke && m_stroke->type() != VStroke::none )
	{
		agg::renderer_rgba32_solid ren( *m_buf );
		rasterizer<scanline_u8, agg::gamma8> ras;
		curved c( *m_path );

		affine_matrix mat( m_matrix.m11(), 0, 0, m_matrix.m22(), m_matrix.dx(), m_matrix.dy() );

		ras.reset();
		double ratio = m_zoomFactor;//sqrt(pow(affine[0], 2) + pow(affine[3], 2)) / sqrt(2);

		gen_stroke::line_cap_e cap;
		// caps translation karbon -> agg2
		if( m_stroke->lineCap() == VStroke::capRound )
			cap = gen_stroke::round_cap;
		else if( m_stroke->lineCap() == VStroke::capSquare )
			cap = gen_stroke::square_cap;
		else
			cap = gen_stroke::butt_cap;

		gen_stroke::line_join_e join;
		// join translation karbon -> agg2
		if( m_stroke->lineJoin() == VStroke::joinRound )
			join = gen_stroke::round_join;
		else if( m_stroke->lineJoin() == VStroke::joinBevel )
			join = gen_stroke::bevel_join;
		else
			join = gen_stroke::miter_join;

		if( m_stroke->dashPattern().array().count() > 0 )
		{
			// there are dashes to be rendered
			curved_dashed path_dash( c );
			curved_dashed_stroked path2( path_dash );
			curved_dashed_stroked_trans path3( path2, mat );
			double offset = m_stroke->dashPattern().offset() * ratio;
			path_dash.dash_start( offset );
			int ndashes = m_stroke->dashPattern().array().count();
			for( int i = 0; i + 1 < ndashes; i += 2 )
				path_dash.add_dash( m_stroke->dashPattern().array()[i] * ratio,
							m_stroke->dashPattern().array()[i + 1] * ratio );
			path2.width( ratio * m_stroke->lineWidth() );
			path2.line_cap( cap );
			path2.line_join( join );
			path2.miter_limit( m_stroke->miterLimit() );
			ras.add_path( path3 );
		}
		else
		{
			curved_stroked cs( c );
			cs.width( ratio * m_stroke->lineWidth() );
			cs.line_cap( cap );
			cs.line_join( join );
			cs.miter_limit( m_stroke->miterLimit() );
			curved_stroked_trans cst( cs, mat );
			ras.add_path( cst );
		}
		QColor col = m_stroke->color();
		rgba8 color( col.red(), col.green(), col.blue() );
		color.opacity( m_stroke->color().opacity() );
		ren.attribute( color );
		ras.render( ren );
	}
}

void
VKoPainter::setClipPath()
{
}

void
VKoPainter::resetClipPath()
{
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

void
VKoPainter::drawNode( const KoPoint& p, int width )
{
	if( !m_bDrawNodes ) return;

	KoPoint _p( m_matrix.map( QPoint( p.x() * m_zoomFactor, p.y() * m_zoomFactor ) ) );
	int x1 = int( _p.x() - width );
	int x2 = int( _p.x() + width );
	int y1 = int( _p.y() - width );
	int y2 = int( _p.y() + width );

	//clampToViewport( x1, y1, x2, y2 );

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
VKoPainter::drawImage( const QImage &image, const QWMatrix &affine )
{
	//QWMatrix *r= m_gstate->matrix();
	affine_matrix mat( m_matrix.m11(), 0, 0, m_matrix.m22(), m_matrix.dx(), m_matrix.dy() );
	//affine_matrix mat;//( r->m11(), r->m12(), r->m21(), r->m22(), r->dx(), r->dy() );

	// Version with "hardcoded" bilinear filter
	typedef image_transform_attr<null_distortions,
					  null_color_alpha,
					  null_gradient_alpha> attr_type;

	typedef span_bgra32_image_bilinear<attr_type> renderer;

	null_distortions    distortions;
	null_color_alpha    color_alpha;
	null_gradient_alpha gradient_alpha;

	rendering_buffer rbuf_img;
	rbuf_img.attach( image.bits(), image.width(), image.height(), image.width() * 4 );

	attr_type attr( rbuf_img, mat, distortions, color_alpha, gradient_alpha );

	renderer_u8<renderer> ri( *m_buf );
	ri.attribute(attr);

	rasterizer<scanline_u8> ras;
	rounded_rect rect;
	rect.rect( 0, 0, image.width(), image.height() );
	conv_transform<rounded_rect> tr( rect, mat );

	ras.add_path( rect );
	ras.render(ri);
}

void
VKoPainter::drawRect( const KoRect &r )
{
	newPath();
	moveTo( r.topLeft() );
	lineTo( r.topRight() );
	lineTo( r.bottomRight() );
	lineTo( r.bottomLeft() );
	lineTo( r.topLeft() );
	fillPath();
	strokePath();
}

void
VKoPainter::drawRect( double x, double y, double w, double h )
{
	drawRect( KoRect( x, y, w, h ) );
}

