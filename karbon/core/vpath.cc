/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <kdebug.h>

#include <math.h>
#include <qpainter.h>

#include "vaffinemap.h"
#include "vpath.h"
#include "vpoint.h"

// TODO:
// - make sure that lastpoint==currenpoint doesnt get removed
// - set m_isDirty everywhere
// - reimplement quadBezier() ?

double
VSegment::s_lastZoomFactor = 0.0;

VSegment:: VSegment()
	: m_QPointArray(1), m_isDirty( true )
{
}

VSegment:: VSegment( const double lpX, const double lpY )
	: m_lastPoint( lpX, lpY ), m_QPointArray(1), m_isDirty( true )
{
}

// -----------------------------------

VFirstPoint::VFirstPoint( const double lpX, const double lpY )
	: VSegment( lpX, lpY )
{
}

void
VFirstPoint::movePointTo( const VPoint* point, const double x, const double y,
	const bool smooth )
{
	if ( point == &m_lastPoint )
	{
		m_lastPoint.moveTo( x, y );
		m_isDirty = true;
	}
}

void
VFirstPoint::transform( const VAffineMap& affMap )
{
	m_lastPoint = affMap.map( m_lastPoint );
	m_isDirty = true;
}

const QPointArray&
VFirstPoint::getQPointArray( const VSegment* prevSeg,
	const double zoomFactor ) const
{
	if ( m_isDirty || zoomFactor != s_lastZoomFactor )
	{
		m_QPointArray.setPoint( 0, m_lastPoint.getQPoint( zoomFactor ) );

		s_lastZoomFactor = zoomFactor;
		m_isDirty = false;
	}
	return m_QPointArray;
}

// -----------------------------------

VLine::VLine( const double lpX, const double lpY )
	: VSegment( lpX, lpY )
{
}

void
VLine::movePointTo( const VPoint* point, const double x, const double y,
	const bool smooth )
{
	if ( point == &m_lastPoint )
	{
		m_lastPoint.moveTo( x, y );
		m_isDirty = true;
	}
}

const VSegment*
VLine::revert( const VSegment* prevSeg )
{
	return new VLine(
		prevSeg->lastPoint()->x(),
		prevSeg->lastPoint()->y() );
}

void
VLine::transform( const VAffineMap& affMap )
{
	m_lastPoint = affMap.map( m_lastPoint );
	m_isDirty = true;
}

const QPointArray&
VLine::getQPointArray( const VSegment* prevSeg, const double zoomFactor ) const
{
	if ( m_isDirty || zoomFactor != s_lastZoomFactor )
	{
		m_QPointArray.setPoint( 0, m_lastPoint.getQPoint( zoomFactor ) );

		s_lastZoomFactor = zoomFactor;
		m_isDirty = false;
	}
	return m_QPointArray;
}

// -----------------------------------

VCurve::VCurve(
		const double fcpX, const double fcpY,
		const double lcpX, const double lcpY,
		const double lpX, const double lpY )
	: VSegment( lpX, lpY ),
	  m_firstCtrlPoint( fcpX, fcpY ),
	  m_lastCtrlPoint( lcpX, lcpY )
{
}

