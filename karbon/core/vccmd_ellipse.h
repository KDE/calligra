/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCCMDELLIPSE_H__
#define __VCCMDELLIPSE_H__

#include "vcommand.h"

// create a ellipse-shape.

class VPath;

class VCCmdEllipse : public VCommand
{
public:
	VCCmdEllipse( KarbonPart* part, const double tlX, const double tlY,
		 const double brX, const double brY );
	virtual ~VCCmdEllipse() {}

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
