/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTNODES_H__
#define __VSELECTNODES_H__


#include "koRect.h"

#include "vvisitor.h"


class VSelectNodes : public VVisitor
{
public:
	VSelectNodes( bool select = true )
	{
		m_select = select;
	}

	VSelectNodes( const KoRect& rect, bool select = true )
	{
		m_select = select;
		m_rect = rect;
	}

	virtual void visitVSegmentList( VSegmentList& segmentList );

private:
	bool m_select;

	KoRect m_rect;
};

#endif

