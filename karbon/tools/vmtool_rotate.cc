/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_rotate.h"
#include "vmcmd_transform.h"

#include <math.h>

const double deg2rad = 0.017453292519943295769;    // pi/180

VMToolRotate* VMToolRotate::s_instance = 0L;

VMToolRotate::VMToolRotate( KarbonPart* part )
	: VTool( part ), m_isDragging( false )
{
}

VMToolRotate::~VMToolRotate()
{
}

VMToolRotate*
VMToolRotate::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolRotate( part );
	}

	return s_instance;
}

void
VMToolRotate::drawTemporaryObject( KarbonView* view )
{
	QPainter painter( view->canvasWidget()->viewport() );
	painter.setRasterOp( Qt::NotROP );

	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->selection().isEmpty()
		&& ( part()->selection().getFirst()->boundingBox( view->zoomFactor() ).contains( m_fp ) ) )
//		part()->selection()->boundingBox().contains( p /* view->zoomFactor() */ ) ) )
	{
		// rotate operation
		QWMatrix mat;
		mat.translate( m_fp.x(), m_fp.y() );
		m_angle = atan2( m_lp.y() - m_fp.y(), m_lp.x() - m_fp.x() );
		//m_angle += M_PI / 2;
		mat.rotate( m_angle / deg2rad );
		mat.translate( - m_fp.x(), - m_fp.y() );

		// TODO :  makes a copy of the selection, do assignment operator instead
		VObjectListIterator itr = part()->selection();
		VObjectList list;
		list.setAutoDelete( true );
	    for( ; itr.current() ; ++itr )
		{
			list.append( itr.current()->clone() );
		}
		VObjectListIterator itr2 = list;
		for( ; itr2.current() ; ++itr2 )
		{
			itr2.current()->transform( mat );
			itr2.current()->setState( VObject::edit );
			itr2.current()->draw(
				painter,
				itr2.current()->boundingBox( view->zoomFactor() ),
				view->zoomFactor() );
		}
	}
	/*else
	{
		painter.setPen( Qt::DotLine );

		painter.moveTo( m_fp.x(), m_fp.y() );
		painter.lineTo( m_lp.x(), m_fp.y() );
		painter.lineTo( m_lp.x(), m_lp.y() );
		painter.lineTo( m_fp.x(), m_lp.y() );
		painter.lineTo( m_fp.x(), m_fp.y() );

	}*/
}

bool
VMToolRotate::eventFilter( KarbonView* view, QEvent* event )
{
	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject( view );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// paint new object:
		drawTemporaryObject( view );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// adjust to real viewport contents instead of raw mouse coords:
		QPoint fp = view->canvasWidget()->viewportToContents( m_fp );
		QPoint lp = view->canvasWidget()->viewportToContents( m_lp );

		part()->addCommand(
			new VMCmdRotate(
				part(),
				part()->selection(), m_fp, m_angle / deg2rad ),
			true );

//			part()->repaintAllViews();

		m_isDragging = false;

		return true;
	}

	// handle pressing of keys:
	if ( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>( event );

		// cancel dragging with ESC-key:
		if ( key_event->key() == Qt::Key_Escape && m_isDragging )
		{
			m_isDragging = false;

			// erase old object:
			drawTemporaryObject( view );

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
		drawTemporaryObject( view );
		m_isDragging = true;

		return true;
	}

	return false;
}
