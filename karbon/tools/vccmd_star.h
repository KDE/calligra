/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDSTAR_H__
#define __VCCMDSTAR_H__

#include "vccommand.h"

// create a star-shape.

class VObject;

class VCCmdStar : public VCCommand
{
public:
	VCCmdStar( KarbonPart* part, double centerX, double centerY,
		double outerR, double innerR, uint edges, double angle = 0.0 );
	virtual ~VCCmdStar() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VObject* createPath();

private:
	double m_centerX;
	double m_centerY;
	double m_outerR;
	double m_innerR;
	uint m_edges;
	double m_angle;
};

#endif
