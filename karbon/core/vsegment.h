/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENT_H__
#define __VSEGMENT_H__

#include <qptrlist.h>

#include <koPoint.h>
#include <koRect.h>

#include "vglobal.h"

class QDomElement;


// line- and all the various other (bezier-)segment types are so similar in practise
// that we only need to have one segment-class.
// this leads to a waste of KoPoints for e.g. line-segments. otoh memory is cheap
// and human resources are expensive. ;)

// segment types:
enum VSegmentType
{
	segment_begin,		// initial moveto
	segment_curve,		// curveto (bezier)
	segment_line		// lineto
};

enum VCtrlPointFixing
{
	segment_none   = 0,
	segment_first  = 1,
	segment_second = 2
};


class VSegment
{
friend class VSegmentList;
friend class VSegmentListIterator;

public:
	VSegment();
	VSegment( const VSegment& segment );

	VSegmentType type() const { return m_type; }
	void setType( VSegmentType type ) { m_type = type; }

	VCtrlPointFixing ctrlPointFixing() const
		{ return m_ctrlPointFixing; }
	void setCtrlPointFixing( VCtrlPointFixing fixing )
		{ m_ctrlPointFixing = fixing; }

	const KoPoint& ctrlPoint1() const { return m_point[0]; }
	const KoPoint& ctrlPoint2() const { return m_point[1]; }
	const KoPoint& knot() const { return m_point[2]; }

	void setCtrlPoint1( const KoPoint& p ) { m_point[0] = p; }
	void setCtrlPoint2( const KoPoint& p ) { m_point[1] = p; }
	void setKnot( const KoPoint& p ) { m_point[2] = p; }

	const VSegment* prev() const { return m_prev; }
	const VSegment* next() const { return m_next; }

	/// Returns true if segment is flat.
	bool isFlat( double flatness = VGlobal::flatnessTolerance ) const;

	/// Returns the point on this segment for 0 <= t <= 1.
	KoPoint point( double t ) const;

	/// Returns the derivative of this segment for 0 <= t <= 1.
	KoPoint derive( double t ) const;

	/**
	 * In case you need the point and the derivative at once for 0 <= t <= 1,
	 * use this function since it is more efficient.  First KoPoint
	 * is the point, the second KoPoint is the derivative.
	 */
	void pointAndDerive( double t, KoPoint& p, KoPoint& der ) const;

// TODO: currently return 0.0 for segments != lines:
	/// Returns the arc length.
	double length() const;

	/// Calculates the bounding box.
	KoRect boundingBox() const;

	/**
	 *  Splits segment at 0 <= t <= 1. Returns the first segment and transforms
	 *  the current one to the second segment. Make sure yourself, this segment
	 *  has a m_prev != 0L and t is in proper range.
	 */
	VSegment* splitAt( double t );

	/**
	 * Makes the segment a bezier curve. Lines will have control points at
	 * t and 1 - t.
	 */
	void convertToCurve( double t = 1.0 / 3.0 );

	/// Returns true if lines A0A1 and B0B1 intersect.
	static bool linesIntersect(
		const KoPoint& a0,
		const KoPoint& a1,
		const KoPoint& b0,
		const KoPoint& b1 );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	VSegment* clone() const;

private:
	VSegment* m_prev;
	VSegment* m_next;

	VSegmentType m_type;
	VCtrlPointFixing m_ctrlPointFixing;
	bool m_smooth;			// first ctrl-point is "smooth".
	KoPoint m_point[3];
};

#endif
