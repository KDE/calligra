/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMOVESELECTIONCMD_H__
#define __VMOVESELECTIONCMD_H__

#include "vcommand.h"

class VSelection;

// 

class VOrderSelection;

class VOrderSelectionCmd : public VCommand
{
public:
	enum VOrder
	{
		bringtofront	= 0,
		up				= 1,
		down			= 2,
		sendtoback		= 3
	};

	VOrderSelectionCmd( VDocument *doc, VOrder );
	virtual ~VOrderSelectionCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;
	VOrder m_state;
};

#endif

