/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VDELETECMD_H__
#define __VDELETECMD_H__

#include "vcommand.h"


class VSelection;


/**
 * A class to provide undo/redoable deletion of VObjects.
 */

class VDeleteCmd : public VCommand
{
public:
	VDeleteCmd( VDocument *part );
	virtual ~VDeleteCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;
};

#endif

