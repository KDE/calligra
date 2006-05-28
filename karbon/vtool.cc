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

#include <qevent.h>
#include <QLabel>
//Added by qt3to4:
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>

#include <kdebug.h>
#include <kiconloader.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vtoolcontroller.h"
#include "KoContextHelp.h"
#include "vtool.h"
#include "vtool.moc"


VTool::VTool( KarbonView *view, const char *name ) : QObject( 0, name ), m_view( view )
{
	m_mouseButtonIsDown	= false;
	m_isDragging		= false;
	m_shiftPressed		= false;
	m_ctrlPressed		= false;
	m_altPressed		= false;
	m_action = 0;
}

VTool::~VTool()
{
	if (toolController())
		toolController()->unregisterTool( this );

	delete m_action;
	//kDebug(38000) << "Deleting : " << name().latin1() << endl;
}

void
VTool::registerTool( VTool *tool )
{
	toolController()->registerTool( tool );
}

VToolController *
VTool::toolController() const
{
	return m_view->toolController();
}

KarbonView *
VTool::view() const
{
	return m_view;
}

bool
VTool::mouseEvent( QMouseEvent* mouseEvent, const QPointF &canvasCoordinate )
{
	if( !view() || !view()->part() || !view()->part()->isReadWrite() )
		return false;

	m_lastPoint.setX( canvasCoordinate.x() );
	m_lastPoint.setY( canvasCoordinate.y() );

	setCursor();

	m_altPressed = mouseEvent->state() & Qt::AltModifier;
	m_shiftPressed = mouseEvent->state() & Qt::ShiftModifier;
	m_ctrlPressed = mouseEvent->state() & Qt::ControlModifier;

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

		if( mouseEvent->button() == Qt::RightButton )
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
			if( mouseEvent->button() == Qt::RightButton )
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

void
VTool::activate()
{
	kDebug() << k_funcinfo << endl;
	refreshUnit();
	QPixmap Icon = BarIcon( icon() );
	view()->contextHelpAction()->updateHelp( uiname(), contextHelp(), &Icon );
	view()->statusMessage()->setText( statusText() );
	toolController()->setCurrentTool( this );
#if 0
	if( toolController()->activeTool() )
	{
		toolController()->activeTool()->action()->setChecked( false );
		toolController()->activeTool()->deactivate();
	}

	if( toolController()->activeTool() == this )
		showDialog();
	else
	{
		refreshUnit();
		QPixmap Icon = BarIcon( icon() );
		view()->contextHelpAction()->updateHelp( uiname(), contextHelp(), &Icon );
		view()->statusMessage()->setText( statusText() );
		toolController()->activeTool()->action()->setChecked( true );
	}
#endif
}

