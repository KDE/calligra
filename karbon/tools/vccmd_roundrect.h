/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDROUNDRECT_H__
#define __VCCMDROUNDRECT_H__

#include "vcommand.h"

// create a round rectangle-shape.

class VPath;

class VCCmdRoundRect : public VCommand
{
public:
	VCCmdRoundRect( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY, const double edgeR );
	virtual ~VCCmdRoundRect() {}

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
	double m_edgeR;
};

#endif
