/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSINUS_H__
#define __VSINUS_H__

#include "vpath.h"

class VSinus : public VPath
{
public:
	VSinus( VObject* parent,
		const KoPoint& topLeft, double width, double height, uint periods );
};

#endif

