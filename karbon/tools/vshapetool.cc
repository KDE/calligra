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

void
VShapeTool::mousePressed( QMouseEvent *mouse_event )
{
	m_fp.setX( mouse_event->pos().x() );
	m_fp.setY( mouse_event->pos().y() );
	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );
		
	// draw initial object:
	recalcCoords();

	drawTemporaryObject();

	m_isDragging = true;
}

void
VShapeTool::mouseMoved( QMouseEvent *mouse_event )
{
	if( !m_isDragging ) return;

	// erase old object:
	drawTemporaryObject();

	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	recalcCoords();

	// paint new object:
	drawTemporaryObject();
}

void
VShapeTool::dragShiftPressed()
{
	m_isSquare = true;

	// erase old object:
	drawTemporaryObject();
	recalcCoords();
	// draw new old object:
	drawTemporaryObject();
}

void
VShapeTool::dragCtrlPressed()
{
	m_isCentered = true;

	// erase old object:
	drawTemporaryObject();
	recalcCoords();
	// draw new old object:
	drawTemporaryObject();
}

void
VShapeTool::dragShiftReleased()
{
	m_isSquare = false;

	// erase old object:
	drawTemporaryObject();
	recalcCoords();
	// draw new old object:
	drawTemporaryObject();
}

void
VShapeTool::dragCtrlReleased()
{
	m_isCentered = false;

	// erase old object:
	drawTemporaryObject();
	recalcCoords();
	// draw new old object:
	drawTemporaryObject();
}

void
VShapeTool::dragAltPressed()
{
}

void
VShapeTool::mouseReleased( QMouseEvent *mouse_event )
{
	if( !m_isDragging ) return;

	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	recalcCoords();

	// adjust to real viewport contents instead of raw mouse coords:
	KoPoint p = view()->canvasWidget()->viewportToContents( QPoint( m_p.x(), m_p.y() ) );
	m_p.setX( m_p.x() / view()->zoom() );
	m_p.setY( m_p.y() / view()->zoom() );
	m_d1 /= view()->zoom();
	if( !m_isPolar )
		m_d2 /= view()->zoom();

	VShapeCmd* cmd = new VShapeCmd(
		&view()->part()->document(),
		name(),
		shape( true ) );

	view()->part()->addCommand( cmd, true );
	view()->selectionChanged();

	m_isDragging = false;
	m_isSquare = false;
	m_isCentered = false;
}

void
VShapeTool::cancel() // ESC pressed
{
	m_isDragging = false;
	m_isSquare = false;
	m_isCentered = false;

	// erase old object:
	drawTemporaryObject();
}


bool
VShapeTool::eventFilter( QEvent* event )
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

void
VShapeTool::drawTemporaryObject() const
{
	VPainter* painter = view()->painterFactory()->editpainter();
	
	VPath* path = shape();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );
	delete( path );
}
