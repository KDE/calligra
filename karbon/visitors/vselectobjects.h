/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTOBJECTS_H__
#define __VSELECTOBJECTS_H__


#include "koRect.h"

#include "vgroup.h"
#include "vpath.h"
#include "vvisitor.h"


class VSelectObjects : public VVisitor
{
public:
	VSelectObjects( VObjectList& selection, bool select = true )
		: m_selection( selection )
	{
		m_select = select;
	}

	VSelectObjects( VObjectList& selection, const KoRect& rect, bool select = true )
		: m_selection( selection )
	{
		m_select = select;
		m_rect = rect;
	}

	virtual void visitVGroup( VGroup& group )
		{ visitVObject( group ); }
	virtual void visitVPath( VPath& path )
		{ visitVObject( path ); }

private:
	void visitVObject( VObject& object );

	VObjectList& m_selection;

	bool m_select;

	KoRect m_rect;
};

#endif

