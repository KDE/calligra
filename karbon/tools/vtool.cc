/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vtool.h"

#include <qevent.h>

VTool::VTool( KarbonView* view )
	: m_isDragging( false ), m_view( view )
{
}

bool
VTool::eventFilter( QEvent* event )
{
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

