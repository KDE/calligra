/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qevent.h>
#include <qpainter.h>
#include <qwidget.h>

#include "karbon_view.h"
#include "vccmd_rectangle.h"
#include "vctool_rectangle.h"
#include "vpoint.h"

// just for testing:
#include <kdebug.h>
#include <qmessagebox.h>
VCToolRectangle* VCToolRectangle::s_instance = 0L;

VCToolRectangle::VCToolRectangle( KarbonPart* part )
	: m_part( part ), m_isDragging( false )
{
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
	if ( event->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_QRect.setLeft( mouse_event->pos().x() );
		m_QRect.setTop( mouse_event->pos().y() );
		m_QRect.setRight( mouse_event->pos().x() );
		m_QRect.setBottom( mouse_event->pos().y() );

		m_isDragging = true;

		return true;
	}

	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		QPainter painter( view->canvas()->viewport() );
		painter.setPen( Qt::black );

		painter.setRasterOp( Qt::NotROP );

		// erase the old rect:
		painter.drawRect( m_QRect );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_QRect.setRight( mouse_event->pos().x() );
		m_QRect.setBottom( mouse_event->pos().y() );

		// paint the new rect:
		painter.drawRect( m_QRect );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease )
	{
		if ( m_isDragging )
		{
			QPainter painter( view->canvas()->viewport() );
			painter.setPen( Qt::black );

			painter.setRasterOp( Qt::NotROP );

			// erase the old rect:
			painter.drawRect( m_QRect );

			// temp vpoints (zoomFactor):
			VPoint tl;
			VPoint br;
			tl.setFromQPoint( m_QRect.topLeft(), view->canvas()->zoomFactor() );
			br.setFromQPoint( m_QRect.bottomRight(), view->canvas()->zoomFactor() );

			m_part->addCommand(
				new VCCmdRectangle( m_part, tl.x(), tl.y(), br.x(), br.y() ) );

// TODO: how to repaint?
//view->repaint();

			m_isDragging = false;
		}
		else
		{
QMessageBox::warning( view, "Karbon",
"You didnt move. I would ask for values in this case normally.\n\n",
"Try again", "Quit", 0, 0, 1 );
		}
	}

	return false;
}
