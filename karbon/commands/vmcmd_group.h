/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMCMDGROUP_H__
#define __VMCMDGROUP_H__

#include "vcommand.h"


// Group object(s)

class VGroup;

class VMCmdGroup : public VCommand
{
public:
	VMCmdGroup( KarbonPart* part );
	virtual ~VMCmdGroup() {}

	virtual void execute();
	virtual void unexecute();

private:
	VObjectList m_objects;
	VGroup *m_group;
};

#endif
