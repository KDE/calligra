/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VZORDERCMD_H__
#define __VZORDERCMD_H__

#include "vcommand.h"


class VSelection;


class VZOrderCmd : public VCommand
{
public:
	enum VOrder
	{
		bringToFront	= 0,
		up				= 1,
		down			= 2,
		sendToBack		= 3
	};

	VZOrderCmd( VDocument* doc, VOrder );
	virtual ~VZOrderCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;
	VOrder m_state;
};

#endif

