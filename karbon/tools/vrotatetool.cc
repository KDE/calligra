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
VRotateTool::activate()
{
	view()->statusMessage()->setText( i18n( "Rotate" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VRotateTool::draw()
{
// TODO: put the calculation stuff into recalc()
/*
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	// already selected, so must be a handle operation (move, scale etc.)
	if(
		view()->part()->document().selection()->objects().count() > 0 &&
		m_activeNode != node_mm )
	{
		KoRect rect = view()->part()->document().selection()->boundingBox();

		m_center = rect.center();

		m_angle = atan2( lp.y() - m_center.y(), lp.x() - m_center.x() );
		if( m_activeNode == node_lt )
			m_angle -= atan2( rect.top() - m_center.y(), rect.left() - m_center.x() );
		else if( m_activeNode == node_mt )
			m_angle += M_PI / 2;
		else if( m_activeNode == node_rt )
			m_angle -= atan2( rect.top() - m_center.y(), rect.right() - m_center.x() );
		else if( m_activeNode == node_rm)
		{
		}
		else if( m_activeNode == node_rb )
			m_angle -= atan2( rect.bottom() - m_center.y(), rect.right() - m_center.x() );
		else if( m_activeNode == node_mb )
			m_angle -= M_PI / 2;
		else if( m_activeNode == node_lb )
			m_angle -= atan2( rect.bottom() - m_center.y(), rect.left() - m_center.x() );
		else if( m_activeNode == node_lm )
		{
		}

		// rotate operation
		QWMatrix mat;
		mat.translate( m_center );
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

	view()->painterFactory()->painter()->end();
*/
}

void
VRotateTool::setCursor( const KoPoint& /*current*/ ) const
{
/*
	switch( view()->part()->document().selection()->handleNode( current ) )
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
*/
}

void
VRotateTool::mouseButtonPress( const KoPoint& current )
{
//	m_activeNode = view()->part()->document().selection()->handleNode( current ) );
}

void
VRotateTool::mouseDrag( const KoPoint& current )
{
	recalc();
}

void
VRotateTool::mouseDragRelease( const KoPoint& current )
{
	recalc();

	view()->part()->addCommand(
		new VRotateCmd( &view()->part()->document(), m_center, m_angle / VGlobal::pi_180 ),
		true );
}

void
VRotateTool::recalc()
{
}

