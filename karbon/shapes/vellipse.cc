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


#include "vellipse.h"
#include "vtransformcmd.h"


VEllipse::VEllipse( VObject* parent,
		const KoPoint& topLeft, double width, double height )
	: VComposite( parent )
{
	setDrawCenterNode();

	// Create (half-)unity circle with topLeft at (0|0):
	moveTo( KoPoint( 0.0, -0.5 ) );
	arcTo( KoPoint( 0.0, -1.0 ), KoPoint( 0.5, -1.0 ), 0.5 );
	arcTo( KoPoint( 1.0, -1.0 ), KoPoint( 1.0, -0.5 ), 0.5 );
	arcTo( KoPoint( 1.0,  0.0 ), KoPoint( 0.5,  0.0 ), 0.5 );
	arcTo( KoPoint( 0.0,  0.0 ), KoPoint( 0.0, -0.5 ), 0.5 );
	close();

	// Translate and scale:
	QWMatrix m;
	m.translate( topLeft.x(), topLeft.y() );
	m.scale( width, height );

	VTransformCmd cmd( 0L, m );
	cmd.visit( *this );
}

