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
	const KoPoint& p3  )
{
	// calculate determinant of p0p and p0p3 to obtain projection of vector p0p to
	// the orthogonal vector of p0p3:
	const double det =
		p.x()  * p0.y() + p3.x() * p.y()  - p.x()  * p3.y() -
		p0.x() * p.y()  + p0.x() * p3.y() - p3.x() * p0.y();

	// calculate norm = length(p0p3):
	const double norm = sqrt(
		( p3.x() - p0.x() ) * ( p3.x() - p0.x() ) +
		( p3.y() - p0.y() ) * ( p3.y() - p0.y() ) );

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
VSegmentTools::isFlat(
	const KoPoint& p0,
	const KoPoint& p1,
	const KoPoint& p2,
	const KoPoint& p3  )
{
	return (
		isFlat( p0, p1, p3 ) &&
		isFlat( p0, p2, p3 ) );
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

	const double r3 = delta_a.y() * b0.x() - delta_a.x() * b0.y() + det_a;
	const double r4 = delta_a.y() * b1.x() - delta_a.x() * b1.y() + det_a;

	if( r3 != 0.0 && r4 != 0.0 && r3 * r4 > 0.0 )
		return false;

	const KoPoint delta_b = b1 - b0;
	const double det_b = b1.x() * b0.y() - b1.y() * b0.x();

	const double r1 = delta_b.y() * a0.x() - delta_b.x() * a0.y() + det_b;
	const double r2 = delta_b.y() * a1.x() - delta_b.x() * a1.y() + det_b;

	if( r1 != 0.0 && r2 != 0.0 && r1 * r2 > 0.0 )
		return false;

	return true;
}

void
VSegmentTools::polygonize(
	const KoPoint& p0,
	const KoPoint& p1,
	const KoPoint& p2,
	const KoPoint& p3,
	const double zoomFactor,
	VSegmentList& list )
{

}
