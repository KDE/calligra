/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VRECTANGLE_H__
#define __VRECTANGLE_H__

#include "vpath.h"

class VRectangle : public VPath
{
public:
	VRectangle( VObject* parent,
		const KoPoint& topLeft, double width, double height );
};

#endif

