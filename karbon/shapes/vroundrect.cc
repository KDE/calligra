/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <klocale.h>

#include "vroundrect.h"
#include "vglobal.h"

VRoundRect::VRoundRect( VObject* parent,
		const KoPoint& topLeft, double width, double height, double edgeRadius )
	: VComposite( parent )
{
	setDrawCenterNode();

	if( edgeRadius < 0.0 )
		edgeRadius = 0.0;

	// Catch case, when radius is larger than width or height:
	double minimum;

	if( edgeRadius > ( minimum = kMin( width * 0.5, height * 0.5 ) ) )
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

QString
VRoundRect::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : "Round Rectangle";
}

