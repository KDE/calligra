/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qdom.h>
#include "vpainter.h"
#include <qwmatrix.h>

#include <koPoint.h>
#include <koRect.h>

#include "vpath.h"
#include "vpath_bounding.h"

#include <kdebug.h>


VPath::VPath()
	: VObject()
{
	m_segmentLists.setAutoDelete( true );

	// add an initial segmentlist:
	m_segmentLists.append( new VSegmentList() );
}

VPath::VPath( const VPath& path )
	: VObject( path )
{
	m_segmentLists.setAutoDelete( true );

	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		m_segmentLists.append( new VSegmentList( *( itr.current() ) ) );
	}
}

VPath::~VPath()
{
}

void
VPath::draw( VPainter *painter, const QRect& rect,
	const double zoomFactor )
{
	if( state() == state_deleted )
		return;

	if( !rect.intersects( boundingBox( zoomFactor ) ) )
		return;

	painter->save();
	painter->setZoomFactor( zoomFactor );

	QPtrListIterator<VSegmentList> itr( m_segmentLists );

	if( state() != state_edit )
	{
		// paint fill:
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 2 ),
						jtr.current()->point( 3 ) );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 1 ),
						jtr.current()->point( 3 ) );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 3 ),
						jtr.current()->point( 3 ) );
				else if(
					jtr.current()->type() == segment_line ||
					jtr.current()->type() == segment_end  )
				{
					painter->lineTo( jtr.current()->point( 3 ) );
				}
				else
					painter->moveTo( jtr.current()->point( 3 ) );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( Qt::NoPen );
		painter->setBrush( fill() );
		painter->fillPath();

		// draw stroke:
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 2 ),
						jtr.current()->point( 3 ) );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 1 ),
						jtr.current()->point( 3 ) );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 3 ),
						jtr.current()->point( 3 ) );
				else if(
					jtr.current()->type() == segment_line ||
					jtr.current()->type() == segment_end  )
				{
					painter->lineTo( jtr.current()->point( 3 ) );
				}
				else
					painter->moveTo( jtr.current()->point( 3 ) );
			}
		}

		painter->setRasterOp( Qt::CopyROP );
		painter->setPen( stroke() );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
	}

	// draw simplistic contour:
	if( state() == state_edit || state() == state_selected )
	{
		for( itr.toFirst(); itr.current(); ++itr )
		{
			VSegmentListIterator jtr( *( itr.current() ) );
			for( ; jtr.current(); ++jtr )
			{
				if( jtr.current()->type() == segment_curve  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 2 ),
						jtr.current()->point( 3 ) );
				else if( jtr.current()->type() == segment_curve1  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 1 ),
						jtr.current()->point( 3 ) );
				else if( jtr.current()->type() == segment_curve2  )
					painter->curveTo(
						jtr.current()->point( 1 ),
						jtr.current()->point( 3 ),
						jtr.current()->point( 3 ) );
				else if(
					jtr.current()->type() == segment_line ||
					jtr.current()->type() == segment_end  )
				{
					painter->lineTo( jtr.current()->point( 3 ) );
				}
				else
					painter->moveTo( jtr.current()->point( 3 ) );
			}
		}

		painter->setRasterOp( Qt::XorROP );
		painter->setPen( Qt::yellow );
		painter->setBrush( Qt::NoBrush );
		painter->strokePath();
	}


/*
// TODO: convert the following to Traversers:
	VSegmentListIterator itr( m_segmentLists );

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
*/

	painter->restore();
}

const KoPoint&
VPath::currentPoint() const
{
	return
		m_segmentLists.getLast()->getLast()->point( 3 );
}

VPath&
VPath::moveTo( const double& x, const double& y )
{
	if( isClosed() ) return *this;

	// move "begin" when path is still empty:
	if( m_segmentLists.getLast()->getLast()->type() == segment_begin )
		m_segmentLists.getLast()->getLast()->setPoint( 3, KoPoint( x, y ) );
	// otherwise create a new subpath:
	else
	{
		// add an initial segmentlist:
		VSegmentList* list = new VSegmentList();

		m_segmentLists.append( list );
		m_segmentLists.getLast()->getLast()->setPoint( 3, KoPoint( x, y ) );
	}

	return *this;
}

