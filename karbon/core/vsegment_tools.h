/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENTTOOLS_H__
#define __VSEGMENTTOOLS_H__

#include "vsegment.h"

class KoPoint;

namespace VSegmentTools
{
	bool isFlat(
		const KoPoint& p0,
		const KoPoint& p,
		const KoPoint& p3 );

	bool isFlat(
		const KoPoint& p0,
		const KoPoint& p1,
		const KoPoint& p2,
		const KoPoint& p3 );

	bool linesIntersect(
		const KoPoint& a0,
		const KoPoint& a1,
		const KoPoint& b0,
		const KoPoint& b1 );

	void polygonize(
		const KoPoint& p0,
		const KoPoint& p1,
		const KoPoint& p2,
		const KoPoint& p3,
		const double zoomFactor,
		VSegmentList& list );
}

#endif
