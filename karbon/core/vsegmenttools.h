/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENTTOOLS_H__
#define __VSEGMENTTOOLS_H__

class KoPoint;

// This is a place for algorithms which are helpfull in various places.

namespace VSegmentTools
{
	/// Returns true if lines A0A1 and B0B1 intersect.
	bool linesIntersect(
		const KoPoint& a0,
		const KoPoint& a1,
		const KoPoint& b0,
		const KoPoint& b1 );
}

#endif
