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
	VCCmdRectangle( KarbonPart* part, const double tl_x, const double tl_y,
		 const double br_x, const double br_y );
	virtual ~VCCmdRectangle() {}

	virtual void execute();
	virtual void unexecute();

private:
	VPath* m_object;
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
};

#endif
