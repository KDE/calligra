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


// VSegment is the abstract base class for VLine and VBezier. Segments are a
// help-class for VPath (see below). Segments share exactly one point: the
// lastPoint of the previos segment.

class VSegment
{
public:
	VSegment( const double lp_x = 0.0, const double lp_y = 0.0 );
	virtual ~VSegment();

	// Beziers need the previous segment's lastpoint to calculate all their QPoints
	virtual const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double& zoomFactor ) const = 0;

	virtual const VPoint* firstCtrlPoint( const VSegment& prevSeg ) { return 0L; }
	virtual const VPoint* lastCtrlPoint( const VSegment& prevSeg ) { return 0L; }
	const VPoint* lastPoint() const { return &m_lastPoint; }

protected:
	// m_firstPoint is a waste, that's why we skip it
	VPoint m_lastPoint;

	mutable QPointArray m_QPointArray;
};


class VLine : public VSegment
{
public:
	VLine( const double lp_x = 0.0, const double lp_y = 0.0 );
	virtual ~VLine();

	const QPointArray& getQPointArray(  const VSegment& prevSeg,
		const double& zoomFactor ) const {};
};


class VCurve : public VSegment
{
public:
	// describes how control-points are handled (see VPath::curve*To() for this)
	enum CtrlPointsConstraint { no = 0, firstFixed = 1, lastFixed = 2 };

	VCurve(
		const double fcp_x = 0.0, const double fcp_y = 0.0,
		const double lcp_x = 0.0, const double lcp_y = 0.0,
		const double lp_x = 0.0, const double lp_y = 0.0,
		const CtrlPointsConstraint cpc = no );
	virtual ~VCurve();

	const QPointArray& getQPointArray( const VSegment& prevSeg,
		const double& zoomFactor ) const {};

	virtual const VPoint* firstCtrlPoint( const VSegment& prevSeg );
	virtual const VPoint* lastCtrlPoint( const VSegment& prevSeg );

private:
	VPoint m_firstCtrlPoint;
	VPoint m_lastCtrlPoint;
	CtrlPointsConstraint m_ctrlPointConstraint;

	// a bezier lies completely inside the "minmax box" (this information is
	// usefull for speeding up intersection-calculations):
	VRect m_minMaxBox;
};


// VPaths are the most common high-level objects. They consist of
// VSegments, which are VLines or VBeziers.

class VPath : public VObject
{
public:
	VPath();
	virtual ~VPath();

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor = 1.0 );

	const VPoint* currentPoint() const;
	const VSegment* lastSegment() const { return m_segments.getLast(); };

	// postscript-like commands:
	VPath& moveTo( const double x, const double y );
	VPath& rmoveTo( const double dx, const double dy );

	VPath& lineTo( const double x, const double y );
	VPath& rlineTo( const double dx, const double dy );

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
	bool isClosed() { return( m_isClosed ); }

	virtual VObject& translate( const double dx = 0.0, const double dy = 0.0 );
	virtual VObject& rotate( const double ang = 0.0 );
	virtual VObject& mirror( const bool horiz = false, const bool verti = false );
	virtual VObject& scale( const double sx = 1.0, const double sy = 1.0 );
	virtual VObject& shear( const double sh = 0.0, const double sv = 1.0 );
	virtual VObject& skew( const double ang = 0.0 );

	virtual VObject& apply( const VAffineMap& affmap );

private:
	// m_segments store all segemnts ( lines or beziers)
	QList<VSegment>	m_segments;

	bool m_isClosed;
};

#endif
