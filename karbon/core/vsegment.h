/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENT_H__
#define __VSEGMENT_H__

#include <qptrlist.h>
#include <koPoint.h>

class QDomElement;

// all classes which have to traverse a segment list should derive from this class.
// it hides the implementation details of segments.

class VSegment;
class VSegmentList;

class VSegmentListTraverser
{
public:
	VSegmentListTraverser();
	virtual ~VSegmentListTraverser() {}

	bool traverse( const VSegmentList& list );

	// if one of these operations should fail, it returns false.
	virtual bool begin( const KoPoint& p ) = 0;
	virtual bool curveTo( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 ) = 0;
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3 ) = 0;
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3 ) = 0;
	virtual bool lineTo( const KoPoint& p ) = 0;
	virtual bool end( const KoPoint& p ) = 0;

	const KoPoint& previousPoint() const { return m_previousPoint; }
	void setPreviousPoint( const KoPoint& p ) { m_previousPoint = p; }

private:
	KoPoint m_previousPoint;
};


// line- and all the various other (bezier-)segment types are so similar in practise
// that we only need to have one segment-class.
// this leads to a waste of KoPoints for e.g. line-segments. otoh memory is cheap
// and human resources are expensive. ;)

class VSegment
{
public:
	VSegment();
	VSegment( const VSegment &other );

	// segment types:
	enum VSegmentType{
		begin,			// initial moveto
		curve,			// curveto (bezier)
		curve1,			// + first ctrl-point is identical to first knot.
		curve2,			// + last ctrl-point is identical to last knot.
		line,			// lineto
		end
	};
	
	VSegmentType type() const { return m_type; }
	void setType( VSegmentType t ) { m_type = t; }

	const KoPoint& point( uint i ) const
	{
		if( i > 0 && i < 4 )	// index 1..3 feel more natural than 0..2
			return m_point[--i];
		else
			return m_point[2];
	}

	void setPoint( uint i, const KoPoint& p )
		{ if( i > 0 && i < 4 ) m_point[--i] = p; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VSegmentType m_type;

	bool m_smooth;			// first ctrl-point is "smooth".

	KoPoint m_point[3];
};

#endif
