/* This file is part of the KDE project
   Copyright (C) 2001, 2003, 2003 The Karbon Developers

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


#include "vellipse.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <vglobal.h>

VEllipse::VEllipse( VObject* parent,
		const KoPoint& topLeft, double width, double height,
		VEllipseType type, double startAngle, double endAngle )
	: VComposite( parent ), m_type( type )
{
	setDrawCenterNode();

	int q1 = floor( startAngle / 90.0 );
	int q2 = floor( endAngle / 90.0 );
	// to radials
	startAngle = VGlobal::pi_2 * ( startAngle / 90.0 );
	endAngle   = VGlobal::pi_2 * ( endAngle / 90.0 );
	// Create (half-)unity circle with topLeft at (0|0):
	KoPoint start( 0.5 * sin( -startAngle ), 0.5 * cos( -startAngle ) );
	moveTo( KoPoint( start.x(), start.y() ) );
	KoPoint current;
	double rest = ( ( q1 + 1 ) * VGlobal::pi_2 - startAngle ) / 2.0;
	double midAngle = ( q1 * VGlobal::pi_2 ) + rest + startAngle;
	//double midAngle = VGlobal::pi_2 / 2.0;
	int i = q1;
	while( i < q2 - q1 )
	{
		i = ++i % 4;
		current = KoPoint( 0.5 * sin( -VGlobal::pi_2 * i - startAngle ), 0.5 * cos( -VGlobal::pi_2 * i - startAngle ) );
		midAngle += VGlobal::pi_2;
		arcTo( KoPoint( cos( midAngle ) * ( 0.5 / cos( VGlobal::pi_2 / 2.0 ) ),
						sin( midAngle ) * ( 0.5 / cos( VGlobal::pi_2 / 2.0 ) ) ) , current, 0.5 );
	}
	rest = ( ( endAngle - ( i * VGlobal::pi_2 ) ) / 2.0 );
	midAngle = ( ( i + 1 ) * VGlobal::pi_2 ) + rest;
	KoPoint end( 0.5 * sin( -endAngle ), 0.5 * cos( -endAngle ) );
	arcTo( KoPoint( cos( midAngle ) * ( 0.5 / cos( rest ) ), sin( midAngle ) * ( 0.5 / cos( rest ) ) ), end, 0.5 );
	if( m_type == cut )
		lineTo( KoPoint( 0.0, 0.0 ) );
	if( m_type != arc )
		close();

	// Translate and scale:
	QWMatrix m;
	m.translate( topLeft.x(), topLeft.y() );
	m.scale( width, height );

	VTransformCmd cmd( 0L, m );
	cmd.visit( *this );
}

QString
VEllipse::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Ellipse" );
}

