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
#include "vglobal.h"
#include "vrotatetool.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vpath.h"
#include "vtransformcmd.h"

#include <kdebug.h>


VRotateTool::VRotateTool( KarbonView* view )
	: VTool( view )
{
}

VRotateTool::~VRotateTool()
{
}

void
VRotateTool::mouseReleased( QMouseEvent * )
{
	if( !m_isDragging ) return;

	view()->part()->addCommand(
		new VRotateCmd( &view()->part()->document(), m_sp, m_angle / VGlobal::pi_180 ),
		true );

	m_isDragging = false;
}

void
VRotateTool::mousePressed( QMouseEvent *mouse_event )
{
	view()->painterFactory()->painter()->end();

	m_fp.setX( mouse_event->pos().x() );
	m_fp.setY( mouse_event->pos().y() );
	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	m_activeNode = view()->part()->document().selection()->handleNode( QPoint( m_lp.x() / view()->zoom(), m_lp.y() / view()->zoom() ) );

	// draw initial object:
	drawTemporaryObject();
	m_isDragging = true;
}

void
VRotateTool::activate()
{
	view()->statusMessage()->setText( i18n( "Rotate" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VRotateTool::setCursor( const QPoint &p ) const
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
VRotateTool::drawTemporaryObject()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	// already selected, so must be a handle operation (move, scale etc.)
	if( view()->part()->document().selection()->objects().count() > 0 && m_activeNode != node_mm )
	{
		KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );
		KoRect rect = view()->part()->document().selection()->boundingBox();

		m_sp = KoPoint( int( rect.left() + rect.width() / 2 ), int( rect.top() + rect.height() / 2 ) );
		KoPoint sp(
			m_sp.x() - view()->canvasWidget()->contentsX(),
			m_sp.y() - view()->canvasWidget()->contentsY() );

		m_angle = atan2( lp.y() - m_sp.y(), lp.x() - m_sp.x() );
		if( m_activeNode == node_lt )
			m_angle -= atan2( rect.top() - m_sp.y(), rect.left() - m_sp.x() );
		else if( m_activeNode == node_mt )
			m_angle += M_PI / 2;
		else if( m_activeNode == node_rt )
			m_angle -= atan2( rect.top() - m_sp.y(), rect.right() - m_sp.x() );
		else if( m_activeNode == node_rm)
		{
		}
		else if( m_activeNode == node_rb )
			m_angle -= atan2( rect.bottom() - m_sp.y(), rect.right() - m_sp.x() );
		else if( m_activeNode == node_mb )
			m_angle -= M_PI / 2;
		else if( m_activeNode == node_lb )
			m_angle -= atan2( rect.bottom() - m_sp.y(), rect.left() - m_sp.x() );
		else if( m_activeNode == node_lm )
		{
		}
		// rotate operation
		QWMatrix mat;
		mat.translate( sp.x(), sp.y() );
		mat.rotate( m_angle / VGlobal::pi_180 );
		mat.translate(	- ( sp.x() + view()->canvasWidget()->contentsX() ),
						- ( sp.y() + view()->canvasWidget()->contentsY() ) );

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
			if( VPath* path = dynamic_cast<VPath*>( itr2.current() ) )
			{
				//path->insertKnots( 5 );
//				path->convertToCurves();
//				path->whirlPinch( KoPoint( sp.x() / view()->zoom(), sp.y() / view()->zoom() ), m_angle / VGlobal::pi_180, 1.0 );
			}
			itr2.current()->transform( mat );
			itr2.current()->setState( VObject::edit );
			itr2.current()->draw( painter, itr2.current()->boundingBox() );
		}
		painter->setZoomFactor( 1.0 );
	}
	else
		m_isDragging = false;
}

