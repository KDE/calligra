/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VNODESELECTOR_H__
#define __VNODESELECTOR_H__


#include "vvisitor.h"
#include "koPoint.h"
#include <qptrlist.h>

class VSegment;


class VNodeSelector : public VVisitor
{
public:
	VNodeSelector( const KoPoint &point ) { m_all = false; m_point = point; }
	VNodeSelector() { m_all = true; }

	virtual void visitVSegmentList( VSegmentList& segmentList );

	QPtrList<VSegment> &result() { return m_segments; }
private:
	bool m_all;
	KoPoint m_point;
	QPtrList<VSegment> m_segments;
};

#endif

