/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSPIRAL_H__
#define __VSPIRAL_H__

#include "vpath.h"

class VSpiral : public VPath
{
public:
	VSpiral( VObject* parent,
		const KoPoint& center, double radius, uint segments,
		double fade, bool clockwise, double angle = 0.0 );
};

#endif

