/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <math.h>
#include <koPoint.h>
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

// -----------------------------------
// -----------------------------------

class VSegment
{
public:
	VSegment( const KoPoint& p3, VSegment* prev  )
		: m_p3( p3 ), m_prev( prev ) {}
	virtual ~VSegment() {}

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

	virtual void setP1( const KoPoint& p ) {}
	virtual void setP2( const KoPoint& p ) {}
	void setP3( const KoPoint& p ) { m_p3 = p; };

	VSegment* previous() const { return m_prev; }
	void setPrevious( VSegment* prev ) { m_prev = prev; }

	virtual bool isFlat() const;
	virtual void calcBoundingBox( KoPoint& min, KoPoint& max ) const;
	virtual void addYourselfLeftSplittedAtTo(
		const double t, VPath& path ) const = 0;
	virtual QPtrVector<VSegment> splitAtMidpoint() const = 0;

	void findIntersectionParametersWith( const double t0, const double t1,
		const VSegment* segB, const double u0, const double u1,
		QValueList<double>& paramsA, QValueList<double>& paramsB ) const;
	void addIntersectionsWithTo( const VSegment* b,
		VPath& pathA, VPath& pathB ) const;

private:
	KoPoint m_p3;
	VSegment* m_prev;
};

bool
VSegment::isFlat() const
{
	if(
		QABS( height( p0(), p3(), p1() ) ) > VGlobal::bezierFlatnessTolerance ||
		QABS( height( p0(), p3(), p2() ) ) > VGlobal::bezierFlatnessTolerance )
	{
		return false;
	}

	return true;
}

void
VSegment::calcBoundingBox( KoPoint& min, KoPoint& max ) const
{
	if ( m_prev == 0L )
		return;

	if( p0()->x() > p3()->x() )
		{ min.setX( p3()->x() ); max.setX( p0()->x() ); }
	else
		{ min.setX( p0()->x() ); max.setX( p3()->x() ); }
	if( p1()->x() < min.x() )
		min.setX( p1()->x() );
	else if( p1()->x() > max.x() )
		max.setX( p1()->x() );
	if( p2()->x() < min.x() )
		min.setX( p2()->x() );
	else if( p2()->x() > max.x() )
		max.setX( p2()->x() );

	if( p0()->y() > p3()->y() )
		{ min.setY( p3()->y() ); max.setY( p0()->y() ); }
	else
		{ min.setY( p0()->y() ); max.setY( p3()->y() ); }
	if( p1()->y() < min.y() )
		min.setY( p1()->y() );
	else if( p1()->y() > max.y() )
		max.setY( p1()->y() );
	if( p2()->y() < min.y() )
		min.setY( p2()->y() );
	else if( p2()->y() > max.y() )
		max.setY( p2()->y() );
}

void
VSegment::findIntersectionParametersWith( const double t0, const double t1,
	const VSegment* segB, const double u0, const double u1,
	QValueList<double>& paramsA, QValueList<double>& paramsB ) const
{
// TODO: to save some stack, maybe move bb-test in front of all
// findIntersectionParametersWith() calls?

	// calculate bounding-boxes:
	KoPoint minA;
	KoPoint maxA;
	calcBoundingBox( minA, maxA );

	KoPoint minB;
	KoPoint maxB;
	segB->calcBoundingBox( minB, maxB );

	// bounding-boxes dont intersect => game over:
	if (
		minA.x() > maxB.x() || minB.x() > maxA.x() ||
		minA.y() > maxB.y() || minB.y() > maxA.y() )
	{
		return;
	}

	if( isFlat() )
	{
		if( segB->isFlat() )
		{
			// calculate intersection of line segments:
			KoPoint a03  = *p3() - *p0();
			KoPoint b03  = *( segB->p3() ) - *( segB->p0() );
			double det = b03.x() * a03.y() - b03.y() * a03.x();
			
			if( det == 0.0 )
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
				u0, midU, this, t0, t1, paramsA, paramsB );
			newSegsB.at( 1 )->findIntersectionParametersWith(
				midU, u1, this, t0, t1, paramsA, paramsB );
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

void
VSegment::addIntersectionsWithTo( const VSegment* segB,
		VPath& pathA, VPath& pathB ) const
{
	// list of intersection-params:
	QValueList<double> paramsA;
	QValueList<double> paramsB;

	findIntersectionParametersWith( 0.0, 1.0, segB, 0.0, 1.0, paramsA, paramsB );

	qHeapSort( paramsA );
	qHeapSort( paramsB );
	
	QValueList<double>::iterator itr;
	for( itr = paramsA.begin(); itr != paramsA.end(); ++itr )
		addYourselfLeftSplittedAtTo( *itr, pathA );
	for( itr = paramsB.begin(); itr != paramsB.end(); ++itr )
		segB->addYourselfLeftSplittedAtTo( *itr, pathB );
}


class VMoveTo : public VSegment
{
public:
	VMoveTo( const KoPoint& p3, VSegment* prev = 0L )
		: VSegment( p3, prev ) {}

	virtual bool isFlat() const { return true; }
	virtual void addYourselfLeftSplittedAtTo( const double t, VPath& path ) const;
	virtual QPtrVector<VSegment> splitAtMidpoint() const;
};


class VLineTo : public VSegment
{
public:
	VLineTo( const KoPoint& p3, VSegment* prev )
		: VSegment( p3, prev ) {}

	virtual bool isFlat() const { return true; }
	virtual void calcBoundingBox( KoPoint& min, KoPoint& max ) const;
	virtual void addYourselfLeftSplittedAtTo( const double t, VPath& path ) const;
	virtual QPtrVector<VSegment> splitAtMidpoint() const;
};

void
VLineTo::calcBoundingBox( KoPoint& min, KoPoint& max ) const
{
	if ( previous() == 0L )
		return;

	if( p0()->x() > p3()->x() )
		{ min.setX( p3()->x() ); max.setX( p0()->x() ); }
	else
		{ min.setX( p0()->x() ); max.setX( p3()->x() ); }

	if( p0()->y() > p3()->y() )
		{ min.setY( p3()->y() ); max.setY( p0()->y() ); }
	else
		{ min.setY( p0()->y() ); max.setY( p3()->y() ); }
}

void
VLineTo::addYourselfLeftSplittedAtTo( const double t, VPath& path ) const
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;
//	path.lineTo();
}

QPtrVector<VSegment>
VLineTo::splitAtMidpoint( ) const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint midPoint(
		( p0()->x() + p3()->x() ) * 0.5,
		( p0()->y() + p3()->y() ) * 0.5 );

	VLineTo* line1 = new VLineTo( midPoint, previous() );
	VLineTo* line2 = new VLineTo( *p3(), line1 );

	segments.insert( 0, line1 );
	segments.insert( 1, line2 );

	return segments;
}


