/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTTOOL_H__
#define __VSELECTTOOL_H__

#include "vtool.h"


class VSelectTool : public VTool
{
public:
	VSelectTool( KarbonView* view );
	virtual ~VSelectTool();

	virtual void activate();

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

private:
	enum { normal, moving, scaling, rotating } m_state;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	bool m_isDragging;
};

#endif

