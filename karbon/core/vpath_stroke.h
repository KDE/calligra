/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHSTROKE_H__
#define __VPATHSTROKE_H__

#include "vsegment.h"

class VStroke;
class VPainter;

class VPathStroke : public VSegmentListTraverser
{
public:
	VPathStroke( const VStroke& stroke );
	virtual ~VPathStroke() {}

	void draw( VPainter *painter, const double zoomFactor, const VSegmentList& list,
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
	const VStroke& m_stroke;
	VPainter *m_painter;
	double m_zoomFactor;
	QPointArray m_pa;
};

#endif
