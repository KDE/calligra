/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_select.h"
#include "vmcmd_transform.h"

#include <math.h>

VMToolSelect* VMToolSelect::s_instance = 0L;

VMToolSelect::VMToolSelect( KarbonPart* part )
	: VTool( part ), m_state( normal ), m_isDragging( false )
{
}

VMToolSelect::~VMToolSelect()
{
}

VMToolSelect*
VMToolSelect::instance( KarbonPart* part )
{
	delete s_instance;

	//if ( s_instance == 0L )
	//{
		s_instance = new VMToolSelect( part );
	//}
	return s_instance;
}

void
VMToolSelect::drawTemporaryObject( KarbonView* view )
{
	VPainter *painter = view->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->selection().isEmpty()
		&& ( m_state != normal ||
			part()->selection().boundingBox( view->zoomFactor() ).contains( m_fp ) ) )
//		part()->selection()->boundingBox().contains( p /* view->zoomFactor() */ ) ) )
	{
		if( m_state != moving )
			m_state = moving;

		// move operation
		QWMatrix mat;
		mat.translate(	( m_lp.x() - m_fp.x() ) / view->zoomFactor(),
						( m_lp.y() - m_fp.y() ) / view->zoomFactor() );

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
	{
		painter->setPen( Qt::DotLine );
		painter->setZoomFactor( 1 );
		painter->moveTo( KoPoint( m_fp.x(), m_fp.y() ) );
		painter->lineTo( KoPoint( m_lp.x(), m_fp.y() ) );
		painter->lineTo( KoPoint( m_lp.x(), m_lp.y() ) );
		painter->lineTo( KoPoint( m_fp.x(), m_lp.y() ) );
		painter->lineTo( KoPoint( m_fp.x(), m_fp.y() ) );
		painter->setZoomFactor( view->zoomFactor() );
		painter->strokePath();

		m_state = normal;
	}
}

bool
VMToolSelect::eventFilter( KarbonView* view, QEvent* event )
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

		if( m_state == moving )
		{
			m_state = normal;
			part()->addCommand(
				new VMCmdTranslate(
					part(),
					part()->selection(),
					qRound( ( lp.x() - fp.x() ) / view->zoomFactor() ),
					qRound( ( lp.y() - fp.y() ) / view->zoomFactor() ) ),
				true );

//			part()->repaintAllViews();
		}
		else
		{
			// erase old object:
			drawTemporaryObject( view );

			part()->deselectAllObjects();

			part()->selectObjectsWithinRect(
				QRect(
					qRound( /*view->zoomFactor() */ fp.x() ),
					qRound( /*view->zoomFactor() */ fp.y() ),
					qRound( /*view->zoomFactor() */ ( lp.x() - fp.x() ) ),
					qRound( /*view->zoomFactor() */ ( lp.y() - fp.y() ) ) ).normalize(),
				view->zoomFactor(),
				true );
				
			//if( part()->selection().count() > 0  )
				part()->repaintAllViews();
		}

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
