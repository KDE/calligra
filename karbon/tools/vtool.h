/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__

#include <koPoint.h>

class KarbonView;
class QEvent;
class VCommand;
class QMouseEvent;

class VTool
{
public:
	VTool( KarbonView* view );

	virtual void activate() {}
	virtual void deactivate() {}

	virtual bool eventFilter( QEvent* event );

	KarbonView* view() const { return m_view; }

protected:
	virtual void mousePressed( QMouseEvent * ) {}
	virtual void mouseMoved( QMouseEvent * ) {}
	virtual void dragShiftPressed() {}
	virtual void dragCtrlPressed() {}
	virtual void dragShiftReleased() {}
	virtual void dragCtrlReleased() {}
	virtual void dragAltPressed() {}
	virtual void mouseReleased( QMouseEvent * ) {}
	virtual void cancel() {} // ESC pressed

	virtual void drawTemporaryObject() = 0;

	// make vtool "abstract":
	virtual ~VTool() {}

	bool m_isDragging;

	// input (mouse coordinates):
	KoPoint m_lp;
	KoPoint m_fp;

private:
	// that's our view:
	KarbonView* m_view;
};

#endif

