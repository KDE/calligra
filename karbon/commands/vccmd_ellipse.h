/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDELLIPSE_H__
#define __VCCMDELLIPSE_H__

#include "vccommand.h"

// create a ellipse-shape.

class VObject;

class VCCmdEllipse : public VCCommand
{
public:
	VCCmdEllipse( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY );
	virtual ~VCCmdEllipse() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VObject* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
};

#endif
