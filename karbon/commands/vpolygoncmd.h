/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGONCMD_H__
#define __VPOLYGONCMD_H__

#include "vshapecmd.h"

// create a polygon-shape.

class VPolygonCmd : public VShapeCmd
{
public:
	VPolygonCmd( VDocument *doc, double centerX, double centerY,
		double radius, uint edges, double angle = 0.0 );
	virtual ~VPolygonCmd() {}

	virtual VShape* createPath();

private:
	double m_centerX;
	double m_centerY;
	double m_radius;
	uint m_edges;
	double m_angle;
};

#endif

