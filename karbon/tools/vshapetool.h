/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPETOOL_H__
#define __VSHAPETOOL_H__

#include "vtool.h"
#include "vglobal.h"

class QEvent;

class KarbonPart;

class VShapeTool : public VTool
{
public:
	VShapeTool( KarbonPart* part = 0L, bool polar = false );
	virtual ~VShapeTool() {}

	virtual bool eventFilter( KarbonView* view, QEvent* event );
};

#endif
