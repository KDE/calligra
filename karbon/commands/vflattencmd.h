/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFLATTENCMD_H__
#define __VFLATTENCMD_H__

#include "vcommand.h"


class VSelection;


class VFlattenCmd : public VCommand
{
public:
	VFlattenCmd( VDocument *doc, double flatness );
	virtual ~VFlattenCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	double m_flatness;
};

#endif

