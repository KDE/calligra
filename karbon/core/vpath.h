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
#include "vsegmentlist.h"

class QDomElement;
class QWMatrix;
class VPainter;

class VPath : public VObject
{
public:
	VPath();
	VPath( const VPath& path );
	virtual ~VPath();

	const KoPoint& currentPoint() const;

	// postscript-like commands:
	bool moveTo( const KoPoint& p );
	bool lineTo( const KoPoint& p );

	/*
	curveTo():

	   p1          p2
	    O   ____   O
	    : _/    \_ :
	    :/        \:
	    x          x
	currP          p3
	*/

	bool curveTo(
		const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );

	/*
	curve1To():

	               p2
	         ____  O
	      __/    \ :
	     /        \:
	    x          x
	currP          p3
	*/

	bool curve1To( const KoPoint& p2, const KoPoint& p3 );

	/*
	curve2To():

	   p1
	    O  ____
	    : /    \__
	    :/        \
	    x          x
	currP          p3
	*/

	bool curve2To( const KoPoint& p1, const KoPoint& p3 );

	// this is a convenience function to approximate circular arcs with
	// beziers. input: 2 tangent vectors and a radius (same as in postscript):

	bool arcTo(
		const KoPoint& p1, const KoPoint& p2, double r );

	void close();
	bool isClosed() const;

	// perform a boolean operation (unite(0), intersect(1), substract(2), xor(3)):
	VPath* booleanOp( const VPath* path, int type = 0 ) const;

	void combine( const VPath& path );
	void combineSegmentList( const VSegmentList& segmentList );

	virtual void draw( VPainter *painter, const QRect& rect,
		const double zoomFactor = 1.0 );

	const VSegment* lastSegment() const
		{ return m_segmentLists.getLast()->getLast(); }

	// apply an affine map:
	virtual void transform( const QWMatrix& m );

	virtual QRect boundingBox( const double zoomFactor ) const;
	virtual bool intersects( const QRect& qrect, const double zoomFactor ) const;

	virtual VObject* clone();

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

private:
	QPtrList<VSegmentList> m_segmentLists;		// list of segmentList
};

#endif
