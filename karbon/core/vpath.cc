/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>
#include <koPoint.h>
#include <qmap.h>
#include <qpainter.h>
#include <qptrvector.h>
#include <qvaluelist.h>
#include <qwmatrix.h>

#include "vglobal.h"
#include "vpath.h"

#include <kdebug.h>
// TODO:

// calculate distance of point c from line L0L1:
static double
height( const KoPoint* l0, const KoPoint* l1, const KoPoint* c )
{
	// calculate determinant of L0C and LOL1 to obtain projection of vector L0C to 
	// the orthogonal vector of L0L1:
	double det =
		c->x()  * l0->y() + l1->x() * c->y()  - c->x()  * l1->y() -
		l0->x() * c->y()  + l0->x() * l1->y() - l1->x() * l0->y();
	// calculate norm = length(L0L1):
	double norm = sqrt(
		( l1->x() - l0->x() ) * ( l1->x() - l0->x() ) +
		( l1->y() - l0->y() ) * ( l1->y() - l0->y() ) );

	// if norm is very small, simply return distance L0C:
	if( norm < 1.0e-6 )
		return( sqrt(
			( c->x() - l0->x() ) * ( c->x() - l0->x() ) +
			( c->y() - l0->y() ) * ( c->y() - l0->y() ) ) );

	// normalize:
	return ( det / norm );
}

struct VRect
{
	VRect( bool valid = true ) : m_isValid( valid ) {}

	KoPoint m_tl;
	KoPoint m_br;
	bool m_isValid;
};

// -----------------------------------
// -----------------------------------

class VSegment
{
public:
	VSegment( const KoPoint& p3, VSegment* prev )
		: m_p3( p3 ), m_prev( prev ) {}
	virtual ~VSegment() {}

	virtual VSegment* clone() const = 0;

	const KoPoint* p0() const
	{
		if( m_prev != 0L )
			return m_prev->p3();
		else
			return 0L;
	}
	virtual const KoPoint* p1() const
		{ return 0L; }
	virtual const KoPoint* p2() const
		{ return 0L; }
	const KoPoint* p3() const
		{ return &m_p3; }

	virtual void setP1( const KoPoint& /*p*/ ) {}
	virtual void setP2( const KoPoint& /*p*/ ) {}
	void setP3( const KoPoint& p ) { m_p3 = p; };

	VSegment* previous() const { return m_prev; }
	void setPrevious( const VSegment* prev )
		{ m_prev = const_cast<VSegment*>( prev ); }

	virtual bool isFlat() const = 0;
	virtual VRect boundingBox() const
		{ return VRect( false ); }
	virtual bool bbsIntersect( const VSegment* seg ) const;
	virtual bool intersectsWithBB( const VRect& rect ) const;
	virtual void addYourselfSplittedAtTo(
		const double t, VPath& path, bool left = true ) = 0;
	virtual QPtrVector<VSegment> splitAtMidpoint() const = 0;

	void findIntersectionParametersWith( const double t0, const double t1,
		const VSegment* segB, const double u0, const double u1,
		QValueList<double>& paramsA, QValueList<double>& paramsB ) const;

private:
	KoPoint m_p3;
	VSegment* m_prev;
};


class VMoveTo : public VSegment
{
public:
	VMoveTo( const KoPoint& p3, VSegment* prev = 0L )
		: VSegment( p3, prev ) {}

	virtual VMoveTo* clone() const;

	virtual bool isFlat() const { return true; }
	virtual bool bbsIntersect( const VSegment* ) const
		{ return false; }
	virtual bool intersectsWithBB( const VRect& ) const
		{ return false; }
	virtual void addYourselfSplittedAtTo(
		const double t, VPath& path, bool left = true );
	virtual QPtrVector<VSegment> splitAtMidpoint() const;
};


class VLineTo : public VSegment
{
public:
	VLineTo( const KoPoint& p3, VSegment* prev )
		: VSegment( p3, prev ) {}

	virtual VLineTo* clone() const;

	virtual bool isFlat() const { return true; }
	virtual VRect boundingBox() const;
	virtual void addYourselfSplittedAtTo(
		const double t, VPath& path, bool left = true );
	virtual QPtrVector<VSegment> splitAtMidpoint() const;
};


