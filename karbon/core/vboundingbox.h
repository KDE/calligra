/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VBOUNDINGBOX_H__
#define __VBOUNDINGBOX_H__

#include "vsegment.h"

class KoRect;

class VBoundingBox : public VSegmentListTraverser
{
public:
	VBoundingBox();

	void calculate( KoRect& rect, const VSegmentList& list  );

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
	KoRect* m_rect;
};

#endif
