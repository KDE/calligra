/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSINUSCMD_H__
#define __VSINUSCMD_H__

#include "vshapecmd.h"

// create a sinus curve-shape.

class VObject;

class VSinusCmd : public VShapeCmd
{
public:
	VSinusCmd( VDocument *doc, const double tlX, const double tlY,
		 const double brX, const double brY, const uint periods = 1 );
	virtual ~VSinusCmd() {}

	virtual VShape* createPath();

private:
	double m_tlX;
	double m_tlY;
	double m_brX;
	double m_brY;
	uint m_periods;
};

#endif

