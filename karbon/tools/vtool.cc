/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "karbon_part.h"
#include "vtool.h"

VTool::VTool( KarbonPart* part )
	: m_part( part )
{
}

bool
VTool::eventFilter( KarbonView* /*view*/, QEvent* /*event*/ )
{
	return false;
}

