/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_shear.h"
#include "vmtool_handle.h"
#include "vmcmd_transform.h"

#include <math.h>

#include <qcursor.h>

VMToolShear* VMToolShear::s_instance = 0L;

VMToolShear::VMToolShear( KarbonPart* part )
	: VTool( part ), m_isDragging( false )
{
}

VMToolShear::~VMToolShear()
{
}

VMToolShear*
VMToolShear::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolShear( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VMToolShear::setCursor( KarbonView* view ) const
{
	switch( VMToolHandle::instance( m_part )->activeNode() )
	{
	case NODE_LT:
	case NODE_RB:	view->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeFDiagCursor ) );
					break;
	case NODE_RT:
	case NODE_LB:	view->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeBDiagCursor ) );
					break;
	case NODE_LM:
	case NODE_RM:	view->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeHorCursor ) );
					break;
	case NODE_MT:
	case NODE_MB:	view->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeVerCursor ) );
					break;
	default:		view->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	}
}

void
VMToolShear::drawTemporaryObject( KarbonView* view )
{
	VPainter *painter = view->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	QPoint fp = view->canvasWidget()->viewportToContents( m_fp );

	QRect rect =  part()->selection().boundingBox( view->zoomFactor() );
	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->selection().isEmpty() && ( rect.contains( fp ) ) )
	{
		// shear operation
		QWMatrix mat;
		mat.translate( m_fp.x() / view->zoomFactor(), m_fp.y() / view->zoomFactor() );
		m_s1 = ( m_lp.x() - m_fp.x() ) / double( rect.width() / 2 );
		m_s2 = ( m_lp.y() - m_fp.y() ) / double( rect.height() / 2 );
		mat.shear( m_s1, m_s2 );
		mat.translate(	- ( m_fp.x() + view->canvasWidget()->contentsX() ) / view->zoomFactor(),
						- ( m_fp.y() + view->canvasWidget()->contentsY() ) / view->zoomFactor() );

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
			itr2.current()->setState( state_edit );
			itr2.current()->draw(
				painter,
				itr2.current()->boundingBox( view->zoomFactor() ),
				view->zoomFactor() );
		}
	}
	else
		m_isDragging = false;
}

bool
VMToolShear::eventFilter( KarbonView* view, QEvent* event )
{
	if ( event->type() == QEvent::MouseMove )
	{
		if( m_isDragging )
		{
			// erase old object:
			drawTemporaryObject( view );

			QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
			m_lp.setX( mouse_event->pos().x() );
			m_lp.setY( mouse_event->pos().y() );

			// paint new object:
			drawTemporaryObject( view );
		}
		else
		{
			VMToolHandle::instance( m_part )->eventFilter( view, event );
			setCursor( view );
		}

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		QPoint fp;
		fp.setX( view->canvasWidget()->viewportToContents( m_fp ).x() );
		fp.setY( view->canvasWidget()->viewportToContents( m_fp ).y() );

		part()->addCommand(
			new VMCmdShear(
				part(),
				part()->selection(), fp / view->zoomFactor(), m_s1, m_s2 ),
			true );

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
		view->painterFactory()->painter()->end();
        VMToolHandle::instance( m_part )->eventFilter( view, event );
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
