/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGRADIENTTOOL_H__
#define __VGRADIENTTOOL_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to scale object(s)

class VGradientTool : public VTool
{
public:
	virtual ~VGradientTool();
	static VGradientTool* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VGradientTool( KarbonPart* part );

private:
	static VGradientTool* s_instance;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	bool m_isDragging;
};

#endif

