/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qevent.h>

#include "karbon_view.h"
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
	setCursor( view()->canvasWidget()->viewportToContents( mouseEvent->pos() ) );

	// Mouse events:
	if( event->type() == QEvent::MouseButtonPress )
	{
		QPoint canvasCoordinate = view()->canvasWidget()->viewportToContents(
			mouseEvent->pos() );

		m_firstPoint.setX( canvasCoordinate.x() * view()->zoom() );
		m_firstPoint.setY( canvasCoordinate.y() * view()->zoom() );

		mouseButtonPress( m_firstPoint );

		// Draw new object:
		draw();

		m_mouseButtonIsDown = true;

		return true;
	}

	if( event->type() == QEvent::MouseMove )
	{
		QPoint canvasCoordinate = view()->canvasWidget()->viewportToContents(
			mouseEvent->pos() );

		// Erase old object:
		draw();

		m_lastPoint.setX( canvasCoordinate.x() * view()->zoom() );
		m_lastPoint.setY( canvasCoordinate.y() * view()->zoom() );

		if( m_mouseButtonIsDown )
		{
			mouseDrag( m_lastPoint );

			m_isDragging = true;
		}
		else
			mouseMove( m_lastPoint );

		// Draw new object:
		draw();

		return true;
	}

	if( event->type() == QEvent::MouseButtonRelease )
	{
		QPoint canvasCoordinate = view()->canvasWidget()->viewportToContents(
			mouseEvent->pos() );

		// Erase old object:
		draw();

		m_lastPoint.setX( canvasCoordinate.x() * view()->zoom() );
		m_lastPoint.setY( canvasCoordinate.y() * view()->zoom() );

		if( m_isDragging )
		{
			mouseDragRelease( m_lastPoint );

			m_isDragging = false;
		}
		else if( m_mouseButtonIsDown )	// False if canceled.
			mouseButtonRelease( m_lastPoint );

		// Draw new object:
		draw();

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

			// Erase old object:
			draw();

			return true;
		}

		// If SHIFT is pressed, some tools create a "square" object while dragging:
		if( keyEvent->key() == Qt::Key_Shift && m_isDragging )
		{
			// Erase old object:
			draw();

			mouseDragShiftPressed( m_lastPoint );

			// Draw new object:
			draw();

			return true;
		}

		// If Ctrl is pressed, some tools create a "centered" object while dragging:
		if ( keyEvent->key() == Qt::Key_Control && m_isDragging )
		{
			// Erase old object:
			draw();

			mouseDragCtrlPressed( m_lastPoint );

			// Draw new object:
			draw();

			return true;
		}
	}

	// Key release events:
	if( event->type() == QEvent::KeyRelease )
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );

		if( keyEvent->key() == Qt::Key_Shift && m_isDragging )
		{
			// Erase old object:
			draw();

			mouseDragShiftReleased( m_lastPoint );

			// Draw new object:
			draw();

			return true;
		}

		if( keyEvent->key() == Qt::Key_Control && m_isDragging )
		{
			// Erase old object:
			draw();

			mouseDragCtrlReleased( m_lastPoint );

			// Draw new object:
			draw();

			return true;
		}
	}

	return false;
}

