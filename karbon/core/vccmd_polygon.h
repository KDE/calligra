/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDPOLYGON_H__
#define __VCCMDPOLYGON_H__

#include "vcommand.h"

// create a polygon-shape.

class VPath;

class VCCmdPolygon : public VCommand
{
public:
	VCCmdPolygon( KarbonPart* part, const double center_x, const double center_y,
		const double radius, const uint edges );
	virtual ~VCCmdPolygon() {}

	virtual void execute();
	virtual void unexecute();

private:
	VPath* m_object;
	double m_centerX;
	double m_centerY;
	double m_radius;
	uint m_edges;
};

#endif
