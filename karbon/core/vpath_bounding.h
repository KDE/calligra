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

	bool intersects( const KoRect& rect, const double zoomFactor, const VSegmentList& list );

	virtual bool begin( const KoPoint& p );
	virtual bool curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3 )
		{ return curveTo( p2, p2, p3 ); }
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3 )
		{ return curveTo( p1, p3, p3 ); }
	virtual bool lineTo( const KoPoint& p );
	virtual bool end( const KoPoint& p )
		{ return lineTo( p ); }

private:
	double m_zoomFactor;
	//QRect* m_rect;
	QPointArray m_pa;
};

#endif
