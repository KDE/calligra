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

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>

#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vglobal.h"
#include "vrotatetool.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vtransformcmd.h"

#include <kdebug.h>


VRotateTool::VRotateTool( KarbonView* view )
	: VTool( view )
{
	m_objects.setAutoDelete( true );
}

VRotateTool::~VRotateTool()
{
	m_objects.clear();
}

void
VRotateTool::activate()
{
	view()->statusMessage()->setText( i18n( "Rotate" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );

	view()->part()->document().selection()->setState( VObject::edit );
}

void
VRotateTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		itr.current()->draw( painter, &itr.current()->boundingBox() );
	}
}

void
VRotateTool::mouseButtonPress()
{
	//view()->painterFactory()->painter()->end();

	recalc();

	// Draw new object:
	draw();
}

void
VRotateTool::mouseDrag()
{
	// Erase old object:
	draw();

	recalc();

	// Draw new object:
	draw();
}

void
VRotateTool::mouseDragRelease()
{
	view()->part()->addCommand(
		new VRotateCmd(
			&view()->part()->document(),
			m_center,
			m_angle ),
		true );

	view()->selectionChanged();
}

void
VRotateTool::recalc()
{
	// Get center:
	m_center = view()->part()->document().selection()->boundingBox().center();

	// Calculate angle between vector (last - center) and (first - center):
	m_angle = VGlobal::one_pi_180 * (
			atan2(
				last().y() - m_center.y(),
				last().x() - m_center.x() )
		-
			atan2(
				first().y() - m_center.y(),
				first().x() - m_center.x() ) );

	// Build affine matrix:
	QWMatrix mat;
	mat.translate( m_center.x(), m_center.y() );
	mat.rotate( m_angle );
	mat.translate( -m_center.x(), -m_center.y() );


	// Copy selected objects and transform:
	m_objects.clear();
	VObject* copy;

	VObjectListIterator itr = view()->part()->document().selection()->objects();
	for ( ; itr.current() ; ++itr )
	{
		if( itr.current()->state() != VObject::deleted )
		{
			copy = itr.current()->clone();
			copy->transform( mat );
			copy->setState( VObject::edit );

			m_objects.append( copy );
		}
	}
}

