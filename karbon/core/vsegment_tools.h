/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENTTOOLS_H__
#define __VSEGMENTTOOLS_H__

#include <koRect.h>

#include "vsegment.h"

class KoPoint;

namespace VSegmentTools
{
	bool linesIntersect(
		const KoPoint& a0,
		const KoPoint& a1,
		const KoPoint& b0,
		const KoPoint& b1 );

/*	KoRect boundingBox( const KoPoint& previous, const VSegment& segment );

	void findIntersections( const double t0, const double t1,
	const VSegment* segB, const double u0, const double u1,
	QValueList<double>& paramsA, QValueList<double>& paramsB ) const;
*/
}

#endif
