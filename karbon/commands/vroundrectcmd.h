/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDRECTCMD_H__
#define __VROUNDRECTCMD_H__

#include "vshapecmd.h"

// create a round rectangle-shape.

class VRoundRectCmd : public VShapeCmd
{
public:
	VRoundRectCmd( VDocument *doc, const double tlX, const double tlY,
		 const double brX, const double brY, const double edgeR );
	virtual ~VRoundRectCmd() {}

	virtual VObject* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
	double m_edgeR;
};

#endif

