/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VPATHCONTOUR_H__
#define __VPATHCONTOUR_H__

#include "vcontour.h"
#include "vsegment.h"

class QPainter;

class VPathContour : public VContour, VSegmentListTraverser
{
public:
	VPathContour( const double width = 1.0, const VLineCap cap = cap_butt,
		const VLineJoin join = join_miter );

	void draw( QPainter& painter, const double zoomFactor, const VSegmentList& list,
		bool plain = false );	// "plain" is for drawing objects while editing them

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
	QPointArray m_pa;
};

#endif
