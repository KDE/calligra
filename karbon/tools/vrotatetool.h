/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROTATETOOL_H__
#define __VROTATETOOL_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to rotate object(s)

class VRotateTool : public VTool
{
public:
	virtual ~VRotateTool();
	static VRotateTool* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VRotateTool( KarbonPart* part );

	void setCursor( KarbonView* view, const QPoint & ) const;

private:
	static VRotateTool* s_instance;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;
	KoPoint m_sp;

	double m_angle;

	bool m_isDragging;
};

#endif

