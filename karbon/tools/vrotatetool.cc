/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qcursor.h>

#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vglobal.h"
#include "vrotatetool.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vselection.h"
#include "vtransformcmd.h"

#include <kdebug.h>


VRotateTool* VRotateTool::s_instance = 0L;

VRotateTool::VRotateTool( KarbonPart* part )
	: VTool( part ), m_isDragging( false )
{
}

VRotateTool::~VRotateTool()
{
}

VRotateTool*
VRotateTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VRotateTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VRotateTool::setCursor( KarbonView* view ) const
{
/*
	switch( VHandleTool::instance( m_part )->activeNode() )
	{
		case node_lt:
		case node_rb:
			view->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeFDiagCursor ) );
			break;
		case node_rt:
		case node_lb:
			view->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeBDiagCursor ) );
			break;
		case node_lm:
		case node_rm:
			view->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeHorCursor ) );
			break;
		case node_mt:
		case node_mb:
			view->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeVerCursor ) );
			break;
		default:
			view->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::arrowCursor ) );
	}
*/
}

void
VRotateTool::drawTemporaryObject( KarbonView* view )
{
	VPainter *painter = view->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );
/*
	// already selected, so must be a handle operation (move, scale etc.)
	if(
		part()->document().selection()->objects().count() > 0 &&
		VHandleTool::instance( m_part )->activeNode() != node_mm )
	{
		setCursor( view );
		KoPoint lp = view->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );
		KoRect rect = part()->document().selection()->boundingBox();

		m_sp = KoPoint( int( rect.left() + rect.width() / 2 ), int( rect.top() + rect.height() / 2 ) );
		KoPoint sp(
			m_sp.x() - view->canvasWidget()->contentsX(),
			m_sp.y() - view->canvasWidget()->contentsY() );

		m_angle = atan2( lp.y() - m_sp.y(), lp.x() - m_sp.x() );
		if( VHandleTool::instance( m_part )->activeNode() == node_lt )
			m_angle -= atan2( rect.top() - m_sp.y(), rect.left() - m_sp.x() );
		else if( VHandleTool::instance( m_part )->activeNode() == node_mt )
			m_angle += M_PI / 2;
		else if( VHandleTool::instance( m_part )->activeNode() == node_rt )
			m_angle -= atan2( rect.top() - m_sp.y(), rect.right() - m_sp.x() );
		else if( VHandleTool::instance( m_part )->activeNode() == node_rm)
		{
		}
		else if( VHandleTool::instance( m_part )->activeNode() == node_rb )
			m_angle -= atan2( rect.bottom() - m_sp.y(), rect.right() - m_sp.x() );
		else if( VHandleTool::instance( m_part )->activeNode() == node_mb )
			m_angle -= M_PI / 2;
		else if( VHandleTool::instance( m_part )->activeNode() == node_lb )
			m_angle -= atan2( rect.bottom() - m_sp.y(), rect.left() - m_sp.x() );
		else if( VHandleTool::instance( m_part )->activeNode() == node_lm )
		{
		}
		// rotate operation
		QWMatrix mat;
		mat.translate( sp.x() / view->zoom(), sp.y() / view->zoom());
		mat.rotate( m_angle / VGlobal::pi_180 );
		mat.translate(	- ( sp.x() + view->canvasWidget()->contentsX() ) / view->zoom(),
						- ( sp.y() + view->canvasWidget()->contentsY() ) / view->zoom() );

		// TODO :  makes a copy of the selection, do assignment operator instead
		VObjectListIterator itr = part()->document().selection()->objects();
		VObjectList list;
		list.setAutoDelete( true );
	    for( ; itr.current() ; ++itr )
		{
			list.append( itr.current()->clone() );
		}
		painter->setZoomFactor( view->zoom() );
		VObjectListIterator itr2 = list;
		for( ; itr2.current() ; ++itr2 )
		{
			if( VPath* path = dynamic_cast<VPath*>( itr2.current() ) )
			{
				//path->insertKnots( 5 );
//				path->convertToCurves();
//				path->whirlPinch( KoPoint( sp.x() / view->zoom(), sp.y() / view->zoom() ), m_angle / VGlobal::pi_180, 1.0 );
			}
			itr2.current()->transform( mat );
			itr2.current()->setState( state_edit );
			itr2.current()->draw( painter, itr2.current()->boundingBox() );
		}
		painter->setZoomFactor( 1.0 );
	}
	else*/
		m_isDragging = false;
}

bool
VRotateTool::eventFilter( KarbonView* view, QEvent* event )
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

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		part()->addCommand(
			new VRotateCmd( &part()->document(), m_sp * (1.0 / view->zoom() ), m_angle / VGlobal::pi_180 ),
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

