/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHBOUNDING_H__
#define __VPATHBOUNDING_H__

#include <qpointarray.h>

#include "vsegment.h"

class QRect;

// calulate bounding box of a vpath.

class VPathBounding : public VSegmentListTraverser
{
public:
	VPathBounding();

	// do the "simple" bounding box calculus:
	void calculate( QRect& rect, const double zoomFactor, const VSegmentList& list );
	// check for intersection with zoomFactor-dependent polyline approximation:
	bool intersects( const QRect& rect, const double zoomFactor, const VSegmentList& list );

	virtual bool begin( const KoPoint& p,
		const VSegment& segment );
	virtual bool curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3,
		const VSegment& segment );
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3,
		const VSegment& segment )
			{ return curveTo( p2, p2, p3, segment ); }
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3,
		const VSegment& segment )
			{ return curveTo( p1, p3, p3, segment ); }
	virtual bool lineTo( const KoPoint& p,
		const VSegment& segment );
	virtual bool end( const KoPoint& p,
		const VSegment& segment )
			{ return lineTo( p, segment ); }

private:
	double m_zoomFactor;
	QRect* m_rect;
	QPointArray m_pa;
	bool m_quick;		// do the simple calculus
};

#endif