class VCurveTo : public VSegment
{
public:
	VCurveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3,
		const char fixedCtrl, VSegment* prev )
		: m_p1( p1 ), m_p2( p2 ), m_fixedCtrl( fixedCtrl ), VSegment( p3, prev ) {}

	virtual VCurveTo* clone() const;

	virtual const KoPoint* p1() const
		{ return m_fixedCtrl==1 ? p0() : &m_p1; }
	virtual const KoPoint* p2() const
		{ return m_fixedCtrl==2 ? p3() : &m_p2; }

	virtual void setP1( const KoPoint& p ) { if( m_fixedCtrl!=1) m_p1 = p; }
	virtual void setP2( const KoPoint& p ) { if( m_fixedCtrl!=2) m_p2 = p; }

	void setFixedCtrl( const char fixedCtrl ) { m_fixedCtrl = fixedCtrl; }

	virtual bool isFlat() const;
	virtual VRect boundingBox() const;
	virtual void addYourselfSplittedAtTo(
		const double t, VPath& path, bool left = true );
	virtual QPtrVector<VSegment> splitAtMidpoint() const;

private:
	KoPoint m_p1;
	KoPoint m_p2;
	char m_fixedCtrl;
};


class VClosePath : public VSegment
{
public:
	VClosePath( const KoPoint& p3, VSegment* prev = 0L )
		: VSegment( p3, prev ) {}

	virtual VClosePath* clone() const;

	virtual bool isFlat() const { return true; }
	virtual bool bbsIntersect( const VSegment* ) const
		{ return false; }
	virtual bool intersectsWithBB( const VRect& ) const
		{ return false; }
	virtual void addYourselfSplittedAtTo(
		const double t, VPath& path, bool left = true );
	virtual QPtrVector<VSegment> splitAtMidpoint() const;
};

bool
VSegment::bbsIntersect( const VSegment* seg ) const
{
	return seg->intersectsWithBB( boundingBox() );
}

bool
VSegment::intersectsWithBB( const VRect& rect ) const
{
	VRect r = boundingBox();

	if(
		r.m_tl.x() > rect.m_br.x() || r.m_tl.y() < rect.m_br.y() ||
		rect.m_tl.x() > r.m_br.x() || rect.m_tl.y() < r.m_br.y() )
	{
		return false;
	}

	return true;
}

void
VSegment::findIntersectionParametersWith( const double t0, const double t1,
	const VSegment* segB, const double u0, const double u1,
	QValueList<double>& paramsA, QValueList<double>& paramsB ) const
{
	if( p0() == 0L || segB->p0() == 0L )
		return;

// TODO: to save some stack, maybe move bb-test in front of all
// findIntersectionParametersWith() calls?

	// bounding-boxes dont intersect => game over:
	if ( !bbsIntersect( segB ) )
		return;

	if( isFlat() )
	{
		if( segB->isFlat() )
		{
			// calculate intersection of line segments:
			KoPoint a03  = *p3() - *p0();
			KoPoint b03  = *( segB->p3() ) - *( segB->p0() );
			double det = b03.x() * a03.y() - b03.y() * a03.x();

			if( 1.0 + det == 1.0 )
				return;
			else
			{
				KoPoint a0b0 = *( segB->p0() ) - *p0();
				const double one_det = 1.0 / det;

				double t = one_det * ( b03.x() * a0b0.y() - b03.y() * a0b0.x() );
				double u = one_det * ( a03.x() * a0b0.y() - a03.y() * a0b0.x() );

				if ( t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0 )
					return;

				paramsA.append( t0 + t * ( t1 - t0 ) );
				paramsB.append( u0 + u * ( u1 - u0 ) );
			}
		}
		else
		{
			double midU = 0.5 * ( u0 + u1 );
			QPtrVector<VSegment> newSegsB = segB->splitAtMidpoint();

			newSegsB.at( 0 )->findIntersectionParametersWith(
				u0, midU, this, t0, t1, paramsB, paramsA );
			newSegsB.at( 1 )->findIntersectionParametersWith(
				midU, u1, this, t0, t1, paramsB, paramsA );
		}
	}
	else
	{
		double midT = 0.5 * ( t0 + t1 );
		QPtrVector<VSegment> newSegsA = splitAtMidpoint();

		if( segB->isFlat() )
		{
			newSegsA.at( 0 )->findIntersectionParametersWith(
				t0, midT, segB, u0, u1, paramsA, paramsB );
			newSegsA.at( 1 )->findIntersectionParametersWith(
				midT, t1, segB, u0, u1, paramsA, paramsB );
		}
		else
		{
			double midU = 0.5 * ( u0 + u1 );
			QPtrVector<VSegment> newSegsB = segB->splitAtMidpoint();

			newSegsA.at( 0 )->findIntersectionParametersWith(
				t0, midT, newSegsB.at( 0 ), u0, midU, paramsA, paramsB );
			newSegsA.at( 1 )->findIntersectionParametersWith(
				midT, t1, newSegsB.at( 0 ), u0, midU, paramsA, paramsB );
			newSegsA.at( 0 )->findIntersectionParametersWith(
				t0, midT, newSegsB.at( 1 ), midU, u1, paramsA, paramsB );
			newSegsA.at( 1 )->findIntersectionParametersWith(
				midT, t1, newSegsB.at( 1 ), midU, u1, paramsA, paramsB );
		}
	}
}


