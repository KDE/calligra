/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vtool.h"

VTool::VTool( KarbonView* view )
	: m_view( view )
{
}

bool
VTool::eventFilter( QEvent* /*event*/ )
{
	return false;
}

