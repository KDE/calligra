/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGROUPCMD_H__
#define __VGROUPCMD_H__

#include "vcommand.h"


// Group object(s)

class VGroup;

class VGroupCmd : public VCommand
{
public:
	VGroupCmd( VDocument *doc );
	virtual ~VGroupCmd() {}

	virtual void execute();
	virtual void unexecute();

private:
	VSelection m_objects;

	VGroup* m_group;
};

#endif

