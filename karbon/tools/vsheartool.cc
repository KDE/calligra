/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vsheartool.h"
#include "vtransformcmd.h"


VShearTool::VShearTool( KarbonView* view ) : VTool( view )
{
}

VShearTool::~VShearTool()
{
}

void
VShearTool::mouseReleased( QMouseEvent * )
{
	if( !m_isDragging ) return;

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );

	view()->part()->addCommand(
		new VShearCmd( &view()->part()->document(), fp * (1.0 / view()->zoom() ), m_s1, m_s2 ),
		true );

	m_isDragging = false;
}

void
VShearTool::mousePressed( QMouseEvent *mouse_event )
{
	view()->painterFactory()->painter()->end();

	m_fp.setX( mouse_event->pos().x() );
	m_fp.setY( mouse_event->pos().y() );
	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	m_activeNode = view()->part()->document().selection()->handleNode( mouse_event->pos() );

	// draw initial object:
	drawTemporaryObject();
	m_isDragging = true;
}

void
VShearTool::activate()
{
	view()->statusMessage()->setText( i18n( "Shear" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VShearTool::setCursor( const QPoint &p ) const
{
	switch( view()->part()->document().selection()->handleNode( p ) )
	{
		case node_lt:
		case node_rb:
			view()->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeFDiagCursor ) );
			break;
		case node_rt:
		case node_lb:
			view()->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeBDiagCursor ) );
			break;
		case node_lm:
		case node_rm:
			view()->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeHorCursor ) );
			break;
		case node_mt:
		case node_mb:
			view()->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::SizeVerCursor ) );
			break;
		default:
			view()->canvasWidget()->viewport()->
				setCursor( QCursor( Qt::arrowCursor ) );
	}
}

void
VShearTool::drawTemporaryObject()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	KoRect rect = view()->part()->document().selection()->boundingBox();

	// already selected, so must be a handle operation (move, scale etc.)
	if( view()->part()->document().selection()->objects().count() > 0 && m_activeNode != node_mm )
	{
		if( m_activeNode == node_lt )
		{
		}
		else if( m_activeNode == node_mt )
		{
			m_s1 = 0;
			m_s2 = ( m_lp.y() - m_fp.y() ) / double( ( rect.height() / 2 ) * view()->zoom() );
		}
		else if( m_activeNode == node_rt )
		{
		}
		else if( m_activeNode == node_rm)
		{
			m_s1 = ( m_lp.x() - m_fp.x() ) / double( ( rect.width() / 2 ) * view()->zoom() );
			m_s2 = 0;
		}
		else if( m_activeNode == node_rb )
		{
		}
		else if( m_activeNode == node_mb )
		{
			m_s1 = 0;
			m_s2 = ( m_lp.y() - m_fp.y() ) / double( ( rect.height() / 2 ) * view()->zoom() );
		}
		else if( m_activeNode == node_lb )
		{
		}
		else if( m_activeNode == node_lm )
		{
			m_s1 = ( m_lp.x() - m_fp.x() ) / double( ( rect.width() / 2 ) * view()->zoom() );
			m_s2 = 0;
		}
		// shear operation
		QWMatrix mat;
		mat.translate( m_fp.x() / view()->zoom(), m_fp.y() / view()->zoom() );
		mat.shear( m_s1, m_s2 );
		mat.translate(	- ( m_fp.x() + view()->canvasWidget()->contentsX() ) / view()->zoom(),
						- ( m_fp.y() + view()->canvasWidget()->contentsY() ) / view()->zoom() );

		// TODO :  makes a copy of the selection, do assignment operator instead
		VObjectListIterator itr = view()->part()->document().selection()->objects();
		VObjectList list;
		list.setAutoDelete( true );
	    for( ; itr.current() ; ++itr )
		{
			list.append( itr.current()->clone() );
		}
		painter->setZoomFactor( view()->zoom() );
		VObjectListIterator itr2 = list;
		for( ; itr2.current() ; ++itr2 )
		{
			itr2.current()->transform( mat );
			itr2.current()->setState( VObject::edit );
			itr2.current()->draw( painter, itr2.current()->boundingBox() );
		}
		painter->setZoomFactor( 1.0 );
	}
	else
		m_isDragging = false;
}

