/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTNODES_H__
#define __VSELECTNODES_H__


#include <qptrlist.h>

#include "koPoint.h"
#include "koRect.h"

#include "vvisitor.h"


class VSegment;


class VSelectNodes : public VVisitor
{
public:
	VSelectNodes( bool select = true )
	{
		m_allNodes = true;
		m_select = select;
	}

	VSelectNodes( const KoPoint& point, bool select = true,
		double isNearRange = VGlobal::isNearRange )
	{
		m_allNodes = false;
		m_select = select;
		m_isNearRange = isNearRange;
		m_point = point;
	}

	VSelectNodes( const KoRect& rect, bool select = true,
		double isNearRange = VGlobal::isNearRange )
	{
		m_allNodes = false;
		m_select = select;
		m_isNearRange = isNearRange;
		m_rect = rect;
	}

	virtual void visitVSegmentList( VSegmentList& segmentList );

	QPtrList<VSegment>& result() { return m_segments; }

private:
	bool m_allNodes;
	bool m_select;
	double m_isNearRange;

	KoPoint m_point;
	KoRect m_rect;
	QPtrList<VSegment> m_segments;
};

#endif

