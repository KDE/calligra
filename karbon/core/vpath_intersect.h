/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATHINTERSECT_H__
#define __VPATHINTERSECT_H__

#include <qvaluelist.h>

#include "vsegment.h"

class VPath;

// calculate intersection points of two vpaths.
// the traverser abstraction doesnt work quite well here, since we need insider
// knowledge about segments at various places. otherwise we would have to copy
// segment datas around everywhere.

class VPathIntersect : public VSegmentListTraverser
{
public:
	VPathIntersect();

	VSegmentList* intersect( const VSegmentList& list1, const VSegmentList& list2 );

	virtual bool begin( const KoPoint& p,
		const VSegment& segment );
	virtual bool curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3,
		const VSegment& segment );
	virtual bool curve1To( const KoPoint& p2, const KoPoint& p3,
		const VSegment& segment );
	virtual bool curve2To( const KoPoint& p1, const KoPoint& p3,
		const VSegment& segment );
	virtual bool lineTo( const KoPoint& p,
		const VSegment& segment );
	virtual bool end( const KoPoint& p,
		const VSegment& segment );

	const KoPoint& previousPoint2() const { return m_previousPoint2; }
	void setPreviousPoint2( const KoPoint& p ) { m_previousPoint2 = p; }

private:
	VPath* m_path;
	const VSegment* m_segment1; 	// current segment from list1.
	const VSegment* m_segment2; 	// current segment from list2.
	QValueList<double> m_params1;	// intersection parameters for segment1.
	QValueList<double> m_params2;	// intersection parameters for segment2.

	bool m_isList1;			// current "path == list1"?

	VSegmentList* m_list2;
	KoPoint m_previousPoint2;
};

#endif
