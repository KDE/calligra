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
	segment_curve1,		// + first ctrl-point is identical to first knot.
	segment_curve2,		// + last ctrl-point is identical to last knot.
	segment_line,		// lineto
	segment_end
};

class VSegment
{
friend class VSegmentList;
friend class VSegmentListIterator;

public:
	VSegment();
	VSegment( const VSegment& segment );

	VSegmentType type() const { return m_type; }
	void setType( VSegmentType t ) { m_type = t; }

	const KoPoint& ctrlPoint1() const { return m_point[0]; }
	const KoPoint& ctrlPoint2() const { return m_point[1]; }

	/// Make sure yourself this segment has m_prev != 0L.
	const KoPoint& knot1() const { return m_prev->m_point[2]; }
	const KoPoint& knot2() const { return m_point[2]; }

	void setCtrlPoint1( const KoPoint& p ) { m_point[0] = p; }
	void setCtrlPoint2( const KoPoint& p ) { m_point[1] = p; }
	void setKnot2( const KoPoint& p ) { m_point[2] = p; }

	/// Returns true if segment is flat.
	bool isFlat( double flatness = VGlobal::flatnessTolerance ) const;

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

private:
	VSegment* m_prev;
	VSegment* m_next;

	VSegmentType m_type;
	bool m_smooth;			// first ctrl-point is "smooth".
	KoPoint m_point[3];
};

#endif
