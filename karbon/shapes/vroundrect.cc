/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/


#include <klocale.h>

#include "vroundrect.h"


VRoundRect::VRoundRect( VObject* parent,
		const KoPoint& topLeft, double width, double height, double edgeRadius )
	: VPath( parent )
{
	if( edgeRadius < 0.0 )
		edgeRadius = 0.0;

	// catch case, when radius is larger than width or height:
	double minimum;

	if( edgeRadius  > ( minimum = QMIN( width, height * 0.5 ) )
	{
 		edgeRadius = minimum;
	}


	moveTo( KoPoint( topLeft.x(), topLeft.y() - edgeRadius ) );
	arcTo(
		KoPoint( topLeft.x(), topLeft.y() ),
		KoPoint( topLeft.x() + edgeRadius, topLeft.y() ), edgeRadius );
	arcTo(
		KoPoint( topLeft.x() + width, topLeft.y() ),
		KoPoint( topLeft.x() + width, topLeft.y() - edgeRadius ), edgeRadius );
	arcTo(
		KoPoint( topLeft.x() + width, topLeft.y() - height ),
		KoPoint( topLeft.x() + width - edgeRadius, topLeft.y() - height ), edgeRadius );
	arcTo(
		KoPoint( topLeft.x(), topLeft.y() - height ),
		KoPoint( topLeft.x(), topLeft.y() - height + edgeRadius ), edgeRadius );
	close();
}

