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


/**
 * A class representing lines and beziers. We waste some KoPoints, if we
 * would use only lines, but this makes it easy to convert the segment types
 * into each other. Make sure yourself, that you pass values to functions within
 * proper ranges.
 */

class VSegment
{
friend class VSegmentList;
friend class VSegmentListIterator;

public:
	/**
	 * Segment types.
	 */
	enum VSegmentType
	{
		begin,		// initial moveto
		curve,		// curveto (bezier)
		line		// lineto
	};

	/**
	 * Tells which control point is "fixed" i.e. located at the
	 * corresponding knot and invisible. This flag makes no sense for
	 * line segments.
	 */
	enum VCtrlPointFixing
	{
		none   = 0,
		first  = 1,
		second = 2
	};


	VSegment();
	VSegment( const VSegment& segment );

	VSegmentType type() const
		{ return m_type; }
	void setType( VSegmentType type )
		{ m_type = type; }

	VCtrlPointFixing ctrlPointFixing() const
		{ return m_ctrlPointFixing; }
	void setCtrlPointFixing( VCtrlPointFixing fixing )
		{ m_ctrlPointFixing = fixing; }

	const KoPoint& ctrlPoint1() const
		{ return m_point[0]; }
	const KoPoint& ctrlPoint2() const
		{ return m_point[1]; }
	const KoPoint& knot() const
		{ return m_point[2]; }

	void setCtrlPoint1( const KoPoint& p )
		{ m_point[0] = p; }
	void setCtrlPoint2( const KoPoint& p )
		{ m_point[1] = p; }
	void setKnot( const KoPoint& p )
		{ m_point[2] = p; }

	/**
	 * Returns a reverted version of this segment.
	 */
	VSegment* revert() const;

	void transform( const QWMatrix& m );

	/**
	 * Returns a pointer to the previous segment, if stored in a
	 * VSegmentList.
	 */
	const VSegment* prev() const
		{ return m_prev; }
	/**
	 * Returns a pointer to the next segment, if stored in a
	 * VSegmentList.
	 */
	const VSegment* next() const
		{ return m_next; }

	/**
	 * Returns true if the segment is flat.
	 */
	bool isFlat( double flatness = VGlobal::flatnessTolerance ) const;

	/**
	 * Returns the point on this segment for 0 <= t <= 1.
	 * This is a convenience wrapper for pointDerivatives().
	 */
	KoPoint point( double t ) const;

	/**
	 * Returns the tangent vector on this segment for 0 <= t <= 1.
	 * This is a convenience wrapper for pointTangentNormal().
	 */
	KoPoint tangent( double t ) const;

	/**
	 * Returns the point and the derivatives of first and second order
	 * for 0 <= t <= 1.
	 */
	void pointDerivatives( double t, KoPoint* p = 0L,
		KoPoint* d1 = 0L, KoPoint* d2 = 0L ) const;

	/**
	 * Returns the point, the tangent and the normal vector for
	 * 0 <= t <= 1.
	 */
	void pointTangentNormal( double t, KoPoint* p = 0L,
		KoPoint* tn = 0L, KoPoint* n = 0L ) const;

	/**
	 * Returns the arc length from 0 to 0 <= t <= 1.
	 */
	double length( double t = 1.0 ) const;

	/**
	 * Returns the chord length |p0p3|.
	 */
	double chordLength() const;

	/**
	 * Returns the length of the control polygon |p0p1|+|p1p2|+|p2p3|.
	 */
	double polyLength() const;

	/**
	 * Returns the parameter of a point located a length len. This is the
	 * exact inverse operation of length( t ).
	 */
	double param( double len ) const;

	/**
	 * Returns the bounding box.
	 */
	KoRect boundingBox() const;

	/**
	 *  Splits the segment at 0 <= t <= 1. Returns the first segment and transforms
	 *  the current one to the second segment.
	 */
	VSegment* splitAt( double t );

	/**
	 * Makes the segment a bezier curve. Lines will have control points at
	 * t and 1 - t.
	 */
	void convertToCurve( double t = 1.0 / 3.0 );

	/**
	 * Returns true if lines A0A1 and B0B1 intersect.
	 */
	static bool linesIntersect(
		const KoPoint& a0,
		const KoPoint& a1,
		const KoPoint& b0,
		const KoPoint& b1 );

	// set up node selection depending on p
	bool selectNode( const KoPoint &p );
	bool checkNode( const KoPoint &p );
	bool selectNode( const KoRect &r );
	void selectNode();
	bool isSelected() const { return m_isSelected[ 0 ] || m_isSelected[ 1 ] || m_isSelected[ 2 ]; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	/**
	 * Returns a pointer to a copy of this segment.
	 */
	VSegment* clone() const;

private:
	KoPoint m_point[3];
	bool m_isSelected[3];

	VSegment* m_prev;
	VSegment* m_next;

	VSegmentType m_type;
	VCtrlPointFixing m_ctrlPointFixing;

	/// Second control point is "smooth".
	bool m_smooth;
};

#endif