void
VCurve::movePointTo( const VPoint* point, const double x, const double y,
	const bool smooth )
{
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

const VSegment*
VCurve::revert( const VSegment* prevSeg )
{
	return new VCurve(
		m_lastCtrlPoint.x(), m_lastCtrlPoint.y(),
		m_firstCtrlPoint.x(), m_firstCtrlPoint.y(),
		prevSeg->lastPoint()->x(), prevSeg->lastPoint()->y() );
}

void
VCurve::transform( const VAffineMap& affMap )
{
	m_firstCtrlPoint	= affMap.map( m_firstCtrlPoint );
	m_lastCtrlPoint		= affMap.map( m_lastCtrlPoint );
	m_lastPoint			= affMap.map( m_lastPoint );

	m_isDirty = true;
}

const QPointArray&
VCurve::getQPointArray( const VSegment* prevSeg,
	const double zoomFactor ) const
{
	if ( m_isDirty || zoomFactor != s_lastZoomFactor )
	{
		// calc all QPoints via a temporary QPointArray and quadBezier():
		QPointArray pa(4);
		pa.setPoint( 0, firstPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 1, firstCtrlPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 2, lastCtrlPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 3, lastPoint()->getQPoint( zoomFactor ) );

		pa = pa.quadBezier();

		// copy pa=>m_QPointArray but skip first point:
		m_QPointArray.resize( pa.size() - 1 );
		for ( uint i = 0; i < m_QPointArray.size() ; ++i )
			m_QPointArray.setPoint( i, pa.point( i + 1 ) );

		s_lastZoomFactor = zoomFactor;
		m_isDirty = false;
	}
	return m_QPointArray;
}

// -----------------------------------

VCurve1::VCurve1(
		const double lcpX, const double lcpY, const double lpX, const double lpY )
	: VSegment( lpX, lpY ), m_lastCtrlPoint( lcpX, lcpY )
{
}

void
VCurve1::movePointTo( const VPoint* point, const double x, const double y,
	const bool smooth )
{
}

const VSegment*
VCurve1::revert( const VSegment* prevSeg )
{
	return new VCurve1();
}

void
VCurve1::transform( const VAffineMap& affMap )
{
	m_lastCtrlPoint		= affMap.map( m_lastCtrlPoint );
	m_lastPoint			= affMap.map( m_lastPoint );

	m_isDirty = true;
}

const QPointArray&
VCurve1::getQPointArray( const VSegment* prevSeg,
	const double zoomFactor ) const
{
	if ( m_isDirty || zoomFactor != s_lastZoomFactor )
	{
		// calc all QPoints via a temporary QPointArray and quadBezier():
		QPointArray pa(4);
		pa.setPoint( 0, firstPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 1, firstCtrlPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 2, lastCtrlPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 3, lastPoint()->getQPoint( zoomFactor ) );

		pa = pa.quadBezier();

		// copy pa=>m_QPointArray but skip first point:
		m_QPointArray.resize( pa.size() - 1 );
		for ( uint i = 0; i < m_QPointArray.size() ; ++i )
			m_QPointArray.setPoint( i, pa.point( i + 1 ) );

		s_lastZoomFactor = zoomFactor;
		m_isDirty = false;
	}
	return m_QPointArray;
}

// -----------------------------------

VCurve2::VCurve2(
		const double fcpX, const double fcpY, const double lpX, const double lpY )
	: VSegment( lpX, lpY ), m_firstCtrlPoint( fcpX, fcpY )
{
}

void
VCurve2::movePointTo( const VPoint* point, const double x, const double y,
	const bool smooth )
{
}

const VSegment*
VCurve2::revert( const VSegment* prevSeg )
{
	return new VCurve2();
}

void
VCurve2::transform( const VAffineMap& affMap )
{
	m_firstCtrlPoint	= affMap.map( m_firstCtrlPoint );
	m_lastPoint			= affMap.map( m_lastPoint );

	m_isDirty = true;
}

const QPointArray&
VCurve2::getQPointArray( const VSegment* prevSeg,
	const double zoomFactor ) const
{
	if ( m_isDirty || zoomFactor != s_lastZoomFactor )
	{
		// calc all QPoints via a temporary QPointArray and quadBezier():
		QPointArray pa(4);
		pa.setPoint( 0, firstPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 1, firstCtrlPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 2, lastCtrlPoint( prevSeg )->getQPoint( zoomFactor ) );
		pa.setPoint( 3, lastPoint()->getQPoint( zoomFactor ) );

		pa = pa.quadBezier();

		// copy pa=>m_QPointArray but skip first point:
		m_QPointArray.resize( pa.size() - 1 );
		for ( uint i = 0; i < m_QPointArray.size() ; ++i )
			m_QPointArray.setPoint( i, pa.point( i + 1 ) );

		s_lastZoomFactor = zoomFactor;
		m_isDirty = false;
	}
	return m_QPointArray;
}

// -----------------------------------
// -----------------------------------

VPath::VPath()
	: VObject(), m_isClosed( false )
{
	// create a first (current) point [0.0,0.0]:
	m_segments.append( new VFirstPoint() );
}

VPath::VPath( const VPath& path )
	: VObject()
{
// TODO: implement copy-ctor
}

VPath::~VPath()
{
// TODO: should we be polite and destruct the QLists as well ?
	QListIterator<VSegment> i( m_segments );
	for ( ; i.current() ; ++i )
		delete( i.current() );
}

void
VPath::draw( QPainter& painter, const QRect& rect, const double zoomFactor )
{
	if ( isDeleted() ) return;

	painter.save();

	// >>> draw the path contour >>>
	QListIterator<VSegment> i( m_segments );
	const VSegment* prev_seg( 0L );	// pointer to previous segment
	QPointArray qpa;

	// skip first point when path is closed:
	if ( isClosed() )
	{
		prev_seg = i.current();
		++i;
	}

	for ( ; i.current(); ++i )
	{
		const QPointArray& seg_qpa =
			i.current()->getQPointArray( prev_seg, zoomFactor);

		uint old_size( qpa.size() );
		uint add_size( seg_qpa.size() );

		qpa.resize( old_size + add_size );

		for ( uint j = 0; j < add_size; ++j )
			qpa.setPoint( old_size + j, seg_qpa.point( j ) );

		prev_seg = i.current();	// remember previous segment
	}

// TODO: remove hardcoded values:
	painter.setPen( Qt::black );
	painter.setBrush( QColor( 205, 205, 205 ) );

// TODO: filling not-closed shapes?
	// draw open or closed path ?
	if ( isClosed() )
		painter.drawPolygon( qpa );
	else
		painter.drawPolyline( qpa );

	// <<< draw the path contour <<<


	// >>> draw the points >>>
	i.toFirst();	// reset iterator
	prev_seg = 0L;	// reset previous segment
	painter.setBrush( Qt::NoBrush );

	for ( ; i.current(); ++i )
	{
		// draw boxes:
		painter.setPen( Qt::black );
		const uint handleSize = 3;

		if ( i.current()->firstPoint( prev_seg ) )
			painter.drawRect(
				i.current()->firstPoint( prev_seg )->
					getQPoint( zoomFactor ).x() - handleSize,
				i.current()->firstPoint( prev_seg )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );
		if ( i.current()->firstCtrlPoint( prev_seg ) )
			painter.drawRect(
				i.current()->firstCtrlPoint( prev_seg )->
					getQPoint( zoomFactor ).x() - handleSize,
				i.current()->firstCtrlPoint( prev_seg )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );
		if ( i.current()->lastCtrlPoint( prev_seg ) )
			painter.drawRect(
				i.current()->lastCtrlPoint( prev_seg )->
					getQPoint( zoomFactor ).x() - handleSize,
				i.current()->lastCtrlPoint( prev_seg )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );
		if ( i.current()->lastPoint( prev_seg ) )
			painter.drawRect(
				i.current()->lastPoint( prev_seg )->
					getQPoint( zoomFactor ).x() - handleSize,
				i.current()->lastPoint( prev_seg )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );

		// draw control-lines of beziers:
		painter.setPen( QPen( Qt::black, 1, Qt::DotLine ) );

		if ( i.current()->firstCtrlPoint( prev_seg ) )
			painter.drawLine(
				i.current()->firstPoint( prev_seg )->getQPoint( zoomFactor ),
				i.current()->firstCtrlPoint( prev_seg )->getQPoint( zoomFactor ) );

		if ( i.current()->lastCtrlPoint( prev_seg ) )
			painter.drawLine(
				i.current()->lastCtrlPoint( prev_seg )->getQPoint( zoomFactor ),
				i.current()->lastPoint( prev_seg )->getQPoint( zoomFactor ) );

		prev_seg = i.current();	// remember previous segment
	}

	// <<< draw the points <<<

	painter.restore();
}

const VPoint*
VPath::currentPoint() const
{
	return( m_segments.getLast()->lastPoint() );
}

VPath&
VPath::moveTo( const double x, const double y )
{
// TODO: what is the exact postscript-behaviour?
	if ( isClosed() ) return *this;

	m_segments.getLast()->movePointTo(
		m_segments.getLast()->lastPoint(),
		x, y );

	return *this;
}

VPath&
VPath::lineTo( const double x, const double y )
{
	if ( isClosed() ) return *this;

	m_segments.append( new VLine( x, y ) );
	return *this;
}

VPath&
VPath::curveTo( const double x1, const double y1,
	const double x2, const double y2, const double x3, const double y3 )
{
	if ( isClosed() ) return *this;

	m_segments.append( new VCurve( x1, y1, x2, y2, x3, y3 ) );
	return *this;
}

VPath&
VPath::curve1To( const double x2, const double y2,
	const double x3, const double y3 )
{
	if ( isClosed() ) return *this;

	m_segments.append( new VCurve1( x2, y2, x3, y3 ) );
	return *this;
}

VPath&
VPath::curve2To( const double x1, const double y1,
	const double x3, const double y3 )
{
	if ( isClosed() ) return *this;

	m_segments.append( new VCurve( x1, y1, x3, y3 ) );
	return *this;
}

VPath&
VPath::arcTo( const double x1, const double y1,
	const double x2, const double y2, const double r )
{
	// parts of this routine are inspired by GNU ghostscript

	if ( isClosed() ) return *this;

	// we need to calculate the tangent points. therefore calculate tangents
	// D10=P1P0 and D12=P1P2 first:
	double dx10 = m_segments.getLast()->lastPoint()->x() - x1;
	double dy10 = m_segments.getLast()->lastPoint()->y() - y1;
	double dx12 = x2 - x1;
	double dy12 = y2 - y1;

	// calculate distance squares:
	double dsq10 = dx10*dx10 + dy10*dy10;
	double dsq12 = dx12*dx12 + dy12*dy12;

	// we now calculate tan(a/2) where a is the angular between D10 and D12.
	// we take advantage of D10*D12=d10*d12*cos(a), |D10xD12|=d10*d12*sin(a)
	// and tan(a/2)=sin(a)/[1-cos(a)].
	double num   = dx10*dy12 - dy10*dx12;
	double denom = sqrt( dsq10*dsq12 ) - dx10*dx12 + dy10*dy12;

	if ( denom == 0.0 )			// points are co-linear
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

		// if (bx0,by0) deviates from current point, add a line to it:
// TODO: decide via radius<XXX or sthg?
		if (
			bx0 != m_segments.getLast()->lastPoint()->x() ||
			by0 != m_segments.getLast()->lastPoint()->y() )
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

// TODO: make this nicer? check the exact meaning of this formula

		if ( distsq >= rsq * 1.0e8 ) // r is very small
			fract = 0.0; // dist==r==0
		else
			fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ) );

		double bx1 = bx0 + (x1 - bx0) * fract;
		double by1 = by0 + (y1 - by0) * fract;
		double bx2 = bx3 + (x1 - bx3) * fract;
		double by2 = by3 + (y1 - by3) * fract;

		// finally add the bezier-segment:
		m_segments.append( new VCurve( bx1, by1, bx2, by2, bx3, by3 ) );
	}
	return *this;
}

