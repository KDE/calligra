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

	virtual void begin( const KoPoint& p );
	virtual void curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );
	virtual void curve1To( const KoPoint& p2, const KoPoint& p3 )
		{ curveTo( p2, p2, p3 ); }
	virtual void curve2To( const KoPoint& p1, const KoPoint& p3 )
		{ curveTo( p1, p3, p3 ); }
	virtual void lineTo( const KoPoint& p );
	virtual void end( const KoPoint& p )
		{ lineTo( p ); }

private:
	KoRect* m_rect;
};

#endif
