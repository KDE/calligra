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

#include <qcursor.h>
#include <qevent.h>
#include <qlabel.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vcanvas.h"
#include "vcomposite.h"
#include "vglobal.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vshapecmd.h"
#include "vshapetool.h"
#include "vselection.h"
#include "vcursor.h"

VShapeTool::VShapeTool( KarbonView *view, const char *name, bool polar )
	: VTool( view, name )
{
	m_cursor = new QCursor( VCursor::createCursor( VCursor::CrossHair ) );

	m_isPolar = polar;
	m_isSquare   = false;
	m_isCentered = false;
}

VShapeTool::~VShapeTool()
{
	delete m_cursor;
}

QString
VShapeTool::contextHelp()
{
	QString s = i18n( "<qt><b>Shape tool</b><br>" );
	s += i18n( "<i>Click and drag</i> to place your own shape.<br>" );
	s += i18n( "<i>Click</i> to place a shape using the tool properties values.</qt>" );
	return s;
} 

void
VShapeTool::activate()
{
	VTool::activate();
	view()->setCursor( *m_cursor );
	view()->part()->document().selection()->showHandle( true );
}

QString
VShapeTool::statusText()
{
	return uiname();
}

void
VShapeTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	VPath* composite = shape();
	composite->setState( VPath::edit );
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

	if( showDialog() )
	{
		VPath* composite = shape( true );

		if( composite )
		{
			VShapeCmd* cmd = new VShapeCmd(
				&view()->part()->document(),
				uiname(), composite, icon() );

			view()->part()->addCommand( cmd, true );
		}
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
		uiname(), shape(), icon() );

	view()->part()->addCommand( cmd, true );

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
	if ( isDragging() )
	{
		draw();
		m_isSquare = false;
		m_isCentered = false;
	}
}

void
VShapeTool::recalc()
{
	m_isSquare = shiftPressed();
	m_isCentered = ctrlPressed();

	KoPoint _first = view()->canvasWidget()->snapToGrid( first() );
	KoPoint _last = view()->canvasWidget()->snapToGrid( last() );

	// Calculate radius and angle:
	if( m_isPolar )
	{
		// Radius:
		m_d1 = sqrt(
			( _last.x() - _first.x() ) * ( _last.x() - _first.x() ) +
			( _last.y() - _first.y() ) * ( _last.y() - _first.y() ) );

		// Angle:
		m_d2 = atan2( _last.y() - _first.y(), _last.x() - _first.x() );

		// Define pi/2 as "0.0":
		m_d2 -= VGlobal::pi_2;

		m_p = _first;
	}
	else
	// Calculate width and height:
	{
		m_d1 = _last.x() - _first.x();
		m_d2 = _last.y() - _first.y();

		const int m_sign1 = VGlobal::sign( m_d1 );
// TODO: revert when we introduce y-mirroring:
		const int m_sign2 = VGlobal::sign( -m_d2 );

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
			_first.x() - ( m_sign1 == -1 ? m_d1 : 0.0 ) );
// TODO: revert when we introduce y-mirroring:
		m_p.setY(
			_first.y() + ( m_sign2 == -1 ? m_d2 : 0.0 ) );

		if ( m_isCentered )
		{
			m_p.setX( m_p.x() - m_sign1 * qRound( m_d1 * 0.5 ) );
			m_p.setY( m_p.y() + m_sign2 * qRound( m_d2 * 0.5 ) );
		}
	}
}

