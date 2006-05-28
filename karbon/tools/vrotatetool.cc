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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <math.h>

#include <qcursor.h>
#include <QLabel>

#include <klocale.h>
#include <QRectF>

#include <karbon_part.h>
#include <karbon_view.h>
#include <core/vglobal.h>
#include "vrotatetool.h"
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <commands/vtransformcmd.h>

#include <kactioncollection.h>

#include <kdebug.h>

VRotateTool::VRotateTool( KarbonView *view )
	: VTool( view, "tool_rotate" )
{
	m_objects.setAutoDelete( true );
	registerTool( this );
}

VRotateTool::~VRotateTool()
{
	m_objects.clear();
}

void
VRotateTool::activate()
{
	view()->setCursor( QCursor( Qt::ArrowCursor ) );
	view()->part()->document().selection()->setState( VObject::selected );
	view()->part()->document().selection()->showHandle( false );
	VTool::activate();
}

QString
VRotateTool::statusText()
{
	return i18n( "Rotate" );
}

void
VRotateTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	//painter->setZoomFactor( view()->zoom() );

	// TODO: Port to Qt4
	//painter->setRasterOp( Qt::NotROP );

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
			m_angle, altPressed() ),
		true );
}

void
VRotateTool::cancel()
{
	// Erase old object:
	if ( isDragging() )
	{
		draw();
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	}
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

	VRotateCmd cmd( 0L, m_center, m_angle );

	// Copy selected objects and transform:
	m_objects.clear();
	VObject* copy;

	VObjectListIterator itr = view()->part()->document().selection()->objects();
	for ( ; itr.current() ; ++itr )
	{
		if( itr.current()->state() != VObject::deleted )
		{
			copy = itr.current()->clone();

			cmd.visit( *copy );

			copy->setState( VObject::edit );

			m_objects.append( copy );
		}
	}
}

void
VRotateTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( i18n( "Rotate Tool" ), "14_rotate", Qt::SHIFT+Qt::Key_H, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Rotate" ) );
		// TODO porting: m_action->setExclusiveGroup( "manipulation" );
		//m_ownAction = true;
	}
}

