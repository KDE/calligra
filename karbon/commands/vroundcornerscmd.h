/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDCORNERSCMD_H__
#define __VROUNDCORNERSCMD_H__

#include "vcommand.h"


class VSelection;


class VRoundCornersCmd : public VCommand
{
public:
	VRoundCornersCmd( VDocument *doc, double radius );
	virtual ~VRoundCornersCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	double m_radius;
};

#endif

