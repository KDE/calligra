/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VSEGMENT_H__
#define __VSEGMENT_H__

#include <q3ptrlist.h>
#include <q3valuelist.h>

#include <KoPoint.h>
#include <KoRect.h>

#include "vglobal.h"
#include <koffice_export.h>

class QDomElement;
class VPainter;

/**
 * A class representing lines and beziers. We waste some KoPoints, if we
 * would use only lines, but this makes it easy to convert the segment types
 * into each other. Make sure yourself, that you pass values to functions within
 * proper ranges.
 */

class KARBONBASE_EXPORT VSegment
{
	friend class VSubpath;
	friend class VSubpathIterator;

public:
	/**
	 * Tells which control point is "fixed" i.e. located at the
	 * corresponding knot and invisible. This flag makes no sense for
	 * line segments.
	 */
	enum VCtrlPointFixing
	{
		none = 0,
		first = 1,
		second = 2
	};

	enum VState
	{
		normal,
		deleted
	};


	VSegment( unsigned short deg = 3 );

	VSegment( const VSegment& segment );

	~VSegment();

	/**
	 * Returns the segment's degree, which is identical to the number of nodes.
	 * For cubic beziers it is "three" and "one" for lines.
	 */
	unsigned short degree() const
	{
		return m_degree;
	}

	/**
	 * Sets the segment's degree and thus resizes the array of node data.
	 * The node data is copied from the old knot "backwards".
	 */
	void setDegree( unsigned short deg );

	/**
	 * Tests for the segment type ("begin", "line" or "curve").
	 */
	bool isBegin() const { return (degree() == 1) && !prev(); }
	bool isLine() const { return (degree() == 1) && prev(); }
	bool isCurve() const { return degree() > 1; }

	/**
	 * Returns the segment state.
	 */
	VState state() const
	{
		return m_state;
	}

	/**
	 * Sets the segment state.
	 */
	void setState( VState state )
	{
		m_state = state;
	}


	/**
	 * Returns the segment's point with index 0 <= i < degree().
	 */
	const KoPoint& point( int i ) const
	{
		return m_nodes[ i ].m_vector;
	}

	/**
	 * This is a convenience function. It returns the point with index
	 * 0 <= i <= degree() while p( 0 ) is the knot of the previous
	 * segment.
	 */
	const KoPoint& p( int i ) const
	{
		return i == 0
			   ? prev()->knot()
			   : m_nodes[ --i ].m_vector;
	}

	/**
	 * Returns the knot. This is a convenience function using point().
	 */
	const KoPoint& knot() const
	{
		return point( degree() - 1 );
	}

	/**
	 * Sets the segment's point with index 0 <= i < degree() to "p".
	 */
	void setPoint( int i, const KoPoint& p )
	{
		m_nodes[ i ].m_vector = p;
	}

	/**
	 * This is a convenience function. It sets the point with index
	 * 0 <= i <= degree() to "p" while setP( 0 ) sets the knot of the
	 * previous segment.
	 */
	void setP( int i, const KoPoint& p )
	{
		if( i == 0 )
			prev()->setKnot( p );
		else
			m_nodes[ --i ].m_vector = p;
	}

	/**
	 * Sets the knot. This is a convenience function.
	 */
	void setKnot( const KoPoint& p )
	{
		m_nodes[ degree() - 1 ].m_vector = p;
	}


	/**
	 * Returns true if the point with index 0 <= i < degree() is selected.
	 */
	bool pointIsSelected( int i ) const
	{
		return m_nodes[ i ].m_isSelected;
	}

	/**
	 * Returns true if the knot is selected. This is a convenience function.
	 */
	bool knotIsSelected() const
	{
		return m_nodes[ degree() - 1 ].m_isSelected;
	}

	/**
	 * Selects the point with index 0 <= i < degree().
	 */
	void selectPoint( int i, bool select = true )
	{
		m_nodes[ i ].m_isSelected = select;
	}

	/**
	 * Selects/deselects the knot of this segment.
	 */
	void selectKnot( bool select = true )
	{
		m_nodes[ degree() - 1 ].m_isSelected = select;
	}


	/**
	 * Returns index of the node at point p. Returns 0 if no
	 * segment point matches point p.
	 */
	// TODO: Move this function into "userland"
	uint nodeNear( const KoPoint& p,
				   double isNearRange = VGlobal::isNearRange ) const;


	/**
	 * Returns a pointer to the previous not deleted segment, if
	 * stored in a VSubpath.
	 */
	VSegment* prev() const;

	/**
	 * Returns a pointer to the next not deleted segment, if
	 * stored in a VSubpath.
	 */
	VSegment* next() const;

	/**
	 * Returns true if the segment is flat. That means it's height
	 * is smaller than flatness.
	 */
	bool isFlat( double flatness = VGlobal::flatnessTolerance ) const;


