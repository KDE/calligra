/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDROUNDRECT_H__
#define __VCCMDROUNDRECT_H__

#include "vccommand.h"

// create a round rectangle-shape.

class VObject;

class VCCmdRoundRect : public VCCommand
{
public:
	VCCmdRoundRect( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY, const double edgeR );
	virtual ~VCCmdRoundRect() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VObject* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
	double m_edgeR;
};

#endif
