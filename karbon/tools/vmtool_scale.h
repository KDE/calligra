/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLSCALE_H__
#define __VMTOOLSCALE_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to select object(s)

class VMToolScale : public VTool
{
public:
	virtual ~VMToolScale();
	static VMToolScale* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VMToolScale( KarbonPart* part );

private:
	static VMToolScale* s_instance;

	// input (mouse coordinates):
	QPoint m_fp;
	QPoint m_lp;

	double m_s1, m_s2;

	bool m_isDragging;
};

#endif