VMoveTo*
VMoveTo::clone() const
{
	return new VMoveTo( *p3(), previous() );
}

void
VMoveTo::addYourselfSplittedAtTo( const double /*t*/, VPath& path, bool left )
{
	if ( !left )
		path.moveTo( *p3() );

	setPrevious( path.lastSegment() );
}

QPtrVector<VSegment>
VMoveTo::splitAtMidpoint() const
{
// TODO: unecessary
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint l_p3 = 0.5 * ( *p0() + *p3() );

	VMoveTo* curve1 = new VMoveTo( l_p3, previous() );
	VMoveTo* curve2 = new VMoveTo( *p3(), curve1 );

	segments.insert( 0, curve1 );
	segments.insert( 1, curve2 );

	return segments;
}


VLineTo*
VLineTo::clone() const
{
	return new VLineTo( *p3(), previous() );
}

VRect
VLineTo::boundingBox() const
{
	VRect rect;

	if ( p0() == 0L )
		return rect;

	double top, left;
	double bottom, right;

	if( p0()->x() > p3()->x() )
		{ left = p3()->x(); right = p0()->x(); }
	else
		{ left = p0()->x(); right = p3()->x(); }
	if( p0()->y() > p3()->y() )
		{ bottom = p3()->y(); top = p0()->y(); }
	else
		{ bottom = p0()->y(); top = p3()->y(); }

	rect.m_tl.setCoords( left, top );
	rect.m_br.setCoords( right, bottom );

	return rect;
}

void
VLineTo::addYourselfSplittedAtTo( const double t, VPath& path, bool left )
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;

	if ( left )
		path.lineTo( *p0() + t * ( *p3() - *p0() ) );
	else
		path.lineTo( *p3() );

	setPrevious( path.lastSegment() );
}

QPtrVector<VSegment>
VLineTo::splitAtMidpoint() const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint l_p3 = 0.5 * ( *p0() + *p3() );

	VLineTo* curve1 = new VLineTo( l_p3, previous() );
	VLineTo* curve2 = new VLineTo( *p3(), curve1 );

	segments.insert( 0, curve1 );
	segments.insert( 1, curve2 );

	return segments;
}


VCurveTo*
VCurveTo::clone() const
{
	return new VCurveTo( m_p1, m_p2, *p3(), m_fixedCtrl, previous() );
}

bool
VCurveTo::isFlat() const
{
	if(
		QABS( height( p0(), p3(), p1() ) ) > VGlobal::bezierFlatnessTolerance ||
		QABS( height( p0(), p3(), p2() ) ) > VGlobal::bezierFlatnessTolerance )
	{
		return false;
	}

	return true;
}

VRect
VCurveTo::boundingBox() const
{
	VRect rect;

	if ( p0() == 0L )
		return rect;

	double top, left;
	double bottom, right;

	if( p0()->x() > p3()->x() )
		{ left = p3()->x(); right = p0()->x(); }
	else
		{ left = p0()->x(); right = p3()->x(); }
	if( p0()->y() > p3()->y() )
		{ bottom = p3()->y(); top = p0()->y(); }
	else
		{ bottom = p0()->y(); top = p3()->y(); }

	if( p1()->x() < left )
		left = p1()->x();
	else if( p1()->x() > right )
		right = p1()->x();
	if( p1()->y() < bottom )
		bottom = p1()->y();
	else if( p1()->y() > top )
		top = p1()->y();

	if( p2()->x() < left )
		left = p2()->x();
	else if( p2()->x() > right )
		right = p2()->x();
	if( p2()->y() < bottom )
		bottom = p2()->y();
	else if( p2()->y() > top )
		top = p2()->y();

	rect.m_tl.setCoords( left, top );
	rect.m_br.setCoords( right, bottom );

	return rect;
}

