/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGON_H__
#define __VPOLYGON_H__

#include "vpath.h"

class VPolygon : public VPath
{
public:
	VPolygon( VObject* parent,
		const KoPoint& center, double radius, uint edges, double angle );
};

#endif

