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
	m_activeNode = node_mm;
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

static KoRect
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
		const KoRect& bb = part()->document().selection().boundingBox();

		m_boundingBox.setCoords(
			bb.left() * zoomFactor,
			bb.top() * zoomFactor,
			bb.right() * zoomFactor,
			bb.bottom() * zoomFactor );

kdDebug() << " x : " << m_boundingBox.x() << ", " << m_boundingBox.y() << ", " << m_boundingBox.width() << ", " << m_boundingBox.height() << endl;
		painter.drawRect( m_boundingBox.toQRect() );

		// draw boxes
		painter.setPen( Qt::blue.light() );
		painter.setBrush( Qt::white );

		m_nodes[ node_lt ] = computeRect( m_boundingBox.left(), m_boundingBox.top(), zoomFactor );
		painter.drawRect( m_nodes[ node_lt ].toQRect() );

		m_nodes[ node_mt ] = computeRect( m_boundingBox.left() + m_boundingBox.width() / 2.0, m_boundingBox.top(), zoomFactor );
		painter.drawRect( m_nodes[ node_mt ].toQRect() );

		m_nodes[ node_rt ] = computeRect( m_boundingBox.right(), m_boundingBox.top(), zoomFactor );
		painter.drawRect( m_nodes[ node_rt ].toQRect() );

		m_nodes[ node_rm ] = computeRect( m_boundingBox.right(), m_boundingBox.top() + m_boundingBox.height() / 2.0, zoomFactor );
		painter.drawRect( m_nodes[ node_rm ].toQRect() );

		m_nodes[ node_rb ] = computeRect( m_boundingBox.right(), m_boundingBox.bottom(), zoomFactor );
		painter.drawRect( m_nodes[ node_rb ].toQRect() );

		m_nodes[ node_mb ] = computeRect( m_boundingBox.left() + m_boundingBox.width() / 2.0, m_boundingBox.bottom(), zoomFactor );
		painter.drawRect( m_nodes[ node_mb ].toQRect() );

		m_nodes[ node_lb ] = computeRect( m_boundingBox.left(), m_boundingBox.bottom(), zoomFactor );
		painter.drawRect( m_nodes[ node_lb ].toQRect() );

		m_nodes[ node_lm ] = computeRect( m_boundingBox.left(), m_boundingBox.top() + m_boundingBox.height() / 2.0, zoomFactor );
		painter.drawRect( m_nodes[ node_lm ].toQRect() );
	}
}

bool
VHandleTool::eventFilter( KarbonView* view, QEvent* event )
{
	kdDebug() << "VHandleTool::eventFilter" << endl;
	m_activeNode = node_mm;

	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
	KoPoint p = view->canvasWidget()->viewportToContents( QPoint( mouse_event->pos().x(), mouse_event->pos().y() ) );
	kdDebug() << "p.x() : " << p.x() << endl;
	kdDebug() << "p.y() : " << p.y() << endl;
	if( m_nodes[ node_lt ].contains( p ) )
	{
		m_activeNode = node_lt;
		return true;
	}
	else if( m_nodes[ node_mt ].contains( p ) )
	{
		m_activeNode = node_mt;
		return true;
	}
	else if( m_nodes[ node_rt ].contains( p ) )
	{
		m_activeNode = node_rt;
		return true;
	}
	else if( m_nodes[ node_rm ].contains( p ) )
	{
		m_activeNode = node_rm;
		return true;
	}
	else if( m_nodes[ node_rb ].contains( p ) )
	{
		m_activeNode = node_rb;
		return true;
	}
	else if( m_nodes[ node_mb ].contains( p ) )
	{
		m_activeNode = node_mb;
		return true;
	}
	else if( m_nodes[ node_lb ].contains( p ) )
	{
		m_activeNode = node_lb;
		return true;
	}
	else if( m_nodes[ node_lm ].contains( p ) )
	{
		m_activeNode = node_lm;
		return true;
	}

	return false;
}

