/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLSELECT_H__
#define __VMTOOLSELECT_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to select object(s)

class VMToolSelect : public VTool
{
public:
	virtual ~VMToolSelect();
	static VMToolSelect* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VMToolSelect( KarbonPart* part );

private:
	static VMToolSelect* s_instance;
	enum { normal, moving, scaling, rotating } m_state;

	// input (mouse coordinates):
	QPoint m_fp;
	QPoint m_lp;

	bool m_isDragging;
};

#endif
