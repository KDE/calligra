/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VELLIPSECMD_H__
#define __VELLIPSECMD_H__

#include "vshapecmd.h"

// create a ellipse-shape.

class VEllipseCmd : public VShapeCmd
{
public:
	VEllipseCmd( VDocument *part, const double tlX, const double tlY,
		 const double brX, const double brY );
	virtual ~VEllipseCmd() {}

	virtual VObject* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
};

#endif

