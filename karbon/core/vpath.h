/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VPATH_H__
#define __VPATH_H__

#include <qlist.h>
#include <qpointarray.h>

#include "vobject.h"
#include "vrect.h"

class VAffineMap;
class VPoint;


// VSegment is the abstract base class for VLine and VBezier. It is basically
// a help-class for VPath (see below). Segments share exactly one point: the
// lastPoint of the previous segment.

class VSegment
{
public:
	VSegment();
	VSegment( const double lpX, const double lpY );

	virtual const VPoint* firstPoint( const VSegment* prevSeg = 0L ) const
		{ return prevSeg ? prevSeg->lastPoint() : 0L; }
	virtual const VPoint* firstCtrlPoint( const VSegment* prevSeg = 0L ) const = 0;
	virtual const VPoint* lastCtrlPoint( const VSegment* prevSeg = 0L ) const = 0;
	// we dont need prevSeg for lastPoint(). we allow passing it for esthetical
	// reasons:
	const VPoint* lastPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_lastPoint; }

	// revert the segment order:
	virtual const VSegment* revert( const VSegment& prevSeg ) = 0;

	// apply a affine map:
	virtual void transform( const VAffineMap& affMap ) = 0;

	// getQPointArray returns the QPoints which define the segment's contour.
	// beziers need the previous segment's lastpoint to calculate all their
	// QPoints, that's why we pass prevSeg.
	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double zoomFactor ) const = 0;

protected:
	VPoint m_lastPoint;

	mutable bool m_isDirty;				// have to recalculate m_QPointArray?
	static double s_lastZoomFactor;		// cache last zoomFactor
	mutable QPointArray m_QPointArray;
};

// VFirstPoint is a bit ugly per se, but necessary. we simply need a first point
// in a path.

class VFirstPoint : public VSegment
{
public:
	VFirstPoint( const double lpX = 0.0, const double lpY = 0.0 );
	virtual const VPoint* firstPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_lastPoint; }
	virtual const VPoint* firstCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return 0L; }
	virtual const VPoint* lastCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return 0L; }

	virtual const VSegment* revert( const VSegment& prevSeg )
		{ return 0L; }

	// apply a affine map:
	virtual void transform( const VAffineMap& affMap );

	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double zoomFactor ) const;
};

// VLine is very similar to VFirstPoint, but not quite.

class VLine : public VSegment
{
public:
	VLine( const double lpX = 0.0, const double lpY = 0.0 );

	virtual const VPoint* firstCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return 0L; }
	virtual const VPoint* lastCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return 0L; }

	virtual const VSegment* revert( const VSegment& prevSeg );

	virtual void transform( const VAffineMap& affMap );

	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double zoomFactor ) const;
};


// VCurve is a ordinary bezier. VCurve1 has "no" first control-point, VCurve2
// has "no" last control-point (have a look at the diagrams near
// VPath::curveTo(), VPath::curve1To() and VPath::curve2To() ).

class VCurve : public VSegment
{
public:
	VCurve(
		const double fcpX = 0.0, const double fcpY = 0.0,
		const double lcpX = 0.0, const double lcpY = 0.0,
		const double lpX = 0.0, const double lpY = 0.0 );

	virtual const VPoint* firstCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_firstCtrlPoint; }
	virtual const VPoint* lastCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_lastCtrlPoint; }

	virtual const VSegment* revert( const VSegment& prevSeg );

	virtual void transform( const VAffineMap& affMap );

	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double zoomFactor ) const;

private:
	VPoint m_firstCtrlPoint;
	VPoint m_lastCtrlPoint;

	VRect m_boundingBox;
};


class VCurve1 : public VSegment
{
public:
	VCurve1(
		const double lcpX = 0.0, const double lcpY = 0.0,
		const double lpX = 0.0, const double lpY = 0.0 );

	virtual const VPoint* firstCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return prevSeg ? prevSeg->lastPoint() : 0L; }
	virtual const VPoint* lastCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_lastCtrlPoint; }

	virtual const VSegment* revert( const VSegment& prevSeg );

	virtual void transform( const VAffineMap& affMap );

	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double zoomFactor ) const;

private:
	VPoint m_lastCtrlPoint;

	VRect m_boundingBox;
};


class VCurve2 : public VSegment
{
public:
	VCurve2(
		const double fcpX = 0.0, const double fcpY = 0.0,
		const double lpX = 0.0, const double lpY = 0.0 );

	virtual const VPoint* firstCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_firstCtrlPoint; }
	virtual const VPoint* lastCtrlPoint( const VSegment* prevSeg = 0L ) const
		{ return &m_lastPoint; }

	virtual const VSegment* revert( const VSegment& prevSeg );

	virtual void transform( const VAffineMap& affMap );

	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double zoomFactor ) const;

private:
	VPoint m_firstCtrlPoint;

	VRect m_boundingBox;
};


// VPaths are the most common high-level objects. They consist of
// VSegments, which are VLines or VBeziers.

class VPath : public VObject
{
public:
	VPath();
	VPath( const VPath& path );
	virtual ~VPath();

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor = 1.0 );

	const VPoint* currentPoint() const;
	const VSegment* lastSegment() const { return m_segments.getLast(); };

	// postscript-like commands:
	VPath& moveTo( const double x, const double y );
	VPath& lineTo( const double x, const double y );

	// curveTo():
	//
	//   p1          p2
	//    O   ____   O
	//    | _/    \_ |
	//    |/        \|
	//    x          x
	// currP         p3
	//
	VPath& curveTo(
		const double x1, const double y1,
		const double x2, const double y2,
		const double x3, const double y3 );

	// curve1To():
	//
	//               p2
	//         ____  O
	//      __/    \ |
	//     /        \|
	//    x          x
	// currP         p3
	//
	VPath& curve1To(
		const double x2, const double y2,
		const double x3, const double y3 );

	// curve2To():
	//
	//   p1
	//    O  ____
	//    | /    \__
	//    |/        \
	//    x          x
	// currP         p3
	//
	VPath& curve2To(
		const double x1, const double y1,
		const double x3, const double y3 );

	// this is a convenience function to approximate circular arcs with
	// beziers
	VPath& arcTo(
		const double x1, const double y1,
		const double x2, const double y2, const double r );

	VPath& close();
	const bool isClosed() { return( m_isClosed ); }

	const VPath& revert();

	virtual VObject& transform( const VAffineMap& affMap );

private:
	// m_segments store all segemnts ( lines or beziers)
	QList<VSegment>	m_segments;

	bool m_isClosed;
};

#endif
