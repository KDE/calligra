/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qdom.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "vpath.h"
#include "vpath_bounding.h"

#include <kdebug.h>


VPath::VPath()
	: VObject(), m_closed( false )
{
	m_segments.setAutoDelete( true );
	m_holes.setAutoDelete( true );

	// add a initial segment at (0,0):
	m_segments.append( new VSegment() );

	m_stroke.setLineWidth( 3.0 );
}

VPath::VPath( const VPath& path )
	: VObject()
{
	// outline:
	m_segments.setAutoDelete( true );
	m_segments.clear();

	VSegmentListIterator itr( path.m_segments );
	for( ; itr.current() ; ++itr )
	{
		m_segments.append( new VSegment( *( itr.current() ) ) );
	}

	// holes:
	m_holes.setAutoDelete( true );
	m_holes.clear();

	VSegmentList* hole;

	QPtrListIterator<VSegmentList> holeItr( path.m_holes );
	for( holeItr.toFirst(); holeItr.current(); ++holeItr )
	{
		hole = new VSegmentList();
		hole->setAutoDelete( true );

		VSegmentListIterator itr2( *( holeItr.current() ) );
		for( ; itr2.current() ; ++itr2 )
		{
			hole->append( new VSegment( *( itr2.current() ) ) );
		}

		m_holes.append( hole );
	}
}

VPath::~VPath()
{
}

void
VPath::draw( QPainter& painter, const QRect& rect,
	const double zoomFactor )
{
	if( state() == deleted )
		return;

	if( !rect.intersects( boundingBox( zoomFactor ) ) )
		return;

	painter.save();
	QPtrListIterator<VSegmentList> holeItr( m_holes );

	if( state() != edit )
	{
		// paint fill:
		m_fill.begin_draw( painter, zoomFactor );
		m_fill.draw( m_segments );		// contour.
		// holes:
		for( holeItr.toFirst(); holeItr.current(); ++holeItr )
		{
			m_fill.draw( *( holeItr.current() ), true );
		}
		m_fill.end_draw();

		// draw stroke:
		// contour:
		m_stroke.draw( painter, zoomFactor, m_segments );
		// holes:
		for( holeItr.toFirst(); holeItr.current(); ++holeItr )
		{
			m_stroke.draw( painter, zoomFactor, *( holeItr.current() ) );
		}
	}

	// draw simplistic contour:
	if( state() == edit || state() == selected )
	{
		// contour:
		m_stroke.draw( painter, zoomFactor, m_segments, true );

		// holes:
		for( holeItr.toFirst(); holeItr.current(); ++holeItr )
		{
			m_stroke.draw( painter, zoomFactor, *( holeItr.current() ), true );
		}
	}



// TODO: convert the following to Traversers:
	VSegmentListIterator itr( m_segments );

	if( state() == selected )
	{
		painter.setRasterOp( Qt::CopyROP );
		painter.setPen( Qt::NoPen );
		painter.setBrush( Qt::blue.light() );

		// draw small boxes for path nodes
		for( itr.toFirst(); itr.current(); ++itr )
		{
			// draw boxes:
			drawBox( painter,
				qRound( zoomFactor * itr.current()->point( 3 ).x() ),
				qRound( zoomFactor * itr.current()->point( 3 ).y() ), 3 );

		}
	}
	else if( state() == edit )
	{
		painter.setRasterOp( Qt::XorROP );
		painter.setPen( Qt::yellow );
		painter.setBrush( Qt::NoBrush );

		for( itr.toFirst(); itr.current(); ++itr )
		{
			// draw boxes:
			painter.setPen( Qt::black );

			drawBox( painter,
				qRound( zoomFactor * itr.current()->point( 3 ).x() ),
				qRound( zoomFactor * itr.current()->point( 3 ).y() ) );
		}

	}

	painter.restore();
}

void
VPath::drawBox( QPainter& painter, double x, double y, uint handleSize )
{
	painter.drawRect( x - handleSize, y - handleSize,
					  handleSize*2 + 1, handleSize*2 + 1 );
}

