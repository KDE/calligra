/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHINTERSECT_H__
#define __VPATHINTERSECT_H__

#include "vsegment.h"

class VPath;

// calulate intersection points of two vpaths.

class VPathIntersect : public VSegmentListTraverser
{
public:
	VPathIntersect();

	VPath* intersect( const VSegmentList& list1, const VSegmentList& list2 );

	virtual bool begin( const KoPoint& p );
	virtual bool curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3 )
		{ return curveTo( p2, p2, p3 ); }
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3 )
		{ return curveTo( p1, p3, p3 ); }
	virtual bool lineTo( const KoPoint& p );
	virtual bool end( const KoPoint& p )
		{ return lineTo( p ); }

	const KoPoint& previousPoint2() const { return m_previousPoint2; }
	void setPreviousPoint2( const KoPoint& p ) { m_previousPoint2 = p; }

private:
	VPath* m_path;
	VSegment m_segment; 	// current segment
	bool m_isList1;			// current "path == list1"?
	VSegmentList* m_list2;
	KoPoint m_previousPoint2;
};

#endif
