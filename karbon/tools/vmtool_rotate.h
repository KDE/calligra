/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLROTATE_H__
#define __VMTOOLROTATE_H__

#include "vtool.h"

class KarbonPart;
class KarbonView;

// A singleton state to rotate object(s)

class VMToolRotate : public VTool
{
public:
	virtual ~VMToolRotate();
	static VMToolRotate* instance( KarbonPart* part );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

	// draw the object while it is edited:
	void drawTemporaryObject( KarbonView* view );

protected:
	VMToolRotate( KarbonPart* part );

private:
	static VMToolRotate* s_instance;

	// input (mouse coordinates):
	QPoint m_fp;
	QPoint m_lp;

	double m_angle;

	bool m_isDragging;
};

#endif
