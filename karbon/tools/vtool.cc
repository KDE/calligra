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

#include <qevent.h>
#include <qlabel.h>

#include <kdebug.h>
#include <kiconloader.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vtoolcontroller.h"
#include "kocontexthelp.h"
#include "vtool.h"


VTool::VTool( KarbonPart *part, const char* ) : m_part( part )
{
	m_mouseButtonIsDown	= false;
	m_isDragging		= false;
	m_shiftPressed		= false;
	m_ctrlPressed		= false;
	m_altPressed		= false;
}

VTool::~VTool()
{
	if (part() && part()->toolController())
		part()->toolController()->unregisterTool( this );
	//kdDebug(38000) << "Deleting : " << name().latin1() << endl;
}

void
VTool::registerTool( VTool *tool )
{
	part()->toolController()->registerTool( tool );
}

KarbonView *
VTool::view() const
{
	return part()->toolController()->activeView();
}

void VTool::activateAll()
{
	refreshUnit();
	QPixmap Icon = BarIcon( icon() );
	view()->contextHelpAction()->updateHelp( name(), contextHelp(), &Icon );
	view()->statusMessage()->setText( statusText() );

	activate();
}

bool
VTool::mouseEvent( QMouseEvent* mouseEvent, const KoPoint &canvasCoordinate )
{
	if( !view() || !view()->part() || !view()->part()->isReadWrite() )
		return false;

	m_lastPoint.setX( canvasCoordinate.x() );
	m_lastPoint.setY( canvasCoordinate.y() );

	setCursor();

	m_altPressed = mouseEvent->state() & Qt::AltButton;
	m_shiftPressed = mouseEvent->state() & Qt::ShiftButton;
	m_ctrlPressed = mouseEvent->state() & Qt::ControlButton;

	// Mouse events:
	if( mouseEvent->type() == QEvent::MouseButtonDblClick )
	{
		mouseButtonDblClick();

		return true;
	}

	if( mouseEvent->type() == QEvent::MouseButtonPress )
	{
		m_firstPoint.setX( canvasCoordinate.x() );
		m_firstPoint.setY( canvasCoordinate.y() );

		if( mouseEvent->button() == QEvent::RightButton )
			rightMouseButtonPress();
		else
			mouseButtonPress();

		m_mouseButtonIsDown = true;

		return true;
	}

	if( mouseEvent->type() == QEvent::MouseMove )
	{
		//setCursor();

		if( m_mouseButtonIsDown )
		{
			mouseDrag();

			m_isDragging = true;
		}
		else
			mouseMove();

		return true;
	}

	if( mouseEvent->type() == QEvent::MouseButtonRelease )
	{
		if( m_isDragging )
		{
			mouseDragRelease();

			m_isDragging = false;
		}
		else if( m_mouseButtonIsDown )	// False if canceled.
			if( mouseEvent->button() == QEvent::RightButton )
				rightMouseButtonRelease();
			else
				mouseButtonRelease();

		m_mouseButtonIsDown = false;

		return true;
	}

	return false;
}

bool
VTool::keyEvent( QEvent* event )
{
	// Key press events.
	if( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );

		// Terminate the current drawing with the Enter-key:
		if(
			( keyEvent->key() == Qt::Key_Enter ||
			  keyEvent->key() == Qt::Key_Return )
			&& !m_isDragging )
		{
			accept();

			return true;
		}

		// Terminate the current drawing with the Enter-key:
		if( keyEvent->key() == Qt::Key_Backspace && !m_isDragging )
		{
			cancelStep();

			return true;
		}

		// Cancel dragging with ESC-key:
		if( keyEvent->key() == Qt::Key_Escape )
		{
			cancel();

			m_isDragging = false;
			m_mouseButtonIsDown = false;

			return true;
		}

		// If SHIFT is pressed, some tools create a "square" object while dragging:
		if( keyEvent->key() == Qt::Key_Shift )
		{
			m_shiftPressed = true;
			if( m_isDragging )
			{
				mouseDragShiftPressed();

				return true;
			}
		}

		// If Ctrl is pressed, some tools create a "centered" object while dragging:
		if( keyEvent->key() == Qt::Key_Control )
		{
			m_ctrlPressed = true;
			if( m_isDragging )
			{
				mouseDragCtrlPressed();

				return true;
			}
		}

	}

	// Key release events:
	if( event->type() == QEvent::KeyRelease )
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );

		Qt::Key key = (Qt::Key)keyEvent->key();
		if( key == Qt::Key_Shift )
		{
			m_shiftPressed = false;
			if( m_isDragging )
			{
				mouseDragShiftReleased();

				return true;
			}
		}

		if( key == Qt::Key_Control )
		{
			m_ctrlPressed = false;
			if( m_isDragging )
			{
				mouseDragCtrlReleased();

				return true;
			}
		}

		if( key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Up || key == Qt::Key_Down )
		{
			arrowKeyReleased( key );
			return true;
		}

		return keyReleased( key );
	}

	return false;
}

