/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VPATH_H__
#define __VPATH_H__

#include <qptrlist.h>
#include <qpointarray.h>

#include "vobject.h"

class QWMatrix;
class KoPoint;

class VSegment;

class VPath : public VObject
{
public:
	VPath();
	VPath( const VPath& path );
	virtual ~VPath();

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor = 1.0 );

	const KoPoint& currentPoint() const;

	// postscript-like commands:
	VPath& moveTo( const double& x, const double& y );
	VPath& lineTo( const double& x, const double& y );

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
		const double& x1, const double& y1,
		const double& x2, const double& y2,
		const double& x3, const double& y3 );

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
		const double& x2, const double& y2,
		const double& x3, const double& y3 );

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
		const double& x1, const double& y1,
		const double& x3, const double& y3 );

	// this is a convenience function to approximate circular arcs with
	// beziers
	VPath& arcTo(
		const double& x1, const double& y1,
		const double& x2, const double& y2, const double& r );

	VPath& close();
	bool isClosed() const;

	// return a reverted path:
	VPath* revert() const;

	// perform a boolean operation (unite(0), intersect(1), substract(2), xor(3)):
	VPath* booleanOp( const VPath* path, int type = 0 ) const;

	// apply an affine map:
	virtual VObject& transform( const QWMatrix& m );

private:
	QPtrList<VSegment> m_segments;
};

#endif
