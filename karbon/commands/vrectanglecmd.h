/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VRECTANGLECMD_H__
#define __VRECTANGLECMD_H__

#include "vshapecmd.h"

// create a rectangle-shape.

class VRectangleCmd : public VShapeCmd
{
public:
	VRectangleCmd( VDocument *doc, const double tlX, const double tlY,
		 const double brX, const double brY );
	virtual ~VRectangleCmd() {}

	virtual VObject* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
};

#endif

