/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATH_H__
#define __VPATH_H__

#include <qpointarray.h>
#include <qptrlist.h>

#include <koPoint.h>

#include "vshape.h"
#include "vsegmentlist.h"
#include "vvisitor.h"

class QDomElement;
class QWMatrix;
class VPainter;

class VPath : public VShape
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

	/**
	 * A convenience function to approximate a circular arc with a
	 * bezier curve. Input: 2 tangent vectors and a radius (same as in postscript):
	 */
	bool arcTo(
		const KoPoint& p1, const KoPoint& p2, double r );

	void close();
	bool isClosed() const;

	/**
	 * Combines two paths. For example, the letter "O" is a combination
	 * of a larger and a smaller ellipitical path.
	 */
	void combine( const VPath& path );
	void combineSegmentList( const VSegmentList& segmentList );

	virtual void draw( VPainter *painter, const KoRect& rect );

	const VSegment* lastSegment() const
		{ return m_segmentLists.getLast()->getLast(); }

	/// Applies an affine transformation.
	virtual void transform( const QWMatrix& m );

	virtual const KoRect& boundingBox() const;
	virtual bool isInside( const KoRect& rect ) const;

	virtual VShape* clone();

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual void accept( VVisitor& visitor )
		{ visitor.visitVPath( *this, m_segmentLists ); }

private:
	QPtrList<VSegmentList> m_segmentLists;		// list of segmentList
};

#endif
