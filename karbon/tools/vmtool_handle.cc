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
	m_activeNode = NODE_MM;
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
VMToolHandle::drawBox( QPainter& painter, short index )
{
	painter.setPen( Qt::blue.light() );
	painter.setBrush( Qt::white );
	painter.drawRect( m_nodes[ index ] );
}

QRect
computeRect( int cx, int cy, const double zoomFactor )
{
	return QRect( QPoint( cx - 2 / zoomFactor, cy - 2 / zoomFactor ),
				  QPoint( cx + 2 / zoomFactor, cy + 2 / zoomFactor ) );
}

void
VMToolHandle::draw( QPainter& painter, const double zoomFactor )
{
	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );
	QWMatrix mat = painter.worldMatrix();
	painter.setWorldMatrix( mat.scale( zoomFactor, zoomFactor ) );

	if( part()->selection().count() > 0 )
	{
		m_bbox =  part()->selection().boundingBox( 1 );
		painter.drawRect( m_bbox );

		// draw boxes
		m_nodes[ NODE_LT ] = computeRect( m_bbox.left(), m_bbox.top(), zoomFactor );
		drawBox( painter, NODE_LT);
		m_nodes[ NODE_MT ] = computeRect( m_bbox.left() + m_bbox.width() / 2, m_bbox.top(), zoomFactor );
		drawBox( painter, NODE_MT);
		m_nodes[ NODE_RT ] = computeRect( m_bbox.right(), m_bbox.top(), zoomFactor );
		drawBox( painter, NODE_RT);
		m_nodes[ NODE_RM ] = computeRect( m_bbox.right(), m_bbox.top() + m_bbox.height() / 2, zoomFactor );
		drawBox( painter, NODE_RM);
		m_nodes[ NODE_RB ] = computeRect( m_bbox.right(), m_bbox.bottom(), zoomFactor );
		drawBox( painter, NODE_RB);
		m_nodes[ NODE_MB ] = computeRect( m_bbox.left() + m_bbox.width() / 2, m_bbox.bottom(), zoomFactor );
		drawBox( painter, NODE_MB);
		m_nodes[ NODE_LB ] = computeRect( m_bbox.left(), m_bbox.bottom(), zoomFactor );
		drawBox( painter, NODE_LB);
		m_nodes[ NODE_LM ] = computeRect( m_bbox.left(), m_bbox.top() + m_bbox.height() / 2, zoomFactor );
		drawBox( painter, NODE_LM);
	}
}

bool
VMToolHandle::eventFilter( KarbonView* view, QEvent* event )
{
	kdDebug() << "VMToolHandle::eventFilter" << endl;
	m_activeNode = NODE_MM;

	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
	if( m_nodes[ NODE_LT ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_LT;
		return true;
	}
	else if( m_nodes[ NODE_MT ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_MT;
		return true;
	}
	else if( m_nodes[ NODE_RT ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_RT;
		return true;
	}
	else if( m_nodes[ NODE_RM ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_RM;
		return true;
	}
	else if( m_nodes[ NODE_RB ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_RB;
		return true;
	}
	else if( m_nodes[ NODE_MB ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_MB;
		return true;
	}
	else if( m_nodes[ NODE_LB ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_LB;
		return true;
	}
	else if( m_nodes[ NODE_LM ].contains( mouse_event->pos() ) )
	{
		m_activeNode = NODE_LM;
		return true;
	}

	return false;
}

short
VMToolHandle::activeNode() const
{
	return m_activeNode;
}

