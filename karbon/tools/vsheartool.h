/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHEARTOOL_H__
#define __VSHEARTOOL_H__

#include "vtool.h"
#include "vselection.h"

class KarbonPart;
class KarbonView;

// A singleton state to scale object(s)

class VShearTool : public VTool
{
public:
	virtual ~VShearTool();
	static VShearTool* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VShearTool( KarbonPart* part );

	void setCursor( KarbonView* view, const QPoint & ) const;

private:
	static VShearTool* s_instance;

	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	double m_s1, m_s2;

	VHandleNode m_activeNode;

	bool m_isDragging;
};

#endif

