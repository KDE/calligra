/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCONTOUR_H__
#define __VCONTOUR_H__

#include "vsegment.h"

class QPainter;

class VContour : public VSegmentListTraverser
{
public:
	VContour();

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
	double m_lineWidth;

	// stuff for painting:
	QPainter* m_painter;
	double m_zoomFactor;
};

#endif