VPath&
VPath::close()
{
	if ( isClosed() ) return *this;

	// draw a line if last point differs from first point:
	if (
		*( m_segments.getFirst()->firstPoint( 0L ) ) !=
		*( m_segments.getLast()->lastPoint() ) )
	{
		lineTo(
			m_segments.getFirst()->firstPoint( 0L )->x(),
			m_segments.getFirst()->firstPoint( 0L )->y() );
	}

	m_isClosed = true;

	return *this;
}

VObject&
VPath::transform( const VAffineMap& affMap )
{
	QListIterator<VSegment> i( m_segments );
	for ( ; i.current() ; ++i )
		i.current()->transform( affMap );

	return *this;
}

QPointArray
VPath::getQPointArray( const double zoomFactor = 1.0 ) const
{
	QListIterator<VSegment> i( m_segments );
	const VSegment* prev_seg( 0L );	// pointer to previous segment
	QPointArray qpa;

	// skip first point when path is closed:
	if ( isClosed() )
	{
		prev_seg = i.current();
		++i;
	}

	for ( ; i.current(); ++i )
	{
		const QPointArray& seg_qpa =
			i.current()->getQPointArray( prev_seg, zoomFactor);

		uint old_size( qpa.size() );
		uint add_size( seg_qpa.size() );

		qpa.resize( old_size + add_size );

		for ( uint j = 0; j < add_size; ++j )
			qpa.setPoint( old_size + j, seg_qpa.point( j ) );

		prev_seg = i.current();	// remember previous segment
	}

	return qpa;
}
