/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDSPIRAL_H__
#define __VCCMDSPIRAL_H__

#include "vccommand.h"

// create a spiral-shape.

class VPath;

class VCCmdSpiral : public VCCommand
{
public:
	VCCmdSpiral( KarbonPart* part, const double centerX, const double centerY,
		const double radius, const uint segments, const double fade, const bool cw );
 	virtual ~VCCmdSpiral() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VPath* createPath();

private:
	double m_centerX;
	double m_centerY;
	double m_radius;
	uint m_segments;
	double m_fade;
	bool m_clockWise;
};

#endif
