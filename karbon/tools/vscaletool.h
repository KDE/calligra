/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSCALETOOL_H__
#define __VSCALETOOL_H__

#include "vtool.h"
#include "vselection.h"

class KarbonPart;
class KarbonView;

// A singleton state to scale object(s)

class VScaleTool : public VTool
{
public:
	virtual ~VScaleTool();
	static VScaleTool* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VScaleTool( KarbonPart* part );

	void setCursor( KarbonView* view, const QPoint & ) const;

private:
	static VScaleTool* s_instance;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	double m_s1, m_s2;
	KoPoint m_sp;

	VHandleNode m_activeNode;

	bool m_isDragging;
};

#endif

