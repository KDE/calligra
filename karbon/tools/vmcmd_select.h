/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDSELECT_H__
#define __VCCMDSELECT_H__

#include "vcommand.h"

// Select object(s)

class VPath;

class VCCmdSelect : public VCommand
{
public:
	VCCmdSelect( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY );
	virtual ~VCCmdSelect() {}

	virtual void execute();
	virtual void unexecute();

	// for complex shapes. needed to draw while creation (creation tool):
	VPath* createPath();

private:
	VPath* m_object;
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
};

#endif