void
VCurveTo::addYourselfSplittedAtTo(
	const double t, VPath& path, bool left )
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;

	if ( left )
	{
		KoPoint q1;
		KoPoint q2;
		KoPoint q3;

		q1   = *p0() + t * ( *p1() - *p0() );
		q2   = *p1() + t * ( *p2() - *p1() );
		m_p2 = *p2() + t * ( *p3() - *p2() );
		m_p1 =  q2   + t * ( *p2() - q2 );
		q2   =  q1   + t * (  q2   - q1 );
		q3   =  q2   + t * ( *p1() - q2 );

		if( m_fixedCtrl==1 )
		{
			path.curve1To( q2, q3 );
			m_fixedCtrl = 0;
		}
		else
			path.curveTo( q1, q2, q3 );
	}
	else
	{
		if( m_fixedCtrl==2 )
			path.curve2To( *p1(), *p3() );
		else
			path.curveTo( *p1(), *p2(), *p3() );
	}

	setPrevious( path.lastSegment() );
}

QPtrVector<VSegment>
VCurveTo::splitAtMidpoint() const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint l_p1 = 0.5 * ( *p0() + *p1() );
	KoPoint r_p1 = 0.5 * ( *p1() + *p2() );	// temporary storage
	KoPoint r_p2 = 0.5 * ( *p2() + *p3() );
	KoPoint l_p2 = 0.5 * ( l_p1 + r_p1 );
	r_p1 = 0.5 * ( r_p1 + r_p2 );
	KoPoint l_p3 = 0.5 * ( l_p2 + r_p1 );

	VCurveTo* curve1 = new VCurveTo(
		l_p1, l_p2, l_p3,
		m_fixedCtrl==1 ? 1 : 0,
		previous() );
	VCurveTo* curve2 = new VCurveTo(
		r_p1, r_p2, *p3(),
		m_fixedCtrl==2 ? 2 : 0,
		curve1 );

	segments.insert( 0, curve1 );
	segments.insert( 1, curve2 );

	return segments;
}


VClosePath*
VClosePath::clone() const
{
	return new VClosePath( *p3(), previous() );
}

void
VClosePath::addYourselfSplittedAtTo(
	const double /*t*/, VPath& path, bool left )
{
	if ( !left )
		path.close();

	setPrevious( path.lastSegment() );
}

QPtrVector<VSegment>
VClosePath::splitAtMidpoint() const
{
// TODO: unecessary
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint l_p3 = 0.5 * ( *p0() + *p3() );

	VClosePath* curve1 = new VClosePath( l_p3, previous() );
	VClosePath* curve2 = new VClosePath( *p3(), curve1 );

	segments.insert( 0, curve1 );
	segments.insert( 1, curve2 );

	return segments;
}

// -----------------------------------
// -----------------------------------


VPath::VPath()
	: VObject()
{
	// we need a current point at (0,0):
	m_segments.append( new VMoveTo( KoPoint( 0.0, 0.0 ) ) );
}

VPath::VPath( const VPath& /*path*/ )
	: VObject()
{
// TODO: implement copy-ctor
}

VPath::~VPath()
{
	QPtrListIterator<VSegment> itr( m_segments );
	for( ; itr.current() ; ++itr )
		delete( itr.current() );
}

