/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTAR_H__
#define __VSTAR_H__

#include "vpath.h"

class VStar : public VPath
{
public:
	VStar( VObject* parent,
		const KoPoint& center, double outerRadius, double innerRadius,
		uint edges, double angle );
};

#endif

