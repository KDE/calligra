/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

	if( m_type == star )
	{
		int j = (edges % 2 == 0 ) ? ( edges - 2 ) / 2 : ( edges - 1 ) / 2;
		//innerRadius = getOptimalInnerRadius( outerRadius, edges, innerAngle );
		int jumpto = 0;
		bool discontinueous = ( edges % 4 == 2 );

		double outerRoundness = ( VGlobal::twopi * outerRadius * roundness ) / edges;
		double nextOuterAngle;

		for ( uint i = 1; i < edges + 1; ++i )
		{
			double nextInnerAngle = angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( jumpto + 0.5 );
			p.setX( innerRadius * cos( nextInnerAngle ) );
			p.setY( innerRadius * sin( nextInnerAngle ) );
			if( roundness == 0.0 )
				lineTo( p );
			else
			{
				nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
				p2.setX( outerRadius * cos( nextOuterAngle ) -
					cos( angle + VGlobal::twopi / edges * jumpto ) * outerRoundness );
				p2.setY( outerRadius * sin( nextOuterAngle ) -
					sin( angle + VGlobal::twopi / edges * jumpto ) * outerRoundness );

				curveTo( p2, p, p );
			}

			jumpto = ( i * j ) % edges;
			nextInnerAngle = angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( jumpto - 0.5 );
			p.setX( innerRadius * cos( nextInnerAngle ) );
			p.setY( innerRadius * sin( nextInnerAngle ) );
			lineTo( p );

			nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
			p.setX( outerRadius * cos( nextOuterAngle ) );
			p.setY( outerRadius * sin( nextOuterAngle ) );

			if( roundness == 0.0 )
				lineTo( p );
			else
			{
				p2.setX( innerRadius * cos( nextInnerAngle ) ); 
				p2.setY( innerRadius * sin( nextInnerAngle ) );

				p3.setX( outerRadius * cos( nextOuterAngle ) +
					cos( angle + VGlobal::twopi / edges * jumpto ) * outerRoundness );
				p3.setY( outerRadius * sin( nextOuterAngle ) +
					sin( angle + VGlobal::twopi / edges * jumpto ) * outerRoundness );

				curveTo( p2, p3, p );
			}
			if( discontinueous && i == ( edges / 2 ) )
			{
				angle += VGlobal::pi;
				nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
				p.setX( outerRadius * cos( nextOuterAngle ) );
				p.setY( outerRadius * sin( nextOuterAngle ) );
				moveTo( p );
			}
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
			double nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 );
			double nextInnerAngle = angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 0.5 );
			if( m_type != polygon )
			{
				p.setX( innerRadius * cos( nextInnerAngle ) );
				p.setY( innerRadius * sin( nextInnerAngle ) );

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

					p3.setX( innerRadius * cos( nextInnerAngle ) +
						cos( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );
					p3.setY( innerRadius * sin( nextInnerAngle ) +
						sin( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );

					if( m_type == gear )
					{
						lineTo( p2 );
						lineTo( p3 );
						lineTo( p );
					}
					else
						curveTo( p2, p3, p );
				}
			}

			p.setX( outerRadius * cos( nextOuterAngle ) );
			p.setY( outerRadius * sin( nextOuterAngle ) );

			if( roundness == 0.0 )
				lineTo( p );
			else
			{
				p2.setX( innerRadius * cos( nextInnerAngle ) -
					cos( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );
				p2.setY( innerRadius * sin( nextInnerAngle ) -
					sin( angle + inAngle + VGlobal::twopi / edges * ( i + 0.5 ) ) * innerRoundness );

				p3.setX( outerRadius * cos( nextOuterAngle ) +
					cos( angle + VGlobal::twopi / edges * ( i + 1.0 ) ) * outerRoundness );
				p3.setY( outerRadius * sin( nextOuterAngle ) +
					sin( angle + VGlobal::twopi / edges * ( i + 1.0 ) ) * outerRoundness );

				if( m_type == gear )
				{
					lineTo( p2 );
					lineTo( p3 );
					lineTo( p );
				}
				else
					curveTo( p2, p3, p );
			}
		}
	}
	if( m_type == wheel || m_type == framed_star )
		for ( int i = edges - 1; i >= 0; --i )
		{
			double nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / edges * ( i + 1.0 );
			p.setX( outerRadius * cos( nextOuterAngle ) );
			p.setY( outerRadius * sin( nextOuterAngle ) );
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

double
VStar::getOptimalInnerRadius( uint edges, double outerRadius, uint innerAngle )
{
	int j = (edges % 2 == 0 ) ? ( edges - 2 ) / 2 : ( edges - 1 ) / 2;

	// get two well chosen lines of the star
	KoPoint p1( outerRadius * cos( VGlobal::pi_2 ), outerRadius * sin( VGlobal::pi_2 ) );
	int jumpto = ( j ) % edges;
	double nextOuterAngle = VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
	KoPoint p2( outerRadius * cos( nextOuterAngle ), outerRadius * sin( nextOuterAngle ) );

	nextOuterAngle = VGlobal::pi_2 + VGlobal::twopi / edges;
	KoPoint p3( outerRadius * cos( nextOuterAngle ),
				outerRadius * sin( nextOuterAngle ) );
	jumpto = ( edges - j + 1 ) % edges;
	nextOuterAngle = VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
	KoPoint p4( outerRadius * cos( nextOuterAngle ), outerRadius * sin( nextOuterAngle ) );

	// calc (x, y) -> intersection point
	double b1 = ( p2.y() - p1.y() ) / ( p2.x() - p1.x() );
	double b2 = ( p4.y() - p3.y() ) / ( p4.x() - p3.x() );
	double a1 = p1.y() - b1 * p1.x();
	double a2 = p3.y() - b2 * p3.x();
	double x = -( a1 - a2 ) / ( b1 - b2 );
	double y = a1 + b1 * x;
	// calc inner radius from intersection point and center
	return sqrt( x * x + y * y );
}

QString
VStar::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Star" );
}

