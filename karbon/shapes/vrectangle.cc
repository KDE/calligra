/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include "vrectangle.h"


VRectangle::VRectangle( VObject* parent,
		const KoPoint& topLeft, const KoPoint& bottomRight )
	: VPath( parent )
{
	moveTo( KoPoint( topLeft.x(),     topLeft.y() ) );
	lineTo( KoPoint( bottomRight.x(), topLeft.y() ) );
	lineTo( KoPoint( bottomRight.x(), bottomRight.y() ) );
	lineTo( KoPoint( topLeft.x(),     bottomRight.y() ) );
	close();
}

