/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qevent.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vshapecmd.h"
#include "vshapetool.h"


VShapeTool::VShapeTool( KarbonView* view, const QString& name, bool polar )
	: VTool( view ), m_name( name )
{
	m_isPolar = polar;
	m_isDragging = false;
	m_isSquare   = false;
	m_isCentered = false;
}

bool
VShapeTool::eventFilter( QEvent* event )
{
	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject();

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		// paint new object:
		drawTemporaryObject();

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		// adjust to real viewport contents instead of raw mouse coords:
		KoPoint p = view()->canvasWidget()->viewportToContents( QPoint( m_p.x(), m_p.y() ) );


		VShapeCmd* cmd = new VShapeCmd(
			&view()->part()->document(),
			name(),
			shape( true ) );

		view()->part()->addCommand( cmd, true );
		view()->selectionChanged();


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
			drawTemporaryObject();

			return true;
		}

		// if SHIFT is pressed, we want a square:
		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = true;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject();
				recalcCoords();
				// draw new old object:
				drawTemporaryObject();
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
				drawTemporaryObject();
				recalcCoords();
				// draw new old object:
				drawTemporaryObject();
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
				drawTemporaryObject();
				recalcCoords();
				// draw new old object:
				drawTemporaryObject();
			}

			return true;
		}

		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = false;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject();
				recalcCoords();
				// draw new old object:
				drawTemporaryObject();
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
		drawTemporaryObject();

		m_isDragging = true;

		return true;
	}

	return false;
}

void
VShapeTool::drawTemporaryObject() const
{
	VPainter* painter = view()->painterFactory()->editpainter();
	
	VPath* path = shape();
	path->setState( state_edit );
	path->draw( painter, path->boundingBox() );
	delete( path );
}
