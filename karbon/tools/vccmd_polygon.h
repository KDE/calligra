/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDPOLYGON_H__
#define __VCCMDPOLYGON_H__

#include "vccommand.h"

// create a polygon-shape.

class VPath;

class VCCmdPolygon : public VCCommand
{
public:
	VCCmdPolygon( KarbonPart* part, double centerX, double centerY,
		double radius, uint edges, double angle = 0.0 );
	virtual ~VCCmdPolygon() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VPath* createPath();

private:
	double m_centerX;
	double m_centerY;
	double m_radius;
	uint m_edges;
	double m_angle;
};

#endif
