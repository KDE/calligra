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
	if( m_isDragging )
	{
		// erase old object:
		drawTemporaryObject();

		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		// paint new object:
		drawTemporaryObject();
	}
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

void
VShapeTool::drawTemporaryObject()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	
	VPath* path = shape();
	path->setState( VObject::edit );
	path->draw( painter, path->boundingBox() );
	delete( path );
}
