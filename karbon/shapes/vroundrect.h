/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDRECT_H__
#define __VROUNDRECT_H__

#include "vpath.h"

class VRoundRect : public VPath
{
public:
	VRoundRect( VObject* parent,
		const KoPoint& topLeft, const KoPoint& bottomRight, double edgeRadius );
};

#endif

