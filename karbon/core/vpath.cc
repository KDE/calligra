/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>
#include <koPoint.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include "vpath.h"

#include <kdebug.h>

// TODO:
// - make sure that lastpoint==currenpoint doesnt get removed
// - set m_isDirty everywhere
// - reimplement cubicBezier() ?


struct VSegment
{
	VSegment( const double& x3, const double& y3 )
		: m_p3( x3, y3 ) {}

	enum
	{
		moveTo,
		lineTo,
		curveTo,
		curve1To,
		curve2To,
		closePath
	} m_type;

	KoPoint m_p3;
};

struct VMoveTo : VSegment
{
	VMoveTo( const double& x3, const double& y3 )
		: VSegment( x3, y3 ) { m_type = moveTo; }
};

struct VLineTo : VSegment
{
	VLineTo( const double& x3, const double& y3 )
		: VSegment( x3, y3 ) { m_type = lineTo; }
};

struct VCurveTo : VSegment
{
	VCurveTo(
		const double& x1, const double& y1,
		const double& x2, const double& y2,
		const double& x3, const double& y3 )
		: m_p1( x1, x1 ), m_p2( x2, y2 ), VSegment( x3, y3 ) { m_type = curveTo; }

	KoPoint m_p1;
	KoPoint m_p2;
};

struct VCurve1To : VSegment
{
	VCurve1To(
		const double& x2, const double& y2,
		const double& x3, const double& y3 )
		: m_p2( x2, y2 ), VSegment( x3, y3 ) { m_type = curve1To; }

	KoPoint m_p2;
};

struct VCurve2To : VSegment
{
	VCurve2To(
		const double& x1, const double& y1,
		const double& x3, const double& y3 )
		: m_p1( x1, x1 ), VSegment( x3, y3 ) { m_type = curve2To; }

	KoPoint m_p1;
};

struct VClosePath : VSegment
{
	VClosePath( const double& x3, const double& y3 )
		: VSegment( x3, y3 ) { m_type = closePath; }
};

// -----------------------------------
// -----------------------------------

// fundamental segment-intersection algorithms (line/line, line/bezier,
// bezier/bezier):

// line/line-intersections:
// returns number of intersections. if intersections are found, they are added
// to a/b.
static int
intersect_line_line(
	const KoPoint& a0, const KoPoint& a3, const KoPoint& b0, const KoPoint& b3,
	QPtrList<VSegment>& a, QPtrList<VSegment>& b )
{
	double minax( QMIN( a0.x(), a3.x() ) );
	double maxax( QMAX( a0.x(), a3.x() ) );
	double minay( QMIN( a0.y(), a3.y() ) );
	double maxay( QMAX( a0.y(), a3.y() ) );
	double minbx( QMIN( b0.x(), b3.x() ) );
	double maxbx( QMAX( b0.x(), b3.x() ) );
	double minby( QMIN( b0.y(), b3.y() ) );
	double maxby( QMAX( b0.y(), b3.y() ) );

	// if bounding boxes do not intersect, lines do not either:
	if ( minax > maxbx || minbx > maxax || minay > maxby || minby > maxay )
	{
		return 0;
	}
// TODO
	return 0;
}

// bezier/bezier-intersections:
static int
intersect_bezier_bezier(
	const KoPoint& a0, const KoPoint& a1, const KoPoint& a2, const KoPoint& a3,
	const KoPoint& b0, const KoPoint& b1, const KoPoint& b2, const KoPoint& b3,
	QPtrList<VSegment>& a, QPtrList<VSegment>& b )
{
	double minax( QMIN( a0.x(), a3.x() ) );
	double maxax( QMAX( a0.x(), a3.x() ) );
	double minay( QMIN( a0.y(), a3.y() ) );
	double maxay( QMAX( a0.y(), a3.y() ) );
	double minbx( QMIN( b0.x(), b3.x() ) );
	double maxbx( QMAX( b0.x(), b3.x() ) );
	double minby( QMIN( b0.y(), b3.y() ) );
	double maxby( QMAX( b0.y(), b3.y() ) );

	// if bounding boxes do not intersect, beziers do not either:
	if ( minax > maxbx || minbx > maxax || minay > maxby || minby > maxay )
	{
		return 0;
	}

// TODO
	return 0;
}

