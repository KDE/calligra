/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qevent.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vshapetool.h"


VShapeTool::VShapeTool( KarbonView* view, bool polar )
	: VTool( view ), m_calcPolar( polar ),
		m_isDragging( false ), m_isSquare( false ),
		m_isCentered( false )
{
}

bool
VShapeTool::eventFilter( QEvent* event )
{
	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject( m_p, m_d1, m_d2 );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		// paint new object:
		drawTemporaryObject( m_p, m_d1, m_d2 );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		VCommand* cmd = 0L;

		// adjust to real viewport contents instead of raw mouse coords:
		KoPoint p = view()->canvasWidget()->viewportToContents( QPoint( m_p.x(), m_p.y() ) );
		cmd = createCmd(
			p.x() / view()->zoom(),
			p.y() / view()->zoom(),
			m_d1 / view()->zoom(),
			m_calcPolar ? m_d2 : m_d2 / view()->zoom() );

		if( cmd )
			view()->part()->addCommand( cmd, true );
		else
		{
			// erase old object:
			drawTemporaryObject( m_p, m_d1, m_d2 );
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
			drawTemporaryObject( m_p, m_d1, m_d2 );

			return true;
		}

		// if SHIFT is pressed, we want a square:
		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = true;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( m_p, m_d1, m_d2 );
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
				drawTemporaryObject( m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( m_p, m_d1, m_d2 );
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
				drawTemporaryObject( m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( m_p, m_d1, m_d2 );
			}

			return true;
		}

		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = false;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( m_p, m_d1, m_d2 );
			}

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>( event );
		m_fp.setX( mouse_event->pos().x() );
		m_fp.setY( mouse_event->pos().y() );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );
		
		// draw initial object:
		recalcCoords();
		drawTemporaryObject( m_p, m_d1, m_d2 );

		m_isDragging = true;

		return true;
	}

	return false;
}

