/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDSINUS_H__
#define __VCCMDSINUS_H__

#include "vcommand.h"

// create a sinus curve-shape.

class VPath;

class VCCmdSinus : public VCommand
{
public:
	VCCmdSinus( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY, const uint periods = 1 );
	virtual ~VCCmdSinus() {}

	virtual void execute();
	virtual void unexecute();

private:
	VPath* m_object;
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
	uint m_periods;
};

#endif