VPath&
VPath::lineTo( const double& x, const double& y )
{
	if( isClosed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( segment_line );
	s->setPoint( 3, KoPoint( x, y ) );
	m_segmentLists.getLast()->append( s );

	return *this;
}

VPath&
VPath::curveTo(
	const double& x1, const double& y1,
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( segment_curve );
	s->setPoint( 1, KoPoint( x1, y1 ) );
	s->setPoint( 2, KoPoint( x2, y2 ) );
	s->setPoint( 3, KoPoint( x3, y3 ) );

	m_segmentLists.getLast()->append( s );

	return *this;
}

VPath&
VPath::curve1To(
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( segment_curve1 );
	s->setPoint( 2, KoPoint( x2, y2 ) );
	s->setPoint( 3, KoPoint( x3, y3 ) );

	m_segmentLists.getLast()->append( s );

	return *this;
}

VPath&
VPath::curve2To(
	const double& x1, const double& y1,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	VSegment* s = new VSegment();
	s->setType( segment_curve2 );
	s->setPoint( 1, KoPoint( x1, y1 ) );
	s->setPoint( 3, KoPoint( x3, y3 ) );

	m_segmentLists.getLast()->append( s );

	return *this;
}

VPath&
VPath::arcTo(
	const double& x1, const double& y1,
	const double& x2, const double& y2, const double& r )
{
	// parts of this routine are inspired by GNU ghostscript

	if( isClosed() ) return *this;

	// we need to calculate the tangent points. therefore calculate tangents
	// D10=P1P0 and D12=P1P2 first:
	double dx10 = currentPoint().x() - x1;
	double dy10 = currentPoint().y() - y1;
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
			bx0 != currentPoint().x() ||
			by0 != currentPoint().y() )
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
	if( m_segmentLists.getLast() == 0L )
		return *this;

	m_segmentLists.getLast()->close();

	return *this;
}

bool
VPath::isClosed() const
{
	if( m_segmentLists.getLast() == 0L )
		return false;

	return m_segmentLists.getLast()->isClosed();
}

VPath*
VPath::revert() const
{
	return 0L;
}

VPath*
VPath::booleanOp( const VPath* /*path*/, int /*type*/ ) const
{
	return 0L;
}

void
VPath::combine( const VPath& path )
{
	QPtrListIterator<VSegmentList> itr( path.m_segmentLists );
	for( ; itr.current(); ++itr )
	{
		combineSegmentList( *( itr.current() ) );
	}
}

void
VPath::combineSegmentList( const VSegmentList& segmentList )
{
	m_segmentLists.append( new VSegmentList( segmentList ) );
}

VObject&
VPath::transform( const QWMatrix& m )
{
	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		VSegmentListIterator itr2( *( itr.current() ) );
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

	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		bb.calculate( rect, zoomFactor, *( itr.current() ) );
	}
	return rect;
}

bool
VPath::intersects( const QRect& rect, const double zoomFactor ) const
{
	VPathBounding bb;
	QPtrListIterator<VSegmentList> itr( m_segmentLists );
	for( itr.toFirst(); itr.current(); ++itr )
	{
		if( bb.intersects( rect, zoomFactor, *( itr.current() ) ) )
			return true;
	}

	return false;
}

VObject*
VPath::clone()
{
	return new VPath( *this );
}

void
VPath::save( QDomElement& element ) const
{
	if( state() != state_deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "PATH" );
		element.appendChild( me );

		VObject::save( me );

		// save segmentlists:
		QPtrListIterator<VSegmentList> itr( m_segmentLists );
		for( itr.toFirst(); itr.current(); ++itr )
		{
			itr.current()->save( me );
		}
	}
}

void
VPath::load( const QDomElement& element )
{
	m_segmentLists.clear();
	setState( state_normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement child = list.item( i ).toElement();

			if( child.tagName() == "SEGMENTS" )
			{
				VSegmentList sl;
				sl.load( child );
				combineSegmentList( sl );
			}
			else
			{
				VObject::load( child );
			}
		}
	}
}
