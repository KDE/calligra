/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMCMDPOLYGONIZE_H__
#define __VMCMDPOLYGONIZE_H__

#include "vcommand.h"


class VPath;

class VMCmdPolygonize : public VCommand
{
public:
	VMCmdPolygonize( KarbonPart* part, const VObjectList& objects,
		double flatness );
	virtual ~VMCmdPolygonize() {}

	virtual void execute();
	virtual void unexecute();

private:
	VObjectList m_objects;
	double m_flatness;
};

#endif
