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

// VPaths are the most common high-level objects. They consist of
// VSegments, which are VLines or VBeziers.

// VSegment is the abstract base class for VLine and VBezier
class VSegment
{
public:
	VSegment() {}
	virtual ~VSegment();

	virtual const QPointArray& getQPointArray( const double& zoomFactor ) = 0;

protected:
	// m_firstPoint is actually a waste, but helps making segments "autarc"
	VPoint* m_firstPoint;
	VPoint* m_lastPoint;

	mutable QPointArray m_QPointArray;
};


class VLine : VSegment
{
public:
	VLine() {}
	~VLine() {}

	const QPointArray& getQPointArray( const double& zoomFactor ) {};
};


class VBezier : VSegment
{
public:
	VBezier() {}
	virtual ~VBezier() {}

	const QPointArray& getQPointArray( const double& zoomFactor ) {};

private:
	VPoint* m_firstCtrlPoint;
	VPoint* m_lastCtrlPoint;

	// a bezier lies completely inside the "minmax box" (this information is
	// usefull for speeding up intersection-calculations):
	VRect m_minMaxBox;
};


class VPath : public VObject
{
public:
	VPath();
	virtual ~VPath();

	virtual void draw( QPainter& painter, const QRect& rect,
		const double& zoomFactor );

	const VPoint* currentPoint() const;
	const QList<VSegment>& segments() { return m_segments; }

	// postscript-compliant commands:
	void moveTo( const double& x, const double& y );
	void rmoveTo( const double& dx, const double& dy );
	void lineTo( const double& x, const double& y );
	void rlineTo( const double& dx, const double& dy );
	void curveTo( const double& x1, const double& y1, const double& x2,
			const double& y2, const double& x3, const double& y3 );
	void rcurveTo( const double& dx1, const double& dy1, const double& dx2,
			const double& dy2, const double& dx3, const double& dy3 );
	void arcTo( const double& x1, const double& y1,
			const double& x2, const double& y2, const double& r );
	void close();
	bool isClosed() { return( m_isClosed ); }

	virtual void translate( const double& dx, const double& dy );
	virtual void rotate( const double& ang );
	virtual void mirror( const bool horiz = false, const bool verti = false );
	virtual void scale( const double& sx, const double& sy );
	virtual void shear( const double& sh, const double& sv );
	virtual void skew( const double& ang );
	virtual void apply( const VAffineMap& affmap );

private:
	// we store all used VPoints in m_pointPool. this way, we can apply
	// operations on them without having to fear dong that unwished multiple times
	// for shared points.
 	QList<VPoint>	m_pointPool;

	// m_segments store all segemnts ( lines or beziers)
	QList<VSegment>	m_segments;

	bool m_isClosed;
};

#endif
