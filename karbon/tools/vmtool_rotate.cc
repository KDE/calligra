/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vglobal.h"
#include "vmtool_rotate.h"
#include "vmtool_handle.h"
#include "vmcmd_transform.h"
#include "vpath.h"

#include <math.h>

#include <qcursor.h>

#include <kdebug.h>

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

	s_instance->m_part = part;
	return s_instance;
}

void
VMToolRotate::setCursor( KarbonView* view ) const
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
VMToolRotate::drawTemporaryObject( KarbonView* view )
{
	VPainter *painter = view->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->selection().isEmpty() && VMToolHandle::instance( m_part )->activeNode() != NODE_MM )
	{
		setCursor( view );
		KoPoint lp = view->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );
		KoRect rect = part()->selection().boundingBox( 1 / view->zoomFactor() );
		m_sp = KoPoint( int( rect.left() + rect.width() / 2 ), int( rect.top() + rect.height() / 2 ) );
		KoPoint sp( m_sp.x() - view->canvasWidget()->contentsX(), m_sp.y() - view->canvasWidget()->contentsY() );
		m_angle = atan2( lp.y() - m_sp.y(), lp.x() - m_sp.x() );
		if( VMToolHandle::instance( m_part )->activeNode() == NODE_LT )
			m_angle -= atan2( rect.top() - m_sp.y(), rect.left() - m_sp.x() );
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_MT )
			m_angle += M_PI / 2;
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_RT )
			m_angle -= atan2( rect.top() - m_sp.y(), rect.right() - m_sp.x() );
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_RM)
		{
		}
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_RB )
			m_angle -= atan2( rect.bottom() - m_sp.y(), rect.right() - m_sp.x() );
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_MB )
			m_angle -= M_PI / 2;
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_LB )
			m_angle -= atan2( rect.bottom() - m_sp.y(), rect.left() - m_sp.x() );
		else if( VMToolHandle::instance( m_part )->activeNode() == NODE_LM )
		{
		}
		// rotate operation
		QWMatrix mat;
		mat.translate( sp.x() / view->zoomFactor(), sp.y() / view->zoomFactor());
		mat.rotate( m_angle / VGlobal::pi_180 );
		mat.translate(	- ( sp.x() + view->canvasWidget()->contentsX() ) / view->zoomFactor(),
						- ( sp.y() + view->canvasWidget()->contentsY() ) / view->zoomFactor() );

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
			if( VPath* path = dynamic_cast<VPath*>( itr2.current() ) )
			{
				//path->insertKnots( 5 );
//				path->convertToCurves();
//				path->whirlPinch( KoPoint( sp.x() / view->zoomFactor(), sp.y() / view->zoomFactor() ), m_angle / VGlobal::pi_180, 1.0 );
			}
			//itr2.current()->transform( mat );
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
VMToolRotate::eventFilter( KarbonView* view, QEvent* event )
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

		part()->addCommand(
			new VMCmdRotate(
				part(),
				part()->selection(), m_sp * (1.0 / view->zoomFactor() ), m_angle / VGlobal::pi_180 ),
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
