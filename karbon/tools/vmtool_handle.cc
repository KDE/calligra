/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vmtool_handle.h"

#include <kdebug.h>

VMToolHandle* VMToolHandle::s_instance = 0L;

VMToolHandle::VMToolHandle( KarbonPart* part )
	: VTool( part )
{
}

VMToolHandle::~VMToolHandle()
{
}

VMToolHandle*
VMToolHandle::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VMToolHandle( part );
	}

	return s_instance;
}

void
VMToolHandle::draw( QPainter& painter, const double zoomFactor )
{
	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );

	if( part()->selection().count() > 0 )
		painter.drawRect(
			part()->selection().boundingBox( zoomFactor ) );

/*
	// draw boxes
	painter.setPen( Qt::blue.light() );
	painter.setBrush( Qt::white );
	drawBox( painter, m_bbox.left(), m_bbox.top() );
	drawBox( painter, m_bbox.left() + m_bbox.width() / 2, m_bbox.top() );
	drawBox( painter, m_bbox.right(), m_bbox.top() );
	drawBox( painter, m_bbox.right(), m_bbox.top() + m_bbox.height() / 2 );
	drawBox( painter, m_bbox.right(), m_bbox.bottom() );
	drawBox( painter, m_bbox.left() + m_bbox.width() / 2, m_bbox.bottom() );
	drawBox( painter, m_bbox.left(), m_bbox.bottom() );
	drawBox( painter, m_bbox.left(), m_bbox.top() + m_bbox.height() / 2 );
*/
}

bool
VMToolHandle::eventFilter( KarbonView* view, QEvent* event )
{
	return false;
}