	/**
	 * Calculates the point on this segment at parameter 0 <= t <= 1.
	 * This is a convenience wrapper for pointDerivativesAt().
	 */
	KoPoint pointAt( double t ) const;

	/**
	 * Calculates the point and the derivatives of first and
	 * second order for 0 <= t <= 1.
	 */
	void pointDerivativesAt( double t, KoPoint* p = 0L,
							 KoPoint* d1 = 0L, KoPoint* d2 = 0L ) const;


	/**
	 * Calculates the normalized tangent vector (length=1) at the point
	 * parameterized by 0 <= t <= 1. This is a convenience wrapper
	 * for pointTangentNormalAt(). Use the latter function directly if you
	 * need to calculate the point and normal vector or tangent vector
	 * at once.
	 */
	KoPoint tangentAt( double t ) const;

	/**
	 * Calculates the point, the tangent vector and the normal vector for
	 * 0 <= t <= 1. The tangent vector and the normal vector are
	 * normalized (length=1).
	 */
	void pointTangentNormalAt( double t, KoPoint* p = 0L,
							   KoPoint* tn = 0L, KoPoint* n = 0L ) const;


	/**
	 * Calculates the arclength from p0 to the point parametrized
	 * by 0 <= t <= 1. For beziers this function is a bit expensive.
	 */
	double length( double t = 1.0 ) const;

	/**
	 * Calculates the chord length (the distance from the previous
	 * knot to the current knot).
	 */
	double chordLength() const;

	/**
	 * Calculates the length of the control polygon.
	 */
	double polyLength() const;


	/**
	 * Calculates the parameter of a point located at arclength len.
	 * This is the exact inverse operation of length( t ).
	 */
	double lengthParam( double len ) const;


	/**
	 * Calculates the parameter of the nearest point on this segment
	 * to the point p. This function is pretty expensive.
	 */
	double nearestPointParam( const KoPoint& p ) const;


	/**
	 * Calculates wether the tangent at the knot is exactly parallel to
	 * the tangent at p0 of the next segment. Returns false if the
	 * current segment is a "begin".
	 */
	bool isSmooth( const VSegment& next ) const;

	bool isSmooth() const
	{
		return next()
			   ? isSmooth( *next() )
			   : false;
	}


	/**
	 * Creates a reverted version of this segment. For example:
	 * if this segment is a line from A to B, the result is a
	 * line from B to A.
	 */
	VSegment* revert() const;


	/**
	 *  Splits the segment at parameter 0 <= t <= 1. Returns a pointer
	 *  to the first segment and modifies the current one to
	 *  be the second segment.
	 */
	VSegment* splitAt( double t );


	/**
	 * Calculates height of point p above line AB.
	 */
	static double height(
		const KoPoint& a,
		const KoPoint& p,
		const KoPoint& b );


	/**
	 * Calculates whether lines A0A1 and B0B1 intersect.
	 */
	static bool linesIntersect(
		const KoPoint& a0,
		const KoPoint& a1,
		const KoPoint& b0,
		const KoPoint& b1 );

	/**
	 * Returns true, if this segment intersects the other segment.
	 */
	bool intersects( const VSegment& segment ) const;


	/**
	 * Returns a number > 0 if the point p is left, 0 if it's on and
	 * a number < 0 if it's right of the infinite line through the
	 * previous segment's knot and the current knot.
	 */
	double pointIsLeft( const KoPoint& p ) const
	{
		return
			( knot().x() - prev()->knot().x() ) *
			( p.y() - prev()->knot().y() )
			-
			( p.x() - prev()->knot().x() ) *
			( knot().y() - prev()->knot().y() );
	}

	/**
	 * Calculates the bounding box.
	 */
	KoRect boundingBox() const;


	void draw( VPainter* painter ) const;

	// TODO: remove this backward compatibility function after koffice 1.3.x.
	void load( const QDomElement& element );


	/**
	 * Returns a pointer to a copy of this segment.
	 */
	VSegment* clone() const;

private:
	/**
	 * Calculates the solutions of y(x) = 0 where 0 <= x <= 1. The
	 * returned parameters are not ordered.
	 */
	void rootParams( Q3ValueList<double>& params ) const;

	/**
	 * Calculates how often the control polygon crosses the x-axis.
	 */
	int controlPolygonZeros() const;


	/**
	 * The segment degree. For (cubic) beziers "three", "one" for lines.
	 */
	unsigned short m_degree : 6;

	/**
	 * The segment state.
	 */
	VState m_state : 2;

	/**
	 * Node data.
	 */
	struct VNodeData
	{
		KoPoint m_vector;
		bool m_isSelected;
	};

	/**
	 * A pointer to an array of node data.
	 */
	VNodeData* m_nodes;


	/**
	 * Pointer to the previous segment.
	 */
	VSegment* m_prev;

	/**
	 * Pointer to the next segment.
	 */
	VSegment* m_next;
};

#endif

