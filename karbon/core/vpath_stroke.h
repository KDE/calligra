/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHSTROKE_H__
#define __VPATHSTROKE_H__

#include "vstroke.h"
#include "vsegment.h"

class QPainter;

class VPathStroke : public VStroke, VSegmentListTraverser
{
public:
	VPathStroke( const double width = 1.0, const VLineCap cap = cap_butt,
		const VLineJoin join = join_miter );

	void draw( QPainter& painter, const double zoomFactor, const VSegmentList& list,
		bool plain = false );	// "plain" is for drawing objects while editing them

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
	QPointArray m_pa;
};

#endif
