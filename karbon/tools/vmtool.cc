/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qevent.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool.h"

VMTool::VMTool( KarbonPart* part, bool polar )
	: VTool( part, polar )
{
}

bool
VMTool::eventFilter( KarbonView* view, QEvent* event )
{
	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject( view, m_p, m_d1, m_d2 );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp = view->canvasWidget()->viewportToContents( mouse_event->pos() );

		recalcCoords();

		// paint new object:
		drawTemporaryObject( view, m_p, m_d1, m_d2 );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp = view->canvasWidget()->viewportToContents( mouse_event->pos() );

		recalcCoords();
        VCommand* cmd = 0L;
		{
			// adjust to real viewport contents instead of raw mouse coords
			QPoint p = view->canvasWidget()->viewportToContents( m_p );
			double zoom = view->zoomFactor();
			cmd = createCmd( m_p.x() / zoom, m_p.y() / zoom, m_d1 / zoom, m_d2 / zoom );
		}

		if( cmd )
			m_part->addCommand( cmd );
		else
		{
			// erase old object:
			drawTemporaryObject( view, m_p, m_d1, m_d2 );
			m_part->repaintAllViews();
		}

		m_isDragging = false;
		m_isSquare = false;
		m_isCentered = false;

		return true;
	}

	// handle pressing of keys:
	if ( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*> ( event );

		// cancel dragging with ESC-key:
		if ( key_event->key() == Qt::Key_Escape && m_isDragging )
		{
			m_isDragging = false;
			m_isSquare = false;
			m_isCentered = false;

			// erase old object:
			drawTemporaryObject( view, m_p, m_d1, m_d2 );

			return true;
		}

		// if SHIFT is pressed, we want a square:
		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = true;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
			}

			return true;
		}

		// if Ctrl is pressed, we want a centered path:
		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = true;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
			}

			return true;
		}
	}

	// handle releasing of keys:
	if ( event->type() == QEvent::KeyRelease )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*> ( event );

		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = false;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
			}

			return true;
		}

		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = false;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
			}

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>( event );
		m_fp = view->canvasWidget()->viewportToContents( mouse_event->pos() );
		m_lp = view->canvasWidget()->viewportToContents( mouse_event->pos() );

		// draw initial object:
		recalcCoords();
		drawTemporaryObject( view, m_p, m_d1, m_d2 );

		// inform about dragging
		startDragging();	
		m_isDragging = true;

		return true;
	}

	return false;
}