const KoPoint&
VPath::currentPoint() const
{
	return m_segments.getLast()->point( 3 );
}

VPath&
VPath::moveTo( const double& x, const double& y )
{
	if( closed() ) return *this;

	if( m_segments.getLast()->type() == VSegment::begin )
	{
		m_segments.getLast()->setPoint( 3, KoPoint( x, y ) );
	}

	return *this;
}

VPath&
VPath::lineTo( const double& x, const double& y )
{
	if( closed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( VSegment::line );
	s->setPoint( 3, KoPoint( x, y ) );
	m_segments.append( s );

	return *this;
}

VPath&
VPath::curveTo(
	const double& x1, const double& y1,
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( closed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( VSegment::curve );
	s->setPoint( 1, KoPoint( x1, y1 ) );
	s->setPoint( 2, KoPoint( x2, y2 ) );
	s->setPoint( 3, KoPoint( x3, y3 ) );

	m_segments.append( s );

	return *this;
}

VPath&
VPath::curve1To(
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( closed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( VSegment::curve1 );
	s->setPoint( 2, KoPoint( x2, y2 ) );
	s->setPoint( 3, KoPoint( x3, y3 ) );

	m_segments.append( s );

	return *this;
}

VPath&
VPath::curve2To(
	const double& x1, const double& y1,
	const double& x3, const double& y3 )
{
	if( closed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( VSegment::curve2 );
	s->setPoint( 1, KoPoint( x1, y1 ) );
	s->setPoint( 3, KoPoint( x3, y3 ) );

	m_segments.append( s );

	return *this;
}

VPath&
VPath::arcTo(
	const double& x1, const double& y1,
	const double& x2, const double& y2, const double& r )
{
	// parts of this routine are inspired by GNU ghostscript

	if( closed() ) return *this;

	// we need to calculate the tangent points. therefore calculate tangents
	// D10=P1P0 and D12=P1P2 first:
	double dx10 = m_segments.getLast()->point( 3 ).x() - x1;
	double dy10 = m_segments.getLast()->point( 3 ).y() - y1;
	double dx12 = x2 - x1;
	double dy12 = y2 - y1;

	// calculate distance squares:
	double dsq10 = dx10*dx10 + dy10*dy10;
	double dsq12 = dx12*dx12 + dy12*dy12;

	// we now calculate tan(a/2) where a is the angular between D10 and D12.
	// we take advantage of D10*D12=d10*d12*cos(a), |D10xD12|=d10*d12*sin(a)
	// (cross product) and tan(a/2)=sin(a)/[1-cos(a)].
	double num   = dx10*dy12 - dy10*dx12;
	double denom = sqrt( dsq10*dsq12 ) - dx10*dx12 + dy10*dy12;

	if( 1.0 + denom == 1.0 )	// points are co-linear
		lineTo( x1, y1 );	// just add a line to first point
    else
    {
		// calculate distances from P1 to tangent points:
		double dist = fabs( r*num / denom );
		double d1t0 = dist / sqrt(dsq10);
		double d1t1 = dist / sqrt(dsq12);

// TODO: check for r<0

		double bx0 = x1 + dx10*d1t0;
		double by0 = y1 + dy10*d1t0;

		// if(bx0,by0) deviates from current point, add a line to it:
// TODO: decide via radius<XXX or sthg?
		if(
			bx0 != m_segments.getLast()->point( 3 ).x() ||
			by0 != m_segments.getLast()->point( 3 ).y() )
		{
			lineTo( bx0, by0 );
		}

		double bx3 = x1 + dx12*d1t1;
		double by3 = y1 + dy12*d1t1;

		// the two bezier-control points are located on the tangents at a fraction
		// of the distance [tangent points<->tangent intersection].
		double distsq = (x1 - bx0)*(x1 - bx0) + (y1 - by0)*(y1 - by0);
		double rsq = r*r;
		double fract;

// TODO: make this nicer?

		if( distsq >= rsq * 1.0e8 ) // r is very small
			fract = 0.0; // dist==r==0
		else
			fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ) );

		double bx1 = bx0 + (x1 - bx0) * fract;
		double by1 = by0 + (y1 - by0) * fract;
		double bx2 = bx3 + (x1 - bx3) * fract;
		double by2 = by3 + (y1 - by3) * fract;

		// finally add the bezier-segment:
		curveTo( bx1, by1, bx2, by2, bx3, by3 );
	}

	return *this;
}

VPath&
VPath::close()
{
	if( closed() ) return *this;

// TODO: add tolerance
	if( currentPoint() != m_segments.getFirst()->point( 3 ) )
	{
		VSegment* s = new VSegment();
		s->setType( VSegment::end );
		s->setPoint( 3, m_segments.getFirst()->point( 3 ) );
		m_segments.append( s );
	}

	m_closed = true;

	return *this;
}

VPath*
VPath::revert() const
{
	return 0L;
}

VPath*
VPath::booleanOp( const VPath* path, int /*type*/ ) const
{
	return 0L;
}

void
VPath::combine( const VPath& path )
{
// TODO: inside/intersection checks needed
	VSegmentList* hole = new VSegmentList();
	VSegmentListIterator itr( path.m_segments );

	hole->setAutoDelete( true );

	for( ; itr.current(); ++itr )
	{
		hole->append( new VSegment( *( itr.current() ) ) );
	}

	m_holes.append( hole );
}

VObject&
VPath::transform( const QWMatrix& m )
{
	// outline:
	VSegmentListIterator itr( m_segments );
	for( ; itr.current() ; ++itr )
	{
		itr.current()->setPoint( 1, itr.current()->point( 1 ).transform( m ) );
		itr.current()->setPoint( 2, itr.current()->point( 2 ).transform( m ) );
		itr.current()->setPoint( 3, itr.current()->point( 3 ).transform( m ) );
	}

	// holes:
	QPtrListIterator<VSegmentList> holeItr( m_holes );
	for( holeItr.toFirst(); holeItr.current(); ++holeItr )
	{
		VSegmentListIterator itr2( *( holeItr.current() ) );
		for( ; itr2.current() ; ++itr2 )
		{
			itr2.current()->setPoint( 1, itr2.current()->point( 1 ).transform( m ) );
			itr2.current()->setPoint( 2, itr2.current()->point( 2 ).transform( m ) );
			itr2.current()->setPoint( 3, itr2.current()->point( 3 ).transform( m ) );
		}
	}

	return *this;
}

QRect
VPath::boundingBox( const double zoomFactor ) const
{
	QRect rect;
	VPathBounding bb;
	bb.calculate( rect, zoomFactor, m_segments );

	return rect;
}

bool
VPath::intersects( const QRect& rect, const double zoomFactor ) const
{
	VPathBounding bb;
	return bb.intersects( rect, zoomFactor, m_segments );
}

VObject*
VPath::clone()
{
	return new VPath( *this );
}

void
VPath::save( QDomElement& element ) const
{
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "PATH" );
		element.appendChild( me );
		me.setAttribute( "closed", m_closed );

		// outline:
		QDomElement outline = element.ownerDocument().createElement( "OUTLINE" );
		me.appendChild( outline );

		VSegmentListIterator itr( m_segments );
		for( ; itr.current() ; ++itr )
		{
			itr.current()->save( outline );
		}

		// holes:
		QPtrListIterator<VSegmentList> holeItr( m_holes );
		for( holeItr.toFirst(); holeItr.current(); ++holeItr )
		{
			QDomElement hole = element.ownerDocument().createElement( "HOLE" );
			me.appendChild( hole );

			VSegmentListIterator itr2( *( holeItr.current() ) );
			for( ; itr2.current() ; ++itr2 )
			{
				itr2.current()->save( hole );
			}
		}
	}
}

void
VPath::load( const QDomElement& element )
{
}
