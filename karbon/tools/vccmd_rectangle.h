/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDRECTANGLE_H__
#define __VCCMDRECTANGLE_H__

#include "vcommand.h"

// create a rectangle-shape.

class VPath;

class VCCmdRectangle : public VCommand
{
public:
	VCCmdRectangle( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY );
	virtual ~VCCmdRectangle() {}

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
