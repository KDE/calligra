/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMCMDSTROKE_H__
#define __VMCMDSTROKE_H__

#include "vcommand.h"
#include "vcolor.h"

// Stroke object(s)

class VPath;

class VMCmdStroke : public VCommand
{
public:
	VMCmdStroke( KarbonPart* part, const VColor &color );
	virtual ~VMCmdStroke() {}

	virtual void execute();
	virtual void unexecute();

private:
	VObjectList m_objects;
	VColor m_color;
};

#endif
