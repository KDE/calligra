/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDSINUS_H__
#define __VCCMDSINUS_H__

#include "vccommand.h"

// create a sinus curve-shape.

class VPath;

class VCCmdSinus : public VCCommand
{
public:
	VCCmdSinus( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY, const uint periods = 1 );
	virtual ~VCCmdSinus() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VPath* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
	uint m_periods;
};

#endif