class VCurveTo : public VSegment
{
public:
	VCurveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3, VSegment*
 prev )
		: m_p1( p1 ), m_p2( p2 ), VSegment( p3, prev ) {}

	virtual const KoPoint* p1() const
		{ return &m_p1; }
	virtual const KoPoint* p2() const
		{ return &m_p2; }

	virtual void setP1( const KoPoint& p ) { m_p1 = p; }
	virtual void setP2( const KoPoint& p ) { m_p2 = p; }

	virtual void addYourselfLeftSplittedAtTo( const double t, VPath& path ) const;
	virtual QPtrVector<VSegment> splitAtMidpoint() const;

private:
	KoPoint m_p1;
	KoPoint m_p2;
};

void
VCurveTo::addYourselfLeftSplittedAtTo( const double t, VPath& path ) const
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;
//	path.curveTo();
}

QPtrVector<VSegment>
VCurveTo::splitAtMidpoint( ) const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint midPoint(
		( p0()->x() + p3()->x() ) * 0.5,
		( p0()->y() + p3()->y() ) * 0.5 );

	VLineTo* line1 = new VLineTo( midPoint, previous() );
	VLineTo* line2 = new VLineTo( *p3(), line1 );

	segments.insert( 0, line1 );
	segments.insert( 1, line2 );

	return segments;
}


class VCurve1To : public VSegment
{
public:
	VCurve1To( const KoPoint& p2, const KoPoint& p3, VSegment* prev )
		: m_p2( p2 ), VSegment( p3, prev ) {}

	virtual const KoPoint* p1() const
		{ return p0(); }
	virtual const KoPoint* p2() const
		{ return &m_p2; }

	virtual void setP2( const KoPoint& p ) { m_p2 = p; }

	virtual void addYourselfLeftSplittedAtTo( const double t, VPath& path ) const;
	virtual QPtrVector<VSegment> splitAtMidpoint() const;

private:
	KoPoint m_p2;
};

void
VCurve1To::addYourselfLeftSplittedAtTo( const double t, VPath& path ) const
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;
//	path.curve1To();
}

QPtrVector<VSegment>
VCurve1To::splitAtMidpoint( ) const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint midPoint(
		( p0()->x() + p3()->x() ) * 0.5,
		( p0()->y() + p3()->y() ) * 0.5 );

	VLineTo* line1 = new VLineTo( midPoint, previous() );
	VLineTo* line2 = new VLineTo( *p3(), line1 );

	segments.insert( 0, line1 );
	segments.insert( 1, line2 );

	return segments;
}


class VCurve2To : public VSegment
{
public:
	VCurve2To( const KoPoint& p1, const KoPoint& p3, VSegment* prev )
		: m_p1( p1 ), VSegment( p3, prev ) {}

	virtual const KoPoint* p1() const
		{ return &m_p1; }
	virtual const KoPoint* p2() const
		{ return p3(); }

	virtual void setP1( const KoPoint& p ) { m_p1 = p; }

	virtual void addYourselfLeftSplittedAtTo( const double t, VPath& path ) const;
	virtual QPtrVector<VSegment> splitAtMidpoint() const;

private:
	KoPoint m_p1;
};

void
VCurve2To::addYourselfLeftSplittedAtTo( const double t, VPath& path ) const
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;
//	path.curve2To();
}

