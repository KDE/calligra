/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>

#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vhandletool.h"

#include <kdebug.h>

#define HANDLE_SIZE 3


VHandleTool* VHandleTool::s_instance = 0L;

VHandleTool::VHandleTool( KarbonPart* part )
	: VTool( part )
{
	m_activeNode = NODE_MM;
}

VHandleTool::~VHandleTool()
{
}

VHandleTool*
VHandleTool::instance( KarbonPart* part )
{
	if ( s_instance == 0L )
	{
		s_instance = new VHandleTool( part );
	}

	s_instance->m_part = part;
	return s_instance;
}

void
VHandleTool::drawBox( QPainter& painter, short index )
{
	painter.setPen( Qt::blue.light() );
	painter.setBrush( Qt::white );
	painter.drawRect( m_nodes[ index ].toQRect() );
}

KoRect
computeRect( double cx, double cy, const double zoomFactor )
{
	return KoRect( KoPoint( cx - HANDLE_SIZE /* zoomFactor */, cy - HANDLE_SIZE /* zoomFactor */ ),
				  KoPoint( cx + HANDLE_SIZE /* zoomFactor */, cy + HANDLE_SIZE /* zoomFactor */ ) );
}

void
VHandleTool::draw( QPainter& painter, const double zoomFactor )
{
	painter.setBrush( Qt::NoBrush );
	painter.setPen( Qt::blue.light() );
	//QWMatrix mat = painter.worldMatrix();
	//painter.setWorldMatrix( mat.scale( 1 / zoomFactor, 1 / zoomFactor ) );

	if( part()->document().selection().count() > 0 )
	{
		m_boundingBox = part()->document().selection().boundingBox();
		kdDebug() << " x : " << m_boundingBox.x() << ", " << m_boundingBox.y() << ", " << m_boundingBox.width() << ", " << m_boundingBox.height() << endl;
		painter.drawRect( m_boundingBox.toQRect() );

		// draw boxes
		m_nodes[ NODE_LT ] = computeRect( m_boundingBox.left(), m_boundingBox.top(), zoomFactor );
		drawBox( painter, NODE_LT);
		m_nodes[ NODE_MT ] = computeRect( m_boundingBox.left() + m_boundingBox.width() / 2.0, m_boundingBox.top(), zoomFactor );
		drawBox( painter, NODE_MT);
		m_nodes[ NODE_RT ] = computeRect( m_boundingBox.right(), m_boundingBox.top(), zoomFactor );
		drawBox( painter, NODE_RT);
		m_nodes[ NODE_RM ] = computeRect( m_boundingBox.right(), m_boundingBox.top() + m_boundingBox.height() / 2.0, zoomFactor );
		drawBox( painter, NODE_RM);
		m_nodes[ NODE_RB ] = computeRect( m_boundingBox.right(), m_boundingBox.bottom(), zoomFactor );
		drawBox( painter, NODE_RB);
		m_nodes[ NODE_MB ] = computeRect( m_boundingBox.left() + m_boundingBox.width() / 2.0, m_boundingBox.bottom(), zoomFactor );
		drawBox( painter, NODE_MB);
		m_nodes[ NODE_LB ] = computeRect( m_boundingBox.left(), m_boundingBox.bottom(), zoomFactor );
		drawBox( painter, NODE_LB);
		m_nodes[ NODE_LM ] = computeRect( m_boundingBox.left(), m_boundingBox.top() + m_boundingBox.height() / 2.0, zoomFactor );
		drawBox( painter, NODE_LM);
	}
}

bool
VHandleTool::eventFilter( KarbonView* view, QEvent* event )
{
	kdDebug() << "VHandleTool::eventFilter" << endl;
	m_activeNode = NODE_MM;

	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
	KoPoint p = view->canvasWidget()->viewportToContents( QPoint( mouse_event->pos().x(), mouse_event->pos().y() ) );
	kdDebug() << "p.x() : " << p.x() << endl;
	kdDebug() << "p.y() : " << p.y() << endl;
	if( m_nodes[ NODE_LT ].contains( p ) )
	{
		m_activeNode = NODE_LT;
		return true;
	}
	else if( m_nodes[ NODE_MT ].contains( p ) )
	{
		m_activeNode = NODE_MT;
		return true;
	}
	else if( m_nodes[ NODE_RT ].contains( p ) )
	{
		m_activeNode = NODE_RT;
		return true;
	}
	else if( m_nodes[ NODE_RM ].contains( p ) )
	{
		m_activeNode = NODE_RM;
		return true;
	}
	else if( m_nodes[ NODE_RB ].contains( p ) )
	{
		m_activeNode = NODE_RB;
		return true;
	}
	else if( m_nodes[ NODE_MB ].contains( p ) )
	{
		m_activeNode = NODE_MB;
		return true;
	}
	else if( m_nodes[ NODE_LB ].contains( p ) )
	{
		m_activeNode = NODE_LB;
		return true;
	}
	else if( m_nodes[ NODE_LM ].contains( p ) )
	{
		m_activeNode = NODE_LM;
		return true;
	}

	return false;
}

short
VHandleTool::activeNode() const
{
	return m_activeNode;
}

