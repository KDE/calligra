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

#include <qcursor.h>
#include <qevent.h>
#include <qlabel.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vcomposite.h"
#include "vglobal.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vshapecmd.h"
#include "vshapetool.h"


VShapeTool::VShapeTool( KarbonView* view, const QString& name, bool polar )
	: VTool( view ), m_name( name )
{
	m_isPolar = polar;
	m_isSquare   = false;
	m_isCentered = false;
}

void
VShapeTool::activate()
{
	view()->statusMessage()->setText( name() );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::crossCursor ) );
}

void
VShapeTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	view()->canvasWidget()->setYMirroring( true );
	painter->setRasterOp( Qt::NotROP );

	VComposite* composite = shape();
	composite->setState( VComposite::edit );
	composite->draw( painter, &composite->boundingBox() );
	delete( composite );
}

void
VShapeTool::mouseButtonPress()
{
	recalc();

	// Draw new object:
	draw();
}

void
VShapeTool::mouseButtonRelease()
{
	draw();

	recalc();

	VComposite* composite = shape( true );

	if( composite )
	{
		VShapeCmd* cmd = new VShapeCmd(
			&view()->part()->document(),
			name(),
			composite );

		view()->part()->addCommand( cmd, true );
		view()->selectionChanged();
	}

	m_isSquare = false;
	m_isCentered = false;
}

void
VShapeTool::mouseDrag()
{
	// Erase old object:
	draw();

	recalc();

	// Draw new object:
	draw();
}

void
VShapeTool::mouseDragRelease()
{
	//recalc();

	VShapeCmd* cmd = new VShapeCmd(
		&view()->part()->document(),
		name(),
		shape() );

	view()->part()->addCommand( cmd, true );
	view()->selectionChanged();

	m_isSquare = false;
	m_isCentered = false;
}

void
VShapeTool::mouseDragShiftPressed()
{
	// Erase old object:
	draw();

	m_isSquare = true;
	recalc();

	// Draw new object:
	draw();
}

void
VShapeTool::mouseDragCtrlPressed()
{
	// Erase old object:
	draw();

	m_isCentered = true;
	recalc();

	// Draw new object:
	draw();
}

void
VShapeTool::mouseDragShiftReleased()
{
	// Erase old object:
	draw();

	m_isSquare = false;
	recalc();

	// Draw new object:
	draw();
}

void
VShapeTool::mouseDragCtrlReleased()
{
	// Erase old object:
	draw();

	m_isCentered = false;
	recalc();

	// Draw new object:
	draw();
}

void
VShapeTool::cancel()
{
	// Erase old object:
	draw();

	m_isSquare = false;
	m_isCentered = false;
}

void
VShapeTool::recalc()
{
	// Calculate radius and angle:
	if( m_isPolar )
	{
		// Radius:
		m_d1 = sqrt(
			( last().x() - first().x() ) * ( last().x() - first().x() ) +
			( last().y() - first().y() ) * ( last().y() - first().y() ) );

		// Angle:
		m_d2 = atan2( last().y() - first().y(), last().x() - first().x() );

		// Define pi/2 as "0.0":
		m_d2 -= VGlobal::pi_2;

		m_p = first();
	}
	else
	// Calculate width and height:
	{
		m_d1 = last().x() - first().x();
		m_d2 = last().y() - first().y();

		const int m_sign1 = m_d1 < 0.0 ? -1 : +1;
// TODO: revert when we introduce y-mirroring:
		const int m_sign2 = m_d2 < 0.0 ? +1 : -1;

		// Make unsigned:
		if( m_d1 < 0.0 )
			m_d1 = -m_d1;

		if( m_d2 < 0.0 )
			m_d2 = -m_d2;

		if ( m_isSquare )
		{
			if ( m_d1 > m_d2 )
				m_d2 = m_d1;
			else
				m_d1 = m_d2;
		}

		m_p.setX(
			first().x() - ( m_sign1 == -1 ? m_d1 : 0.0 ) );
// TODO: revert when we introduce y-mirroring:
		m_p.setY(
			first().y() + ( m_sign2 == -1 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() + m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

