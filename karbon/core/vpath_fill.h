/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHFILL_H__
#define __VPATHFILL_H__

#include "vfill.h"
#include "vsegment.h"

class QPainter;

enum VFillRule { evenOdd = 0, winding = 1 };

class VPathFill : public VFill, VSegmentListTraverser
{
public:
	VPathFill();

	void begin_draw( QPainter& painter, const double zoomFactor, VFillRule rule );
	void draw( const VSegmentList& list );
	void end_draw();

	virtual bool begin( const KoPoint& p );
	virtual bool curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3 )
		{ return curveTo( p2, p2, p3 ); }
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3  )
		{ return curveTo( p1, p3, p3 ); }
	virtual bool lineTo( const KoPoint& p );
	virtual bool end( const KoPoint& p )
		{ return lineTo( p ); }

private:
	QPainter* m_painter;
	double m_zoomFactor;
	QPointArray m_pa;
	VFillRule m_fillRule;
};

#endif