QPtrVector<VSegment>
VCurve2To::splitAtMidpoint( ) const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint midPoint(
		( p0()->x() + p3()->x() ) * 0.5,
		( p0()->y() + p3()->y() ) * 0.5 );

	VLineTo* line1 = new VLineTo( midPoint, previous() );
	VLineTo* line2 = new VLineTo( *p3(), line1 );

	segments.insert( 0, line1 );
	segments.insert( 1, line2 );

	return segments;
}


class VClosePath : public VSegment
{
public:
	VClosePath( const KoPoint& p3, VSegment* prev = 0L )
		: VSegment( p3, prev ) {}

	virtual bool isFlat() const { return true; }

	virtual void addYourselfLeftSplittedAtTo( const double t, VPath& path ) const;
	virtual QPtrVector<VSegment> splitAtMidpoint() const;
};

void
VClosePath::addYourselfLeftSplittedAtTo( const double t, VPath& path ) const
{
	if ( p0() == 0L || t < 0.0 || t > 1.0 )
		return;
//	path.close();
}

QPtrVector<VSegment>
VClosePath::splitAtMidpoint( ) const
{
	QPtrVector<VSegment> segments( 2 );

	if ( p0() == 0L )
		return segments;

	KoPoint midPoint(
		( p0()->x() + p3()->x() ) * 0.5,
		( p0()->y() + p3()->y() ) * 0.5 );

	VLineTo* line1 = new VLineTo( midPoint, previous() );
	VLineTo* line2 = new VLineTo( *p3(), line1 );

	segments.insert( 0, line1 );
	segments.insert( 1, line2 );

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
	QPtrListIterator<VSegment> itr( m_segments );

	for( ; itr.current(); ++itr )
	{
		if( VClosePath* seg = dynamic_cast<VClosePath*>( itr.current() ) )
			continue;
		else
		if( VMoveTo* seg = dynamic_cast<VMoveTo*>( itr.current() ) )
			painter.moveTo( seg->p3()->x(), seg->p3()->y() );
		else
		if( VLineTo* seg = dynamic_cast<VLineTo*>( itr.current() ) )
			painter.lineTo( seg->p3()->x(), seg->p3()->y() );
		else
		{
			QPointArray seg_qpa( 4 );
			seg_qpa.setPoint( 0, seg->p0()->x(), seg->p0()->y() );
			seg_qpa.setPoint( 1, seg->p1()->x(), seg->p1()->y() );
			seg_qpa.setPoint( 2, seg->p2()->x(), seg->p2()->y() );
			seg_qpa.setPoint( 3, seg->p3()->x(), seg->p3()->y() );

			painter.drawPolyline( seg_qpa.cubicBezier() );
			painter.moveTo( seg->p3()->x(), seg->p3()->y() );
		}
	}


/*
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
*/
	// <<< draw the path contour <<<



	// >>> draw the points >>>
	itr.toFirst();	// reset iterator
	painter.setBrush( Qt::NoBrush );

	for( ; itr.current(); ++itr )
	{
		// draw boxes:
		painter.setPen( Qt::black );
		const uint handleSize = 3;

		painter.drawRect(
			itr.current()->p3()->x() - handleSize,
			itr.current()->p3()->y() - handleSize,
			handleSize*2 + 1,
			handleSize*2 + 1 );


/*		if( itr.current()->firstPoint( itr - 1 ) )
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
		if( itr.current()->lastCtrlPoint( itr - 1 ) )painter.lineTo(
 itr.current()->m_p3.x(), itr.current()->m_p3.y() );
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
*/
	}

	// <<< draw the points <<<

	painter.restore();
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

	// modify last moveTo:
	if ( dynamic_cast<VMoveTo*>( m_segments.getLast() ) )
	{
		m_segments.getLast()->setP3( KoPoint( x, y ) );
	}
	else
		// or append a new moveTo:
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
			KoPoint( x3, y3 ), m_segments.getLast() ) );

	return *this;
}

VPath&
VPath::curve1To(
	const double& x2, const double& y2,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append(
		new VCurve1To(
			KoPoint( x2, y2 ),
			KoPoint( x3, y3 ), m_segments.getLast() ) );

	return *this;
}

VPath&
VPath::curve2To(
	const double& x1, const double& y1,
	const double& x3, const double& y3 )
{
	if( isClosed() ) return *this;

	m_segments.append(
		new VCurve2To(
			KoPoint( x1, y1 ),
			KoPoint( x3, y3 ), m_segments.getLast() ) );

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

	if( denom == 0.0 )			// points are co-linear
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
VPath::booleanOp( const VPath* path, int type ) const
{
	// step one: find intersections.

	// the new intersected paths:
	VPath pathA;
	VPath pathB;

	QPtrListIterator<VSegment> itrA( m_segments );
	QPtrListIterator<VSegment> itrB( path->m_segments );

	for( ; itrA.current(); ++itrA )
	{
		for( itrB.toFirst(); itrB.current(); ++itrB )
		{
			itrA.current()->addIntersectionsWithTo(
				itrB.current(), pathA, pathB );
		}
	}


	return 0L;
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
