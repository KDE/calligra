/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vpainterfactory.h"
#include "vpainter.h"
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_scale.h"
#include "vmcmd_transform.h"

#include <math.h>

VMToolScale* VMToolScale::s_instance = 0L;

VMToolScale::VMToolScale( KarbonPart* part )
	: VTool( part ), m_isDragging( false )
{
}

VMToolScale::~VMToolScale()
{
}

VMToolScale*
VMToolScale::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolScale( part );
	}

	return s_instance;
}

void
VMToolScale::drawTemporaryObject( KarbonView* view )
{
	VPainter *painter = VPainterFactory::editpainter();
	painter->setRasterOp( Qt::NotROP );

	QRect rect =  part()->selection().boundingBox( view->zoomFactor() );
	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->selection().isEmpty()
		&& ( rect.contains( m_fp ) ) )
//		part()->selection()->boundingBox().contains( p /* view->zoomFactor() */ ) ) )
	{
		// rotate operation
		QWMatrix mat;
		mat.translate( m_fp.x(), m_fp.y() );
		m_s1 = ( m_lp.x() - m_fp.x() ) / double( rect.width() / 2 );
		m_s2 = ( m_lp.y() - m_fp.y() ) / double( rect.height() / 2 );
		mat.scale( m_s1, m_s2 );
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
}

bool
VMToolScale::eventFilter( KarbonView* view, QEvent* event )
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
		//QPoint fp = view->canvasWidget()->viewportToContents( m_fp );
		//QPoint lp = view->canvasWidget()->viewportToContents( m_lp );

		part()->addCommand(
			new VMCmdScale(
				part(),
				part()->selection(), m_fp, m_s1, m_s2 ),
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
