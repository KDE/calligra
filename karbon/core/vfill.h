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

	void draw( QPainter& painter, const double zoomFactor, const VSegmentList& list );

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

	// stuff for painting:
	double m_zoomFactor;
	QPointArray m_pa;
};

#endif
