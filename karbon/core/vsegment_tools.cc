/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <koPoint.h>

#include "vglobal.h"
#include "vsegment_tools.h"

bool
VSegmentTools::isFlat(
	const KoPoint& p0,
	const KoPoint& p,
	const KoPoint& p1  )
{
	// calculate determinant of p0p and p0p1 to obtain projection of vector p0p to
	// the orthogonal vector of p0p1:
	const double det =
		p.x()  * p0.y() + p1.x() * p.y()  - p.x()  * p1.y() -
		p0.x() * p.y()  + p0.x() * p1.y() - p1.x() * p0.y();

	// calculate norm = length(p0p1):
	const double norm = sqrt(
		( p1.x() - p0.x() ) * ( p1.x() - p0.x() ) +
		( p1.y() - p0.y() ) * ( p1.y() - p0.y() ) );

	// if norm is very small, simply use distance p0p:
	if( norm < 1.0e-6 )
		return(
			sqrt(
				( p.x() - p0.x() ) * ( p.x() - p0.x() ) +
				( p.y() - p0.y() ) * ( p.y() - p0.y() ) )
			< VGlobal::flatnessTolerance );

	// normalize:
	return( det / norm < VGlobal::flatnessTolerance );
}

bool
VSegmentTools::linesIntersect(
	const KoPoint& a0,
	const KoPoint& a1,
	const KoPoint& b0,
	const KoPoint& b1 )
{
	const KoPoint delta_a = a1 - a0;
	const double det_a = a1.x() * a0.y() - a1.y() * a0.x();

	const double r_b0 = delta_a.y() * b0.x() - delta_a.x() * b0.y() + det_a;
	const double r_b1 = delta_a.y() * b1.x() - delta_a.x() * b1.y() + det_a;

	if( r_b0 != 0.0 && r_b1 != 0.0 && r_b0 * r_b1 > 0.0 )
		return false;

	const KoPoint delta_b = b1 - b0;
	const double det_b = b1.x() * b0.y() - b1.y() * b0.x();

	const double r_a0 = delta_b.y() * a0.x() - delta_b.x() * a0.y() + det_b;
	const double r_a1 = delta_b.y() * a1.x() - delta_b.x() * a1.y() + det_b;

	if( r_a0 != 0.0 && r_a1 != 0.0 && r_a0 * r_a1 > 0.0 )
		return false;

	return true;
}

/*KoRect
VSegmentTools::boundingBox( const KoPoint& previous, const VSegment& segment )
{
	KoRect rect;

	// first and last knot:
	if( previous.x() > segment.point( 3 ).x() )
	{
		rect.setLeft( segment.point( 3 ).x() );
		rect.setRight( previous.x() );
	}
	else
	{
		rect.setLeft( previous.x() );
		rect.setRight( segment.point( 3 ).x() );
	}

	if( previous.y() > segment.point( 3 ).y() )
	{
		rect.setBottom( previous.y() );
		rect.setTop( segment.point( 3 ).y() );
	}
	else
	{
		rect.setBottom( segment.point( 3 ).y() );
		rect.setTop( previous.y() );
	}

	// control points:
	if(
		segment.type() == VSegment::curve ||
		segment.type() == VSegment::curve1 )
	{
		if( segment.point( 2 ).x() < rect.left() )
			rect.setLeft( segment.point( 2 ).x() );
		if( segment.point( 2 ).x() > rect.right() )
			rect.setRight( segment.point( 2 ).x() );
		if( segment.point( 2 ).y() < rect.top() )
			rect.setTop( segment.point( 2 ).y() );
		if( segment.point( 2 ).y() > rect.bottom() )
			rect.setBottom( segment.point( 2 ).y() );
	}

	if(
		segment.type() == VSegment::curve ||
		segment.type() == VSegment::curve2 )
	{
		if( segment.point( 1 ).x() < rect.left() )
			rect.setLeft( segment.point( 1 ).x() );
		if( segment.point( 1 ).x() > rect.right() )
			rect.setRight( segment.point( 1 ).x() );
		if( segment.point( 1 ).y() < rect.top() )
			rect.setTop( segment.point( 1 ).y() );
		if( segment.point( 1 ).y() > rect.bottom() )
			rect.setBottom( segment.point( 1 ).y() );
	}

	return rect;
}*/

