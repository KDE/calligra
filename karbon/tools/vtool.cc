/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vtool.h"
#include "karbon_view.h"

#include <qevent.h>

VTool::VTool( KarbonView* view )
	: m_isDragging( false ), m_view( view )
{
}

void
VTool::mouseMoved( QMouseEvent *mouse_event )
{
	if( m_isDragging )
	{
		// erase old object:
		drawTemporaryObject();

		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// paint new object:
		drawTemporaryObject();
	}
}

void
VTool::cancel()
{
	m_isDragging = false;

	// erase old object:
	drawTemporaryObject();
}

bool
VTool::eventFilter( QEvent* event )
{
	QMouseEvent* mouse_event = static_cast<QMouseEvent *>( event );
	setCursor( view()->canvasWidget()->viewportToContents( mouse_event->pos() ) );
	if( event->type() == QEvent::MouseMove )
	{
		mouseMoved( static_cast<QMouseEvent *> ( event ) );
		return true;
	}

	if( event->type() == QEvent::MouseButtonRelease )
	{
		mouseReleased( static_cast<QMouseEvent *> ( event ) );
		return true;
	}

	// handle pressing of keys:
	if( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*> ( event );

		// cancel dragging with ESC-key:
		if( key_event->key() == Qt::Key_Escape && m_isDragging )
		{
			cancel();
			return true;
		}

		// if SHIFT is pressed, we want a square:
		if( key_event->key() == Qt::Key_Shift && m_isDragging )
		{
			dragShiftPressed();
			return true;
		}

		// if Ctrl is pressed, we want a centered path:
		if ( key_event->key() == Qt::Key_Control && m_isDragging )
		{
			dragCtrlPressed();
			return true;
		}
	}

	// handle releasing of keys:
	if( event->type() == QEvent::KeyRelease )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*> ( event );

		if( key_event->key() == Qt::Key_Shift && m_isDragging )
		{
			dragShiftReleased();
			return true;
		}

		if( key_event->key() == Qt::Key_Control && m_isDragging )
		{
			dragCtrlReleased();
			return true;
		}
	}

	// the whole story starts with this event:
	if( event->type() == QEvent::MouseButtonPress )
	{
		mousePressed( static_cast<QMouseEvent *> ( event ) );
		return true;
	}

	return false;
}

