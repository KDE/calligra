/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vccmd_rectangle.h"	// command
#include "vcdlg_rectangle.h"	// dialog
#include "vctool_rectangle.h"
#include "vpoint.h"

VCToolRectangle* VCToolRectangle::s_instance = 0L;

// to improve performance, we use Qt/X11 primitives/commands where
// ever possible, instead of using our vpaths.

VCToolRectangle::VCToolRectangle( KarbonPart* part )
	: m_part( part ), m_isDragging( false ), m_isSquare( false ),
	  m_isCentered( false ), m_round( 0.0 )
{
	// create config dialog:
	m_dialog = new VCDlgRectangle();
	m_dialog->setValueWidth( 100.0 );
	m_dialog->setValueHeight( 100.0 );
	m_dialog->setValueRound( m_round );
}

VCToolRectangle::~VCToolRectangle()
{
	delete( m_dialog );
}

VCToolRectangle*
VCToolRectangle::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VCToolRectangle( part );
	}

	return s_instance;
}

bool
VCToolRectangle::eventFilter( KarbonView* view, QEvent* event )
{

	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old rect:
		drawTemporaryRect( view );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		recalcRect();

		// paint new rect:
		drawTemporaryRect( view );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		m_isDragging = false;
		m_isSquare = false;
		m_isCentered = false;

		// erase old rect:
		drawTemporaryRect( view );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// did we drag the mouse?
		if ( m_fp == m_lp )
		{
			// we didnt drag => show a config-dialog:
			if ( m_dialog->exec() )
			{
				// temp vpoint (zoomFactor):
				VPoint tl;
				tl.setFromQPoint( m_fp, view->zoomFactor() );

				m_round = m_dialog->valueRound();

				m_part->addCommand(
					new VCCmdRectangle( m_part,
						tl.x(), tl.y(),
						tl.x() + m_dialog->valueWidth(),
						tl.y() + m_dialog->valueHeight(), m_round ) );
			}
		}
		else
		{
			// temp vpoints (zoomFactor):
			VPoint tl;
			VPoint br;
			tl.setFromQPoint( m_rect.topLeft(), view->zoomFactor() );
			br.setFromQPoint( m_rect.bottomRight(), view->zoomFactor() );

			m_part->addCommand(
				new VCCmdRectangle( m_part, tl.x(), tl.y(), br.x(), br.y(), m_round ) );
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

			// erase old rect:
			drawTemporaryRect( view );

			return true;
		}

		// if SHIFT is pressed, we want a square:
		if ( key_event->key() == Qt::Key_Shift )
		{
			m_isSquare = true;

			if ( m_isDragging )
			{
				// erase old rect:
				drawTemporaryRect( view );
				recalcRect();
				// draw new old rect:
				drawTemporaryRect( view );
			}

			return true;
		}

		// if Ctrl is pressed, we want a centered path:
		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = true;

			if ( m_isDragging )
			{
				// erase old rect:
				drawTemporaryRect( view );
				recalcRect();
				// draw new old rect:
				drawTemporaryRect( view );
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
				// erase old rect:
				drawTemporaryRect( view );
				recalcRect();
				// draw new old rect:
				drawTemporaryRect( view );
			}

			return true;
		}

		if ( key_event->key() == Qt::Key_Control )
		{
			m_isCentered = false;

			if ( m_isDragging )
			{
				// erase old rect:
				drawTemporaryRect( view );
				recalcRect();
				// draw new old rect:
				drawTemporaryRect( view );
			}

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_fp.setX( mouse_event->pos().x() );
		m_fp.setY( mouse_event->pos().y() );

		// set initial rect:
		m_rect.setLeft( m_fp.x() );
		m_rect.setTop( m_fp.y() );
		m_rect.setRight( m_fp.x() + 1 );
		m_rect.setBottom( m_fp.y() + 1 );

		// draw initial rect:
		drawTemporaryRect( view );

		m_isDragging = true;

		return true;
	}

	return false;
}


