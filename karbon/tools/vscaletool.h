/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSCALETOOL_H__
#define __VSCALETOOL_H__


#include "vselection.h"
#include "vtool.h"


class VScaleTool : public VTool
{
public:
	VScaleTool( KarbonView* view );
	virtual ~VScaleTool();

	virtual void activate();

	virtual bool eventFilter( QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject();

protected:
	void setCursor( const QPoint & ) const;

private:
	// input (mouse coordinates):
	KoPoint m_fp;
	KoPoint m_lp;

	double m_s1;
	double m_s2;
	KoPoint m_sp;

	VHandleNode m_activeNode;

	bool m_isDragging;
};

#endif

