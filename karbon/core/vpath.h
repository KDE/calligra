/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATH_H__
#define __VPATH_H__

#include <qpointarray.h>
#include <qptrlist.h>

#include <koPoint.h>

#include "vobject.h"
#include "vpath_fill.h"
#include "vpath_stroke.h"
#include "vsegment.h"

class QWMatrix;

class VPath : public VObject
{
public:
	VPath();
	VPath( const VPath& path );
	virtual ~VPath();

	const KoPoint& currentPoint() const;

	// postscript-like commands:
	VPath& moveTo( const double& x, const double& y );
	VPath& moveTo( const KoPoint& p )
		{ return moveTo( p.x(), p.y() ); }

	VPath& lineTo( const double& x, const double& y );
	VPath& lineTo( const KoPoint& p )
		{ return lineTo( p.x(), p.y() ); }

	/*
	curveTo():

	   p1          p2
	    O   ____   O
	    | _/    \_ |
	    |/        \|
	    x          x
	currP          p3
	*/
	VPath& curveTo(
		const double& x1, const double& y1,
		const double& x2, const double& y2,
		const double& x3, const double& y3 );
	VPath& curveTo(
		const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
	{
		return curveTo( p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y() );
	}

	/*
	curve1To():

	               p2
	         ____  O
	      __/    \ |
	     /        \|
	    x          x
	currP          p3
	*/
	VPath& curve1To(
		const double& x2, const double& y2,
		const double& x3, const double& y3 );
	VPath& curve1To(
		const KoPoint& p2, const KoPoint& p3 )
	{
		return curve1To( p2.x(), p2.y(), p3.x(), p3.y() );
	}

	/*
	curve2To():

	   p1
	    O  ____
	    | /    \__
	    |/        \
	    x          x
	currP          p3
	*/
	VPath& curve2To(
		const double& x1, const double& y1,
		const double& x3, const double& y3 );
	VPath& curve2To(
		const KoPoint& p1, const KoPoint& p3 )
	{
		return curve2To( p1.x(), p1.y(), p3.x(), p3.y() );
	}

	// this is a convenience function to approximate circular arcs with
	// beziers. input: 2 tangent vectors and a radius (same as in postscript):
	VPath& arcTo(
		const double& x1, const double& y1,
		const double& x2, const double& y2, const double& r );
	VPath& arcTo(
		const KoPoint& p1, const KoPoint& p2, const double& r )
	{
		return arcTo( p1.x(), p1.y(), p2.x(), p2.y(), r );
	}

	VPath& close();
	bool isClosed() const { return m_isClosed; }

	// return a reverted path:
	VPath* revert() const;

	// perform a boolean operation (unite(0), intersect(1), substract(2), xor(3)):
	VPath* booleanOp( const VPath* path, int type = 0 ) const;

	void combine( const VPath& path );

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor = 1.0 );

	const VSegment* lastSegment() const { return m_segments.getLast(); }

	// apply an affine map:
	virtual VObject& transform( const QWMatrix& m );

	virtual QRect boundingBox( const double zoomFactor ) const;
	virtual bool intersects( const QRect& rect, const double zoomFactor ) const;

	virtual VObject* clone();

private:
	void drawBox( QPainter &painter, double x, double y, uint handleSize = 3 ); // helper function for draw()

private:
	VSegmentList m_segments;			// "outline" segments.
	QPtrList<VSegmentList> m_holes;		// "holes" inside the outline

	VPathStroke m_stroke;		// stroke.
	VPathFill m_fill;			// fill.

	bool m_isClosed;
};

#endif
