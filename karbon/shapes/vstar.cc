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


#include <math.h>

#include <qwmatrix.h>

#include "vglobal.h"
#include "vstar.h"


VStar::VStar( VObject* parent,
		const KoPoint& center, double outerRadius, double innerRadius,
		uint edges, double angle )
	: VComposite( parent )
{
	setDrawCenterNode();

	// A star should have at least 3 edges:
	if( edges < 3 )
		edges = 3;

	// Make sure, radii are positive:
	if( outerRadius < 0.0 )
		outerRadius = -outerRadius;

	if( innerRadius < 0.0 )
		innerRadius = -innerRadius;


	// We start at angle + VGlobal::pi_2:
	KoPoint p(
		outerRadius * cos( angle + VGlobal::pi_2 ),
		outerRadius * sin( angle + VGlobal::pi_2 ) );
	moveTo( p );

	for ( uint i = 0; i < edges; ++i )
	{
		p.setX( innerRadius *
			cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) );
		p.setY( innerRadius *
			sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) );
		lineTo( p );

		p.setX( outerRadius *
			cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) );
		p.setY( outerRadius *
			sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) );
		lineTo( p );
	}
	close();

	// translate path to center:
	QWMatrix m;
	m.translate( center.x(), center.y() );
	transform( m );
}

