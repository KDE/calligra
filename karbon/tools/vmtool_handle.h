/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMTOOLHANDLE_H__
#define __VMTOOLHANDLE_H__

#include "vtool.h"

class KarbonPart;

// A singleton state to represent a handle.

class VMToolHandle : public VTool
{
public:
	virtual ~VMToolHandle();
	static VMToolHandle* instance( KarbonPart* part );

	virtual VCommand* createCmd( double x, double y, double d1, double d2 );

	virtual void drawTemporaryObject(
		KarbonView* view, const QPoint& p, double d1, double d2 );

protected:
	VMToolHandle( KarbonPart* part );

private:
	static VMToolHandle* s_instance;
};

#endif
