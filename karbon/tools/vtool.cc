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

#include <qevent.h>

#include "karbon_view.h"
#include "karbon_part.h"
#include "vtool.h"


VTool::VTool( KarbonView* view )
	: m_view( view )
{
	m_mouseButtonIsDown = false;
	m_isDragging = false;
}

bool
VTool::eventFilter( QEvent* event )
{
	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );
	setCursor( view()->canvasWidget()->toContents( mouseEvent->pos() ) );

	// Mouse events:
	if( event->type() == QEvent::MouseButtonPress )
	{
		QPoint canvasCoordinate = view()->canvasWidget()->toContents(
			mouseEvent->pos() );

		m_firstPointRaw.setX( mouseEvent->pos().x() );
		m_firstPointRaw.setY( mouseEvent->pos().y() );
		m_firstPoint.setX( canvasCoordinate.x() * view()->zoom() );
		m_firstPoint.setY( canvasCoordinate.y() * view()->zoom() );

		mouseButtonPress( m_firstPoint );

		m_mouseButtonIsDown = true;

		return true;
	}

	if( event->type() == QEvent::MouseMove )
	{
            if ( !view()->part()->isReadWrite())
                return false;

		QPoint canvasCoordinate = view()->canvasWidget()->toContents(
			mouseEvent->pos() );

		m_lastPointRaw.setX( mouseEvent->pos().x() );
		m_lastPointRaw.setY( mouseEvent->pos().y() );
		m_lastPoint.setX( canvasCoordinate.x() * view()->zoom() );
		m_lastPoint.setY( canvasCoordinate.y() * view()->zoom() );

		if( m_mouseButtonIsDown )
		{
			mouseDrag( KoPoint( mouseEvent->pos() ) );

			m_isDragging = true;
		}
		else
			mouseMove( m_lastPoint );

		return true;
	}

	if( event->type() == QEvent::MouseButtonRelease )
	{
		QPoint canvasCoordinate = view()->canvasWidget()->toContents(
			mouseEvent->pos() );

		m_lastPointRaw.setX( mouseEvent->pos().x() );
		m_lastPointRaw.setY( mouseEvent->pos().y() );
		m_lastPoint.setX( canvasCoordinate.x() * view()->zoom() );
		m_lastPoint.setY( canvasCoordinate.y() * view()->zoom() );

		if( m_isDragging )
		{
			mouseDragRelease( m_lastPoint );

			m_isDragging = false;
		}
		else if( m_mouseButtonIsDown )	// False if canceled.
			mouseButtonRelease( m_lastPoint );

		m_mouseButtonIsDown = false;

		return true;
	}

	// Key press events:
	if( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );

		// Cancel dragging with ESC-key:
		if( keyEvent->key() == Qt::Key_Escape && m_isDragging )
		{
			cancel();

			m_isDragging = false;
			m_mouseButtonIsDown = true;

			return true;
		}

		// If SHIFT is pressed, some tools create a "square" object while dragging:
		if( keyEvent->key() == Qt::Key_Shift && m_isDragging )
		{
			mouseDragShiftPressed( m_lastPoint );

			return true;
		}

		// If Ctrl is pressed, some tools create a "centered" object while dragging:
		if ( keyEvent->key() == Qt::Key_Control && m_isDragging )
		{
			mouseDragCtrlPressed( m_lastPoint );

			return true;
		}
	}

	// Key release events:
	if( event->type() == QEvent::KeyRelease )
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );

		if( keyEvent->key() == Qt::Key_Shift && m_isDragging )
		{
			mouseDragShiftReleased( m_lastPoint );

			return true;
		}

		if( keyEvent->key() == Qt::Key_Control && m_isDragging )
		{
			mouseDragCtrlReleased( m_lastPoint );

			return true;
		}
	}

	return false;
}

