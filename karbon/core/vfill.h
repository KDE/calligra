/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

#include "vsegment.h"

class QPainter;

class VFill : public VSegmentListTraverser
{
public:
	VFill();

	void begin_draw( QPainter& painter, const double zoomFactor );
	void draw( const VSegmentList& list );
	void end_draw();

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
	// stuff for painting:
	QPainter* m_painter;
	double m_zoomFactor;
	QPointArray m_pa;
};

#endif
