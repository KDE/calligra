/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMCMDINSERTKNOTS_H__
#define __VMCMDINSERTKNOTS_H__

#include "vcommand.h"


class VPath;

class VMCmdInsertKnots : public VCommand
{
public:
	VMCmdInsertKnots( KarbonPart* part, const VObjectList& objects,
		uint knots );
	virtual ~VMCmdInsertKnots() {}

	virtual void execute();
	virtual void unexecute();

private:
	VObjectList m_objects;
	uint m_knots;
};

#endif
