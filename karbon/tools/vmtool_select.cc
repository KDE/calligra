/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>

#include "karbon_view.h"
#include "vmtool_select.h"
#include "vmcmd_select.h"
#include "vmcmd_transform.h"
#include "vhandle.h"

#include <kdebug.h>

VMToolSelect* VMToolSelect::s_instance = 0L;

VMToolSelect::VMToolSelect( KarbonPart* part )
	: VTool( part )
{
	m_TransformState = NoTransform;
}

VMToolSelect::~VMToolSelect()
{
}

VMToolSelect*
VMToolSelect::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolSelect( part );
	}

	return s_instance;
}

void
VMToolSelect::drawTemporaryObject(
	KarbonView* view, const QPoint& p, double d1, double d2 )
{
	QPainter painter( view->canvasWidget()->viewport() );
	// already selected, so must be a handle operation (move, scale etc.)
	if( !part()->handle()->objects().isEmpty() && part()->handle()->boundingBox().contains( p ) )
	{
		if( m_TransformState != Moving )
			m_TransformState = Moving;
		QRect rect = part()->handle()->boundingBox();
		kdDebug() << "p.x() : " << p.x() << endl;
		kdDebug() << "p.y() : " << p.y() << endl;
		kdDebug() << "d1 : " << d1 << endl;
		kdDebug() << "d2 : " << d2 << endl;
		kdDebug() << "rect.y() : " << rect.y() << endl;
		kdDebug() << "rect.y() : " << rect.y() << endl;
		kdDebug() << "rect.width() : " << rect.width() << endl;
		kdDebug() << "rect.height() : " << rect.height() << endl;
		painter.setRasterOp( Qt::NotROP );
		painter.translate( ( p.x() + d1 ) - ( rect.x() + rect.width() / 2 ),
							( p.y() + d2 ) - ( rect.y() + rect.height() / 2 ) );
		kdDebug() << "Middle x : " << ( p.x() + d1 ) - ( rect.x() + rect.width() / 2 ) << endl;
		kdDebug() << "Middle y : " << ( p.y() + d2 ) - ( rect.y() + rect.height() / 2 ) << endl;
		QPtrList<VObject> objects = part()->handle()->objects();
		QPtrListIterator<VObject> itr( objects );
		for ( ; itr.current() ; ++itr )
			itr.current()->draw( painter, rect, 1.0 );
		part()->drawHandle( painter );
	}
	else
	{
		painter.save();
		painter.setPen( Qt::DotLine );
		painter.setRasterOp( Qt::NotROP );

		painter.moveTo( p.x(), p.y() );
		painter.lineTo( p.x() + d1, p.y() );
		painter.lineTo( p.x() + d1, p.y() + d2 );
		painter.lineTo( p.x(), p.y() + d2 );
		painter.lineTo( p.x(), p.y() );

		painter.restore();
	}
}

VCommand*
VMToolSelect::createCmd( const QPoint& p, double d1, double d2 )
{
	if( m_TransformState == Moving )
	{
		QWMatrix mat;
		QRect rect = part()->handle()->boundingBox();
		mat.translate( ( p.x() + d1 ) - ( rect.x() + rect.width() / 2 ),
						( p.y() + d2 ) - ( rect.y() + rect.height() / 2 ) );
		m_TransformState = NoTransform;
		return
			new VMCmdTransform( part(), part()->handle()->objects(), mat );
	}
	else
	{
		// we dont want select to be undoable
		part()->unselectObjects();
		VMCmdSelect cmd( part(), p.x(), p.y(), p.x() + d1, p.y() + d2 );
		cmd.execute();
		return 0L;
	}
}

/*bool
VMToolSelect::eventFilter( KarbonView* view, QEvent* event )
{
	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject( view, m_p, m_d1, m_d2 );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		// paint new object:
		drawTemporaryObject( view, m_p, m_d1, m_d2 );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcCoords();

		VCommand* cmd = createCmd( m_p, m_d1, m_d2 );

		if( cmd )
			part()->addCommand( cmd );
		else
			// erase old object:
			drawTemporaryObject( view, m_p, m_d1, m_d2 );

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
			drawTemporaryObject( view, m_p, m_d1, m_d2 );

			return true;
		}

		// if SHIFT is pressed, we want a square:
		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = true;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
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
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
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
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
			}

			return true;
		}

		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = false;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view, m_p, m_d1, m_d2 );
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
		drawTemporaryObject( view, m_p, m_d1, m_d2 );

		m_isDragging = true;

		return true;
	}

	return false;
}*/

