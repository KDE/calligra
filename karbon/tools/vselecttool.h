/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTTOOL_H__
#define __VSELECTTOOL_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to select object(s)

class VSelectTool : public VTool
{
public:
	virtual ~VSelectTool();
	static VSelectTool* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VSelectTool( KarbonPart* part );

private:
	static VSelectTool* s_instance;
	enum { normal, moving, scaling, rotating } m_state;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	bool m_isDragging;
};

#endif

