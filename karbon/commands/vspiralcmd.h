/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSPIRALCMD_H__
#define __VSPIRALCMD_H__

#include "vshapecmd.h"

// create a spiral-shape.

class VObject;

class VSpiralCmd : public VShapeCmd
{
public:
	VSpiralCmd( VDocument *doc, double centerX, double centerY,
		double radius, uint segments, double fade, bool cw = true, double angle = 0.0 );
 	virtual ~VSpiralCmd() {}

	virtual VShape* createPath();

private:
	double m_centerX;
	double m_centerY;
	double m_radius;
	uint m_segments;
	double m_fade;
	bool m_clockWise;
	double m_angle;
};

#endif

