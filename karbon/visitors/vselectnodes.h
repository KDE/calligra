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
protected:
	VSelectNodes()
		{ m_deselect = true; }

public:
	VSelectNodes( const KoPoint& point )
		{ m_deselect = false; m_point = point; }
	VSelectNodes( const KoRect& rect )
		{ m_deselect = false; m_rect = rect; }

	virtual void visitVSegmentList( VSegmentList& segmentList );

	QPtrList<VSegment>& result() { return m_segments; }

private:
	bool m_deselect;
	KoPoint m_point;
	KoRect m_rect;
	QPtrList<VSegment> m_segments;
};


class VDeselectNodes : public VSelectNodes
{
	VDeselectNodes() : VSelectNodes() {}
};

#endif

