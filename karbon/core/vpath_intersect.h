/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHINTERSECT_H__
#define __VPATHINTERSECT_H__

#include <qvaluelist.h>

#include "vsegment.h"

class VPath;

// calculate intersection points of two vpaths.

class VPathIntersect : public VSegmentListTraverser
{
public:
	VPathIntersect();

	VSegmentList* intersect( const VSegmentList& list1, const VSegmentList& list2 );

	// "begin" is special: we dont intersect a "moveto".
	virtual bool begin( const KoPoint& p );
	virtual bool curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3 );
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3 );
	virtual bool lineTo( const KoPoint& p );
	virtual bool end( const KoPoint& p );

	const KoPoint& previousPoint2() const { return m_previousPoint2; }
	void setPreviousPoint2( const KoPoint& p ) { m_previousPoint2 = p; }

private:
	VPath* m_path;
	bool m_currentListisList1;

	VSegmentList m_list1;
	VSegmentList m_list2;

	QValueList<double> m_params1;	// intersection parameters for segment1.
	QValueList<double> m_params2;	// intersection parameters for segment2.

	KoPoint m_previousPoint2;	// additional to m_previousPoint
};

#endif
