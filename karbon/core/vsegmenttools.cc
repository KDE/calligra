/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <koPoint.h>

#include "vglobal.h"
#include "vsegmenttools.h"

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

double
VSegmentTools::height(
	const KoPoint& a,
	const KoPoint& p,
	const KoPoint& b )
{
	// calculate determinant of AP and AB to obtain projection of vector AP to
	// the orthogonal vector of AB:
	const double det =
		p.x() * a.y() + b.x() * p.y() - p.x() * b.y() -
		a.x() * p.y() + a.x() * b.y() - b.x() * a.y();

	// calculate norm = length(AB):
	const double norm = sqrt(
		( b.x() - a.x() ) * ( b.x() - a.x() ) +
		( b.y() - a.y() ) * ( b.y() - a.y() ) );

	// if norm is very small, simply use distance AP:
	if( norm < 1.0e-6 )
		return
			sqrt(
				( p.x() - a.x() ) * ( p.x() - a.x() ) +
				( p.y() - a.y() ) * ( p.y() - a.y() ) );

	// normalize:
	return det / norm;
}
