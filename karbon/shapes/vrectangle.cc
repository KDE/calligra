/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include "vrectangle.h"


VRectangle::VRectangle( VObject* parent,
		const KoPoint& topLeft, double width, double height )
	: VPath( parent )
{
	moveTo( topLeft );
	lineTo( KoPoint( topLeft.x() + width, topLeft.y() ) );
	lineTo( KoPoint( topLeft.x() + width, topLeft.y() - height ) );
	lineTo( KoPoint( topLeft.x(),         topLeft.y() - height ) );
	close();
}

