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
	enum VLineCap{
		cap_butt,
		cap_round,
		cap_square };

	enum VLineJoin{
		join_miter,
		join_round,
		join_bevel };

	VContour();
	VContour( const double width, const VLineCap cap, const VLineJoin join );

	void draw( QPainter& painter, const double zoomFactor, const VSegmentList& list,
		bool pure = false );	// "pure" is for drawing objects while editing them

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
	VLineCap m_lineCap;
	VLineJoin m_lineJoin;

	// stuff for painting:
	double m_zoomFactor;
	QPointArray m_pa;
};

#endif
