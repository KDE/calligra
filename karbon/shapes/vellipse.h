/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VELLIPSE_H__
#define __VELLIPSE_H__

#include "vpath.h"

class VEllipse : public VPath
{
public:
	VEllipse( VObject* parent,
		const KoPoint& topLeft, double width, double height );
};

#endif