// -----------------------------------
// -----------------------------------

VPath::VPath()
	: VObject()
{
	// we need a current point at (0,0):
	m_segments.append( new VMoveTo( 0.0, 0.0 ) );
}

VPath::VPath( const VPath& path )
	: VObject()
{
// TODO: implement copy-ctor
}

VPath::~VPath()
{
// TODO: should we be polite and destruct the QPtrLists as well ?
	QPtrListIterator<VSegment> itr( m_segments );
	for( ; itr.current() ; ++itr )
		delete( itr.current() );
}

void
VPath::draw( QPainter& painter, const QRect& rect, const double zoomFactor )
{
	if( isDeleted() ) return;

	painter.save();

	// >>> draw the path contour >>>
	QPtrListIterator<VSegment> itr( m_segments );
	QPointArray qpa;

	// skip first point when path is closed:
	if( isClosed() )
		++itr;

	for( ; itr.current(); ++itr )
	{
		QPointArray seg_qpa;

		switch( itr.current()->m_type )
		{
			case VSegment::moveTo:
			case VSegment::lineTo:
			case VSegment::closePath:
				seg_qpa.resize( 1 );
				seg_qpa.setPoint( 0, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
			case VSegment::curveTo:
				seg_qpa.resize( 4 );
				seg_qpa.setPoint( 0, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 1,
					static_cast<VCurveTo*>( itr.current() )->m_p1.x(),
					static_cast<VCurveTo*>( itr.current() )->m_p1.y() );
				seg_qpa.setPoint( 2,
					static_cast<VCurveTo*>( itr.current() )->m_p2.x(),
					static_cast<VCurveTo*>( itr.current() )->m_p2.y() );
				seg_qpa.setPoint( 3, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
			case VSegment::curve1To:
				seg_qpa.resize( 4 );
				seg_qpa.setPoint( 0, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 1, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 2,
					static_cast<VCurve1To*>( itr.current() )->m_p2.x(),
					static_cast<VCurve1To*>( itr.current() )->m_p2.y() );
				seg_qpa.setPoint( 3, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
			case VSegment::curve2To:
				seg_qpa.resize( 4 );
				seg_qpa.setPoint( 0, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 1,
					static_cast<VCurve2To*>( itr.current() )->m_p1.x(),
					static_cast<VCurve2To*>( itr.current() )->m_p1.y() );
				seg_qpa.setPoint( 2, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
				seg_qpa.setPoint( 3, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
		}


		uint old_size( qpa.size() );
		uint add_size( seg_qpa.size() );

		qpa.resize( old_size + add_size );

		for( uint j = 0; j < add_size; ++j )
			qpa.setPoint( old_size + j, seg_qpa.point( j ) );
	}

// TODO: remove hardcoded values:
	painter.setPen( Qt::black );
	painter.setBrush( QColor( 205, 205, 205 ) );

// TODO: filling not-closed shapes?
	// draw open or closed path ?
	if( isClosed() )
		painter.drawPolygon( qpa );
	else
		painter.drawPolyline( qpa );

	// <<< draw the path contour <<<


/*
	// >>> draw the points >>>
	itr.toFirst();	// reset iterator
	painter.setBrush( Qt::NoBrush );

	for( ; itr.current(); ++itr )
	{
		// draw boxes:
		painter.setPen( Qt::black );
		const uint handleSize = 3;

		if( itr.current()->firstPoint( itr - 1 ) )
			painter.drawRect(
				itr.current()->firstPoint( itr - 1 )->
					getQPoint( zoomFactor ).x() - handleSize,
				itr.current()->firstPoint( itr - 1 )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );
		if( itr.current()->firstCtrlPoint( itr - 1 ) )
			painter.drawRect(
				itr.current()->firstCtrlPoint( itr - 1 )->
					getQPoint( zoomFactor ).x() - handleSize,
				itr.current()->firstCtrlPoint( itr - 1 )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );
		if( itr.current()->lastCtrlPoint( itr - 1 ) )
			painter.drawRect(
				itr.current()->lastCtrlPoint( itr - 1 )->
					getQPoint( zoomFactor ).x() - handleSize,
				itr.current()->lastCtrlPoint( itr - 1 )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );
		if( itr.current()->lastPoint( itr - 1 ) )
			painter.drawRect(
				itr.current()->lastPoint( itr - 1 )->
					getQPoint( zoomFactor ).x() - handleSize,
				itr.current()->lastPoint( itr - 1 )->
					getQPoint( zoomFactor ).y() - handleSize,
				handleSize*2 + 1,
				handleSize*2 + 1 );

		// draw control-lines of beziers:
		painter.setPen( QPen( Qt::black, 1, Qt::DotLine ) );

		if( itr.current()->firstCtrlPoint( itr - 1 ) )
			painter.drawLine(
				itr.current()->firstPoint( itr - 1 )->getQPoint( zoomFactor ),
				itr.current()->firstCtrlPoint( itr - 1 )->getQPoint( zoomFactor ) );

		if( itr.current()->lastCtrlPoint( itr - 1 ) )
			painter.drawLine(
				itr.current()->lastCtrlPoint( itr - 1 )->getQPoint( zoomFactor ),
				itr.current()->lastPoint( itr - 1 )->getQPoint( zoomFactor ) );
	}

	// <<< draw the points <<<
*/
	painter.restore();
}

const KoPoint&
VPath::currentPoint() const
{
	return( m_segments.getLast()->m_p3 );
}

VPath&
VPath::moveTo( const double& x, const double& y )
{
	if( isClosed() ) return *this;

	m_segments.append( new VMoveTo( x, y ) );

	return *this;
}

VPath&
VPath::lineTo( const double& x, const double& y )
{
	if( isClosed() ) return *this;

	m_segments.append( new VLineTo( x, y ) );

	return *this;
}

VPath&
VPath::curveTo(
	const double& x1, const double& y1,
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append( new VCurveTo( x1, y1, x2, y2, x3, y3 ) );

	return *this;
}

VPath&
VPath::curve1To(
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append( new VCurve1To( x2, y2, x3, y3 ) );

	return *this;
}

VPath&
VPath::curve2To(
	const double& x1, const double& y1,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append( new VCurve2To( x1, y1, x3, y3 ) );

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
	double dx10 = m_segments.getLast()->m_p3.x() - x1;
	double dy10 = m_segments.getLast()->m_p3.y() - y1;
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

	if( denom == 0.0 )			// points are co-linear
		lineTo( x1, y1 );	// just add a line to first point
    else
    {
		// calculate distances from P1 to tangent points:
		double dist = fabs( r*num / denom );
		double d1t0 = dist / sqrt(dsq10);
		double d1t1 = dist / sqrt(dsq12);

// TODO: check for(r<0

		double bx0 = x1 + dx10*d1t0;
		double by0 = y1 + dy10*d1t0;

		// if(bx0,by0) deviates from current point, add a line to it:
// TODO: decide via radius<XXX or sthg?
		if(
			bx0 != m_segments.getLast()->m_p3.x() ||
			by0 != m_segments.getLast()->m_p3.y() )
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

		if( distsq >= rsq * 1.0e8 ) // r is very small
			fract = 0.0; // dist==r==0
		else
			fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ) );

		double bx1 = bx0 + (x1 - bx0) * fract;
		double by1 = by0 + (y1 - by0) * fract;
		double bx2 = bx3 + (x1 - bx3) * fract;
		double by2 = by3 + (y1 - by3) * fract;

		// finally add the bezier-segment:
		m_segments.append( new VCurveTo( bx1, by1, bx2, by2, bx3, by3 ) );
	}

	return *this;
}

VPath&
VPath::close()
{
	if( isClosed() ) return *this;

	m_segments.append(
		new VClosePath(
			m_segments.getFirst()->m_p3.x(),
			m_segments.getFirst()->m_p3.y() ) );

	return *this;
}


bool
VPath::isClosed() const
{
	return
		m_segments.getLast()->m_type == VSegment::closePath;
}

VPath*
VPath::revert() const
{
	return 0L;
}

VPath*
VPath::boolean( VPath& path, int type ) const
{
	return 0L;
}

VObject&
VPath::transform( const QWMatrix& m )
{
	QPtrListIterator<VSegment> itr( m_segments );
	for( ; itr.current() ; ++itr )
	{
		switch( itr.current()->m_type )
		{
			case VSegment::curveTo:
				static_cast<VCurveTo*>( itr.current() )->m_p1 =
					static_cast<VCurveTo*>( itr.current() )->m_p1.transform( m );
				static_cast<VCurveTo*>( itr.current() )->m_p2 =
					static_cast<VCurveTo*>( itr.current() )->m_p2.transform( m );
			break;
			case VSegment::curve1To:
				static_cast<VCurve1To*>( itr.current() )->m_p2 =
					static_cast<VCurve1To*>( itr.current() )->m_p2.transform( m );
			break;
			case VSegment::curve2To:
				static_cast<VCurve2To*>( itr.current() )->m_p1 =
					static_cast<VCurve2To*>( itr.current() )->m_p1.transform( m );
			break;
		}
		itr.current()->m_p3 = itr.current()->m_p3.transform( m );
	}

	return *this;
}

QPointArray
VPath::getQPointArray( const double zoomFactor ) const
{
	QPtrListIterator<VSegment> itr( m_segments );
	QPointArray qpa;

	// skip first point when path is closed:
	if( isClosed() )
		++itr;

	for( ; itr.current(); ++itr )
	{
		QPointArray seg_qpa;

		switch( itr.current()->m_type )
		{
			case VSegment::moveTo:
			case VSegment::lineTo:
			case VSegment::closePath:
				seg_qpa.resize( 1 );
				seg_qpa.setPoint( 0, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
			case VSegment::curveTo:
				seg_qpa.resize( 4 );
				seg_qpa.setPoint( 0, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 1,
					static_cast<VCurveTo*>( itr.current() )->m_p1.x(),
					static_cast<VCurveTo*>( itr.current() )->m_p1.y() );
				seg_qpa.setPoint( 2,
					static_cast<VCurveTo*>( itr.current() )->m_p2.x(),
					static_cast<VCurveTo*>( itr.current() )->m_p2.y() );
				seg_qpa.setPoint( 3, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
			case VSegment::curve1To:
				seg_qpa.resize( 4 );
				seg_qpa.setPoint( 0, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 1, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 2,
					static_cast<VCurve1To*>( itr.current() )->m_p2.x(),
					static_cast<VCurve1To*>( itr.current() )->m_p2.y() );
				seg_qpa.setPoint( 3, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
			case VSegment::curve2To:
				seg_qpa.resize( 4 );
				seg_qpa.setPoint( 0, ( itr - 1 )->m_p3.x(), ( itr - 1 )->m_p3.y() );
				seg_qpa.setPoint( 1,
					static_cast<VCurve2To*>( itr.current() )->m_p1.x(),
					static_cast<VCurve2To*>( itr.current() )->m_p1.y() );
				seg_qpa.setPoint( 2, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
				seg_qpa.setPoint( 3, itr.current()->m_p3.x(), itr.current()->m_p3.y() );
			break;
		}

		uint old_size( qpa.size() );
		uint add_size( seg_qpa.size() );

		qpa.resize( old_size + add_size );

		for( uint j = 0; j < add_size; ++j )
			qpa.setPoint( old_size + j, seg_qpa.point( j ) );
	}

	return qpa;
}
