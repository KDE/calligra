/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VINSERTKNOTSCMD_H__
#define __VINSERTKNOTSCMD_H__

#include "vcommand.h"


class VSelection;


class VInsertKnotsCmd : public VCommand
{
public:
	VInsertKnotsCmd( VDocument *doc, uint knots );
	virtual ~VInsertKnotsCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	uint m_knots;
};

#endif

