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
		QPoint canvasCoordinate = view()->canvasWidget()->ViewportToContents(
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
		QPoint canvasCoordinate = view()->canvasWidget()->ViewportToContents(
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
		QPoint canvasCoordinate = view()->canvasWidget()->ViewportToContents(
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

