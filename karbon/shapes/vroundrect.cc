/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef QMIN
	#define QMIN(a, b)      ((a) < (b) ? (a) : (b))
#endif

#include <klocale.h>

#include "vroundrect.h"


VRoundRect::VRoundRect( VObject* parent,
		const KoPoint& topLeft, double width, double height, double edgeRadius )
	: VPath( parent )
{
	setDrawCenterNode();

	if( edgeRadius < 0.0 )
		edgeRadius = 0.0;

	// Catch case, when radius is larger than width or height:
	double minimum;

	if( edgeRadius > ( minimum = QMIN( width * 0.5, height * 0.5 ) ) )
	{
 		edgeRadius = minimum;
	}


	moveTo(
		KoPoint( topLeft.x(), topLeft.y() - height + edgeRadius ) );
	arcTo(
		KoPoint( topLeft.x(), topLeft.y() - height ),
		KoPoint( topLeft.x() + edgeRadius, topLeft.y() - height ), edgeRadius );
	arcTo(
		KoPoint( topLeft.x() + width, topLeft.y() - height ),
		KoPoint( topLeft.x() + width, topLeft.y() - height + edgeRadius ), edgeRadius );
	arcTo(
		KoPoint( topLeft.x() + width, topLeft.y() ),
		KoPoint( topLeft.x() + width - edgeRadius, topLeft.y() ), edgeRadius );
	arcTo(
		KoPoint( topLeft.x(), topLeft.y() ),
		KoPoint( topLeft.x(), topLeft.y() - edgeRadius ), edgeRadius );
	close();
}

