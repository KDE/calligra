/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qevent.h>

#include "vctool_rectangle.h"

VCToolRectangle* VCToolRectangle::s_instance = 0L;

VCToolRectangle::VCToolRectangle( KarbonPart* part )
	: m_part( part )
{
}

VCToolRectangle*
VCToolRectangle::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolRectangle( part );
	}
	return s_instance;
}

bool
VCToolRectangle::eventFilter( QEvent* event )
{
	if ( event->type() == QEvent::MouseButtonPress )
	{
		return true;
	}
	else
		return false;
}
