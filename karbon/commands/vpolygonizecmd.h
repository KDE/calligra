/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGONIZECMD_H__
#define __VPOLYGONIZECMD_H__

#include "vcommand.h"


class VPolygonizeCmd : public VCommand
{
public:
	VPolygonizeCmd( VDocument *doc, double flatness );
	virtual ~VPolygonizeCmd() {}

	virtual void execute();
	virtual void unexecute();

private:
	VSelection m_objects;

	double m_flatness;
};

#endif

