/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILLCMD_H__
#define __VFILLCMD_H__

#include "vcommand.h"
#include "vfill.h"

#include <qvaluevector.h>


class VSelection;


// Fill object(s)

class VPath;

class VFillCmd : public VCommand
{
public:
	VFillCmd( VDocument *doc, const VFill & );
	virtual ~VFillCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	VFill m_fill;

	QValueVector<VFill> m_oldcolors;
};

#endif

