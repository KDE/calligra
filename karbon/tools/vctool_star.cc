/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vccmd_star.h"	// command
#include "vcdlg_star.h"	// dialog
#include "vctool_star.h"

VCToolStar* VCToolStar::s_instance = 0L;

VCToolStar::VCToolStar( KarbonPart* part )
	: m_part( part ), m_isDragging( false ), m_isSquare( false ),
		m_isCentered( false ), m_width( 0 ), m_height( 0 )
{
	// create config dialog:
	m_dialog = new VCDlgStar();
	m_dialog->setValueOuterR( 100.0 );
	m_dialog->setValueInnerR( 50.0 );
	m_dialog->setValueEdges( 3 );
}

VCToolStar::~VCToolStar()
{
	delete( m_dialog );
}

VCToolStar*
VCToolStar::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolStar( part );
	}

	return s_instance;
}

bool
VCToolStar::eventFilter( KarbonView* view, QEvent* event )
{

	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject( view );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp = view->canvasWidget()->viewportToContents( mouse_event->pos() );

		recalcCoords();

		// paint new object:
		drawTemporaryObject( view );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		m_isDragging = false;
		m_isSquare = false;
		m_isCentered = false;

		// erase old object:
		drawTemporaryObject( view );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*>( event );
		m_lp = view->canvasWidget()->viewportToContents( mouse_event->pos() );

		// did we drag the mouse?
		if ( m_fp == m_lp )
		{
			// we didnt drag => show a config-dialog:
			if ( m_dialog->exec() )
			{
				m_part->addCommand(
					new VCCmdStar( m_part,
						m_fp.x(), m_fp.y(),
						m_dialog->valueOuterR(),
						m_dialog->valueInnerR(),
						m_dialog->valueEdges() ) );
			}
		}
		else
		{
			m_part->addCommand(
				new VCCmdStar( m_part, m_tl.x(), m_tl.y(),
					qRound( m_width / 2 ),
					qRound( m_dialog->valueInnerR() *
						m_width / ( m_dialog->valueOuterR() * 2 ) ),
					m_dialog->valueEdges() )
 );
		}

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
			drawTemporaryObject( view );

			return true;
		}

		// if SHIFT is pressed, we want a square:
		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = true;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view );
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
				drawTemporaryObject( view );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view );
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
				drawTemporaryObject( view );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view );
			}

			return true;
		}

		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = false;

			if ( m_isDragging )
			{
				// erase old object:
				drawTemporaryObject( view );
				recalcCoords();
				// draw new old object:
				drawTemporaryObject( view );
			}

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_fp = view->canvasWidget()->viewportToContents( mouse_event->pos() );

		// set initial object:
		m_tl.setX( m_fp.x() );
		m_tl.setY( m_fp.y() );
		m_br.setX( m_fp.x() + 1 );
		m_br.setY( m_fp.y() + 1 );

		// draw initial object:
		drawTemporaryObject( view );

		m_isDragging = true;

		return true;
	}

	return false;
}


