/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGONIZECMD_H__
#define __VPOLYGONIZECMD_H__

#include "vcommand.h"


class VSelection;


class VPolygonizeCmd : public VCommand
{
public:
	VPolygonizeCmd( VDocument *doc, double flatness );
	virtual ~VPolygonizeCmd();

	virtual void execute();
	virtual void unexecute();

private:
	VSelection* m_selection;

	double m_flatness;
};

#endif

