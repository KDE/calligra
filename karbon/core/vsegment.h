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


	VSegment( VSegmentType type = begin );
	VSegment( const VSegment& segment );

	VSegmentType type() const
		{ return m_type; }
	void setType( VSegmentType type )
		{ m_type = type; }

	VCtrlPointFixing ctrlPointFixing() const
		{ return m_ctrlPointFixing; }
	void setCtrlPointFixing( VCtrlPointFixing fixing )
		{ m_ctrlPointFixing = fixing; }

	/**
	 * Returns node with 1 <= index <= 3.
	 */
	const KoPoint& node( uint index ) const
		{ return m_node[--index]; }

	/**
	 * Sets node with 1 <= index <= 3 to point.
	 */
	void setNode( uint index, const KoPoint& point )
		{ m_node[--index] = point; }

	const KoPoint& ctrlPoint1() const
		{ return m_node[0]; }
	const KoPoint& ctrlPoint2() const
		{ return m_node[1]; }
	const KoPoint& knot() const
		{ return m_node[2]; }

	void setCtrlPoint1( const KoPoint& p )
		{ m_node[0] = p; }
	void setCtrlPoint2( const KoPoint& p )
		{ m_node[1] = p; }
	void setKnot( const KoPoint& p )
		{ m_node[2] = p; }

	/**
	 * Returns index of the node at point p. Returns 0 of none
	 * matches.
	 */
	uint nodeNear( const KoPoint& p,
		double isNearRange = VGlobal::isNearRange ) const;

	/**
	 * Returns a reverted version of this segment. For example:
	 * if this segment is a line from A to B, the result is a line from
	 * B to A.
	 */
	VSegment* revert() const;

	/**
	 * Returns a pointer to the previous segment, if stored in a
	 * VSegmentList.
	 */
	VSegment* prev() const
		{ return m_prev; }

	/**
	 * Returns a pointer to the next segment, if stored in a
	 * VSegmentList.
	 */
	VSegment* next() const
		{ return m_next; }

	/**
	 * Returns true if the segment is flat. That means it's height is
	 * smaller than flatness.
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
	 * parameters t and 1 - t.
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

	/**
	 * Select the node at point p.
	 */
	bool select(
		const KoPoint& p,
		double isNearRange = VGlobal::isNearRange,
		bool select = true );

	/**
	 * Deselect the node at point p.
	 */
	bool deselect( const KoPoint& p,
		double isNearRange = VGlobal::isNearRange )
	{
		return select( p, isNearRange, false );
	}

	/**
	 * Select all nodes inside rect.
	 */
	bool select( const KoRect& rect );

	/**
	 * Deselect all nodes inside rect.
	 */
	void deselect( const KoRect& rect );

	/**
	 * Deselect all nodes.
	 */
	void deselect();

	/**
	 * Returns true if node with 1 <= index <= 3 is selected.
	 */
	bool selected( uint index ) const
		{ return m_nodeSelected[--index]; }

	bool edited( uint index ) const
		{ return m_nodeEdited[--index]; }

	/**
	 * Returns true if at least one node is selected.
	 */
	bool hasSelectedNodes() const;

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	/**
	 * Returns a pointer to a copy of this segment.
	 */
	VSegment* clone() const;

private:
	// TODO : struct?
	KoPoint m_node[3];
	bool m_nodeSelected[3];
	bool m_nodeEdited[3];

	VSegment* m_prev;
	VSegment* m_next;

	VSegmentType m_type;
	VCtrlPointFixing m_ctrlPointFixing;

	/// Second control point is "smooth".
	bool m_smooth;
};

#endif
