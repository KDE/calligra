/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VMTOOLSELECT_H__
#define __VMTOOLSELECT_H__

#include "vtool.h"

class KarbonPart;

// A singleton state to select object(s)

class VMToolSelect : public VTool
{
public:
	virtual ~VMToolSelect();
	static VMToolSelect* instance( KarbonPart* part );

	virtual VCommand* createCmdFromDialog( const QPoint& point );
	virtual VCommand* createCmdFromDragging( const QPoint& tl, const QPoint& br );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& tl, const QPoint& br );

protected:
	VMToolSelect( KarbonPart* part );

private:
	static VMToolSelect* s_instance;
};

#endif
