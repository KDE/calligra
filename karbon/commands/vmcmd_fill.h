/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMCMDFILL_H__
#define __VMCMDFILL_H__

#include "vcommand.h"
#include "vcolor.h"
#include "vfill.h"

#include <qvaluevector.h>

// Fill object(s)

class VPath;

class VMCmdFill : public VCommand
{
public:
	VMCmdFill( KarbonPart* part, const VColor &color, float opacity = -1 );
	virtual ~VMCmdFill() {}

	virtual void execute();
	virtual void unexecute();

private:
	VObjectList m_objects;
	VColor m_color;
	QValueVector<VFill> m_oldcolors;
	float m_opacity;
};

#endif
