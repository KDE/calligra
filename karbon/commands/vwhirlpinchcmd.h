/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VWHIRLPINCHCMD_H__
#define __VWHIRLPINCHCMD_H__

#include "vcommand.h"


class VWhirlPinchCmd : public VCommand
{
public:
	VWhirlPinchCmd( VDocument* doc,
		double angle, double pinch, double radius );
	virtual ~VWhirlPinchCmd() {}

	virtual void execute();
	virtual void unexecute();

private:
	VObjectList m_objects;
	KoPoint m_center;
	double m_angle;
	double m_pinch;
	double m_radius;
};

#endif