void
VPath::draw( QPainter& painter, const QRect& rect,
	const double zoomFactor )
{
	if( state() == deleted )
		return;

	if( !rect.intersects( boundingBox() ) )
		return;

	painter.save();

	if( state() == edit )
	{
		painter.setPen( Qt::yellow );
		painter.setRasterOp( Qt::XorROP );
	}

	QPtrListIterator<VSegment> itr( m_segments );

	for( ; itr.current(); ++itr )
	{
		if( VClosePath* seg = dynamic_cast<VClosePath*>( itr.current() ) )
			painter.moveTo(
				qRound( zoomFactor * seg->p3()->x() ),
				qRound( zoomFactor * seg->p3()->y() ) );
		else
		if( VMoveTo* seg = dynamic_cast<VMoveTo*>( itr.current() ) )
			painter.moveTo(
				qRound( zoomFactor * seg->p3()->x() ),
				qRound( zoomFactor * seg->p3()->y() ) );
		else
		if( VLineTo* seg = dynamic_cast<VLineTo*>( itr.current() ) )
			painter.lineTo(
				qRound( zoomFactor * seg->p3()->x() ),
				qRound( zoomFactor * seg->p3()->y() ) );
		else
		if( VCurveTo* seg = dynamic_cast<VCurveTo*>( itr.current() ) )
		{
			if( seg->p0() )
			{
				QPointArray seg_qpa( 4 );
				seg_qpa.setPoint( 0,
					qRound( zoomFactor * seg->p0()->x() ),
					qRound( zoomFactor * seg->p0()->y() ) );
				seg_qpa.setPoint( 1,
					qRound( zoomFactor * seg->p1()->x() ),
					qRound( zoomFactor * seg->p1()->y() ) );
				seg_qpa.setPoint( 2,
					qRound( zoomFactor * seg->p2()->x() ),
					qRound( zoomFactor * seg->p2()->y() ) );
				seg_qpa.setPoint( 3,
					qRound( zoomFactor * seg->p3()->x() ),
					qRound( zoomFactor * seg->p3()->y() ) );

				painter.drawPolyline( seg_qpa.cubicBezier() );
			}
			else
				painter.lineTo(
					qRound( zoomFactor * seg->p3()->x() ),
					qRound( zoomFactor * seg->p3()->y() ) );

			painter.moveTo(
				qRound( zoomFactor * seg->p3()->x() ),
				qRound( zoomFactor * seg->p3()->y() ) );
		}
	}

	if( state() == selected )
	{
		// draw small boxes for path nodes
		for( itr.toFirst(); itr.current(); ++itr )
		{
			// draw boxes:
			painter.setPen( Qt::NoPen );
			painter.setBrush( Qt::blue.light() );
			drawBox( painter,
				qRound( zoomFactor * itr.current()->p3()->x() ),
				qRound( zoomFactor * itr.current()->p3()->y() ), 3 );
		}

	}
	if( state() == edit )
	{
		painter.setBrush( Qt::NoBrush );

		for( itr.toFirst(); itr.current(); ++itr )
		{
			// draw boxes:
			painter.setPen( Qt::black );
			drawBox( painter,
				qRound( zoomFactor * itr.current()->p3()->x() ),
				qRound( zoomFactor * itr.current()->p3()->y() ) );
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
	return *( m_segments.getLast()->p3() );
}

VPath&
VPath::moveTo( const double& x, const double& y )
{
	if( isClosed() ) return *this;

	if( dynamic_cast<VMoveTo*>( m_segments.getLast() ) )
	{
		m_segments.getLast()->setP3( KoPoint( x, y ) );
	}
	else
		m_segments.append( new VMoveTo( KoPoint( x, y ), m_segments.getLast() ) );

	return *this;
}

VPath&
VPath::lineTo( const double& x, const double& y )
{
	if( isClosed() ) return *this;

	m_segments.append( new VLineTo( KoPoint( x, y ), m_segments.getLast() ) );

	return *this;
}

VPath&
VPath::curveTo(
	const double& x1, const double& y1,
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append(
		new VCurveTo(
			KoPoint( x1, y1 ),
			KoPoint( x2, y2 ),
			KoPoint( x3, y3 ),
			0,
			m_segments.getLast() ) );

	return *this;
}

VPath&
VPath::curve1To(
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append(
		new VCurveTo(
			KoPoint( 0.0, 0.0 ),
			KoPoint( x2, y2 ),
			KoPoint( x3, y3 ),
			1,
			m_segments.getLast() ) );

	return *this;
}

VPath&
VPath::curve2To(
	const double& x1, const double& y1,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append(
		new VCurveTo(
			KoPoint( x1, y1 ),
			KoPoint( 0.0, 0.0 ),
			KoPoint( x3, y3 ),
			2,
			m_segments.getLast() ) );

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
	double dx10 = m_segments.getLast()->p3()->x() - x1;
	double dy10 = m_segments.getLast()->p3()->y() - y1;
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
			bx0 != m_segments.getLast()->p3()->x() ||
			by0 != m_segments.getLast()->p3()->y() )
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
	if( isClosed() ) return *this;

	// add a lineTo if necessary:
	if( currentPoint() != *( m_segments.getFirst()->p3() ) )
		lineTo( *( m_segments.getFirst()->p3() ) );

	m_segments.append(
		new VClosePath(
			*( m_segments.getFirst()->p3() ), m_segments.getLast() ) );

	return *this;
}


bool
VPath::isClosed() const
{
	return dynamic_cast<VClosePath*>( m_segments.getLast() );
}

VPath*
VPath::revert() const
{
	return 0L;
}

VPath*
VPath::booleanOp( const VPath* path, int /*type*/ ) const
{
	// step one: find intersections.

	// list of intersection-parameters:
	typedef QValueList<double> VParamList;
	VParamList paramsA;
	QMap<VSegment*,VParamList> paramsB;

	// the new intersected paths:
	VPath* pathA = new VPath();
	VPath* pathB = new VPath();

	// segment copies. we stay const while "modifying" the segments
	// for easiest book-keeping:
	VSegment* copyA;
	VSegment* copyB;

	QPtrListIterator<VSegment> itrA( m_segments );
	QPtrListIterator<VSegment> itrB( path->m_segments );

	QValueList<double>::iterator paramItr;
	double prevParam;

	for( ; itrA.current(); ++itrA )
	{
		paramsA.clear();

		for( itrB.toFirst(); itrB.current(); ++itrB )
		{
			itrA.current()->findIntersectionParametersWith(
				0.0, 1.0, *itrB, 0.0, 1.0, paramsA, paramsB[*itrB] );
		}

		qHeapSort( paramsA );
		copyA = itrA.current()->clone();
		prevParam = 0.0;

		for( paramItr = paramsA.begin(); paramItr != paramsA.end(); ++paramItr )
		{
			copyA->addYourselfSplittedAtTo(
				( *paramItr - prevParam )/( 1 - prevParam ), *pathA );
			prevParam = *paramItr;
		}
		copyA->addYourselfSplittedAtTo( prevParam, *pathA, false );

		delete copyA;
	}

	for( itrB.toFirst(); itrB.current(); ++itrB )
	{
		qHeapSort( paramsB[*itrB] );
		copyB = itrB.current()->clone();
		prevParam = 0.0;

		for( paramItr = paramsB[*itrB].begin(); paramItr != paramsB[*itrB].end();
			++paramItr )
		{
			copyB->addYourselfSplittedAtTo(
				( *paramItr - prevParam )/( 1 - prevParam ), *pathB );
			prevParam = *paramItr;
		}
		copyB->addYourselfSplittedAtTo( prevParam, *pathB, false );

		delete copyB;
	}

// TODO
return pathA;
}

VObject&
VPath::transform( const QWMatrix& m )
{
	QPtrListIterator<VSegment> itr( m_segments );
	for( ; itr.current() ; ++itr )
	{
		if( itr.current()->p1() )
			itr.current()->setP1( itr.current()->p1()->transform( m ) );
		if( itr.current()->p2() )
			itr.current()->setP2( itr.current()->p2()->transform( m ) );

		itr.current()->setP3( itr.current()->p3()->transform( m ) );
	}

	return *this;
}

QRect
VPath::boundingBox() const
{
	QRect rect;
	VRect r;
	bool notSet = true;

	double top, left;
	double bottom, right;

	QPtrListIterator<VSegment> itr( m_segments );

	for( ; itr.current(); ++itr )
	{
		r = itr.current()->boundingBox();

		if( !r.m_isValid )
			continue;

		if( notSet )
		{
			left = r.m_tl.x();
			right = r.m_br.x();
			bottom = r.m_br.y();
			top = r.m_tl.y();
			notSet = false;
		}

		if( r.m_tl.x() < left )
			left = r.m_tl.x();
		if( r.m_br.x() > right )
			right = r.m_br.x();
		if( r.m_br.y() < bottom )
			bottom = r.m_br.y();
		if( r.m_tl.y() > top )
			top = r.m_tl.y();
	}
	rect.setCoords( left, bottom, right, top );

	return rect;
}

VObject *
VPath::clone()
{
	return new VPath( *this );
}

