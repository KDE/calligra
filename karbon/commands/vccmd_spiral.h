/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDSPIRAL_H__
#define __VCCMDSPIRAL_H__

#include "vccommand.h"

// create a spiral-shape.

class VObject;

class VCCmdSpiral : public VCCommand
{
public:
	VCCmdSpiral( KarbonPart* part, double centerX, double centerY,
		double radius, uint segments, double fade, bool cw = true, double angle = 0.0 );
 	virtual ~VCCmdSpiral() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VObject* createPath();

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
