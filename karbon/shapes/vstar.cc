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
#include "vtransformcmd.h"
#include <klocale.h>

VStar::VStar( VObject* parent,
		const KoPoint& center, double outerRadius, double innerRadius,
		uint edges, double angle, uint innerAngle, double roundness, VStarType type )
	: VComposite( parent ), m_type( type )
{
	// A star should have at least 3 edges:
	if( edges < 3 )
		edges = 3;

	// Make sure, radii are positive:
	if( outerRadius < 0.0 )
		outerRadius = -outerRadius;

	if( innerRadius < 0.0 )
		innerRadius = -innerRadius;


	// We start at angle + VGlobal::pi_2:
	KoPoint p2, p3;
	KoPoint p(
		outerRadius * cos( angle + VGlobal::pi_2 ),
		outerRadius * sin( angle + VGlobal::pi_2 ) );
	moveTo( p );

	double inAngle = VGlobal::twopi / 360 * innerAngle;

	if( m_type == star || m_type == framed_star )
	{
		innerRadius = 0.4 * outerRadius;
		int j = (edges % 2 == 0 ) ? ( edges - 2 ) / 2 : ( edges - 1 ) / 2;
		int k = j - 1;
		int jumpto = 0;
		for ( uint i = 1; i < edges + 1; ++i )
		{
			p.setX( innerRadius *
				cos( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( jumpto + 0.5 ) ) );
			p.setY( innerRadius *
				sin( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( jumpto + 0.5 ) ) );
			lineTo( p );

			jumpto = ( i * j ) % edges;

			p.setX( innerRadius *
				cos( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( jumpto - 0.5 ) ) );
			p.setY( innerRadius *
				sin( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( jumpto - 0.5 ) ) );
			lineTo( p );

			p.setX( outerRadius *
				cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * jumpto ) );
			p.setY( outerRadius *
				sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * jumpto ) );
			lineTo( p );
		}
	}
	else
	{
		if( m_type == wheel || m_type == spoke )
			innerRadius = 0.0;

		double innerRoundness = ( VGlobal::twopi * innerRadius * roundness ) / edges;
		double outerRoundness = ( VGlobal::twopi * outerRadius * roundness ) / edges;
		for ( uint i = 0; i < edges; ++i )
		{
			if( m_type != polygon )
			{
				p.setX( innerRadius *
					cos( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) );
				p.setY( innerRadius *
					sin( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) );

				if( roundness == 0.0 )
					lineTo( p );
				else
				{
					p2.setX( outerRadius *
						cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i ) ) -
						cos( angle + VGlobal::twopi / edges * ( i ) ) * outerRoundness );
					p2.setY( outerRadius *
						sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i ) ) -
						sin( angle + VGlobal::twopi / edges * ( i ) ) * outerRoundness );

					p3.setX( innerRadius *
						cos( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) +
						cos( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );
					p3.setY( innerRadius *
						sin( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) +
						sin( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );

					curveTo( p2, p3, p );
				}
			}

			p.setX( outerRadius *
				cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) );
			p.setY( outerRadius *
				sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) );

			if( roundness == 0.0 )
				lineTo( p );
			else
			{
				p2.setX( innerRadius *
					cos( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) -
					cos( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );
				p2.setY( innerRadius *
					sin( angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 ) ) -
					sin( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );

				p3.setX( outerRadius *
					cos( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) +
					cos( angle + VGlobal::twopi / edges * ( i + 1.0 ) ) * outerRoundness );
				p3.setY( outerRadius *
					sin( angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 ) ) +
					sin( angle + VGlobal::twopi / edges * ( i + 1.0 ) ) * outerRoundness );

				curveTo( p2, p3, p );
			}
		}
	}
	if( m_type == wheel || m_type == framed_star )
		for ( int i = edges - 1; i >= 0; --i )
		{
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

	VTransformCmd cmd( 0L, m );
	cmd.visit( *this );

	setFillRule( evenOdd );
}

QString
VStar::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Star" );
}

