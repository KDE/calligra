/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qevent.h>
#include <qpainter.h>
#include <qwidget.h>

#include "karbon_view.h"
#include "vctool_rectangle.h"

#include <kdebug.h>

VCToolRectangle* VCToolRectangle::s_instance = 0L;

VCToolRectangle::VCToolRectangle( KarbonPart* part )
	: m_part( part ), m_isDown( false )
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
kdDebug() << "p" << endl;
		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_QRect.moveTopLeft( mouse_event->pos() );
		m_QRect.moveBottomRight( mouse_event->pos() );
		m_isDown = true;

		return true;
	}

	if ( event->type() == QEvent::MouseMove && m_isDown )
	{
kdDebug() << "m" << endl;
		QPainter painter( view->canvas() );
		painter.setPen( Qt::black );

//		painter.setRasterOp( Qt::XorROP );

		// erase the old rect:
		painter.drawRect( m_QRect );

		QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
		m_QRect.moveBottomRight( mouse_event->pos() );

		// paint the new rect:
		painter.drawRect( m_QRect );

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease )
	{
		m_isDown = false;
	}

	return false;
}
