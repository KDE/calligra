/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLHANDLE_H__
#define __VMTOOLHANDLE_H__

#include "vtool.h"

class QPainter;
class KarbonPart;

// A singleton state to represent a handle.

class VMToolHandle : public VTool
{
public:
	virtual ~VMToolHandle();
	static VMToolHandle* instance( KarbonPart* part );

	void draw( QPainter& painter, const double zoomFactor );

	virtual bool eventFilter( KarbonView* view, QEvent* event );

protected:
	VMToolHandle( KarbonPart* part );

private:
	static VMToolHandle* s_instance;
};

#endif
