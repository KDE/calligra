/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <math.h>

#include <qcursor.h>
#include <qlabel.h>

#include <klocale.h>
#include <koPoint.h>
#include <koRect.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vselecttool.h"
#include "vtransformcmd.h"

#include <kdebug.h>


VSelectTool::VSelectTool( KarbonView* view )
	: VTool( view ), m_state( normal )
{
	m_lock = none;
	m_objects.setAutoDelete( true );
}

VSelectTool::~VSelectTool()
{
}

void
VSelectTool::activate()
{
	//if( m_state == normal )
		view()->statusMessage()->setText( i18n( "Select" ) );
	//else
//		view()->statusMessage()->setText( i18n( "Scale" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	view()->part()->document().selection()->setState( VObject::selected );
	view()->part()->document().selection()->clearNodes();
}

void
VSelectTool::draw()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	KoPoint current = m_current;
	current.setY( -current.y() + view()->canvasWidget()->viewport()->height() );

	KoRect rect = view()->part()->document().selection()->boundingBox();

	if( m_state != normal || rect.contains( current ) )
	{
		if( m_state == normal )
			m_state = moving;

		VObjectListIterator itr = m_objects;
		for( ; itr.current(); ++itr )
		{
			itr.current()->draw( painter, itr.current()->boundingBox() );
		}

		painter->setZoomFactor( 1.0 );
	}
	else if( m_state == normal )
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( first( true ).x(), first( true ).y() ) );
		painter->lineTo( KoPoint( m_current.x(), first( true ).y() ) );
		painter->lineTo( KoPoint( m_current.x(), m_current.y() ) );
		painter->lineTo( KoPoint( first( true ).x(), m_current.y() ) );
		painter->lineTo( KoPoint( first( true ).x(), first( true ).y() ) );
		painter->strokePath();

		m_state = normal;
	}
/*

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
	fp.setY( -fp.y() + view()->canvasWidget()->viewport()->height() );
	KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x() / view()->zoom(), m_lp.y() / view()->zoom() ) );

	KoRect rect = view()->part()->document().selection()->boundingBox();

	kdDebug() << " x: " << rect.x() << " y: " << rect.y() << " rect.width: " << rect.width() << " rect.height: " << rect.height() << endl;
	if( view()->part()->document().selection()->objects().count() > 0 &&
		( m_state != normal || m_activeNode != node_none || rect.contains( fp * ( 1.0 /  view()->zoom() ) ) ) )
	{
		if( m_state == normal )
			m_state = ( m_activeNode == node_none ) ? moving : scaling;

		// move operation
		QWMatrix mat;
		if( m_state == moving )
			mat.translate(	( m_lp.x() - fp.x() ) / view()->zoom(),
							( m_lp.y() - fp.y() ) / view()->zoom() );
		else
		{
			// scale operation
			if( m_activeNode == node_lt )
			{
				m_sp = KoPoint( rect.right(), rect.bottom() );
				m_s1 = ( rect.right() - lp.x() ) / double( rect.width() );
				m_s2 = ( rect.bottom() - lp.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_mt )
			{
				m_sp = KoPoint( ( ( rect.right() + rect.left() ) / 2 ), rect.bottom() );
				m_s1 = 1;
				m_s2 = ( rect.bottom() - lp.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_rt )
			{
				m_sp = KoPoint( rect.x(), rect.bottom() );
				m_s1 = ( lp.x() - rect.x() ) / double( rect.width() );
				m_s2 = ( rect.bottom() - lp.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_rm)
			{
				m_sp = KoPoint( rect.x(), ( rect.bottom() + rect.top() )  / 2 );
				m_s1 = ( lp.x() - rect.x() ) / double( rect.width() );
				m_s2 = 1;
			}
			else if( m_activeNode == node_rb )
			{
				m_sp = KoPoint( rect.x(), rect.y() );
				m_s1 = ( lp.x() - rect.x() ) / double( rect.width() );
				m_s2 = ( lp.y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_mb )
			{
				m_sp = KoPoint( ( ( rect.right() + rect.left() ) / 2 ), rect.y() );
				m_s1 = 1;
				m_s2 = ( lp.y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_lb )
			{
				m_sp = KoPoint( rect.right(), rect.y() );
				m_s1 = ( rect.right() - lp.x() ) / double( rect.width() );
				m_s2 = ( lp.y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_lm )
			{
				m_sp = KoPoint( rect.right(), ( rect.bottom() + rect.top() )  / 2 );
				m_s1 = ( rect.right() - lp.x() ) / double( rect.width() );
				m_s2 = 1;
			}
			KoPoint sp = KoPoint( m_sp.x() - view()->canvasWidget()->contentsX(), m_sp.y() - view()->canvasWidget()->contentsY() );
			mat.translate( sp.x(), sp.y() );
			mat.scale( m_s1, m_s2 );
			mat.translate(	- ( sp.x() + view()->canvasWidget()->contentsX() ),
							- ( sp.y() + view()->canvasWidget()->contentsY() ) );
		}

		// TODO :  makes a copy of the selection, do assignment operator instead
		VObjectListIterator itr = view()->part()->document().selection()->objects();
		VObjectList list;
		list.setAutoDelete( true );
		for( ; itr.current() ; ++itr )
		{
			list.append( itr.current()->clone() );
		}
		VObjectListIterator itr2 = list;
		painter->setZoomFactor( view()->zoom() );
		for( ; itr2.current() ; ++itr2 )
		{
			itr2.current()->setState( VObject::edit );
			itr2.current()->transform( mat );

			itr2.current()->draw(
				painter,
				itr2.current()->boundingBox() );
		}
		painter->setZoomFactor( 1.0 );
	}
	else

	view()->painterFactory()->painter()->end();
*/
}

void
VSelectTool::setCursor( const KoPoint& current ) const
{
/*
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
*/
}

void
VSelectTool::mouseButtonPress( const KoPoint& current )
{
	m_current = first( true );
/*
	m_fp.setX( mouse_event->pos().x() );
	m_fp.setY( mouse_event->pos().y() );
	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	QPoint lp = view()->canvasWidget()->viewportToContents( mouse_event->pos() );
	m_activeNode = view()->part()->document().selection()->handleNode( lp );
	view()->part()->document().selection()->setState( VObject::edit );
	view()->part()->repaintAllViews( true );
*/
}

void
VSelectTool::mouseDrag( const KoPoint& current )
{
	draw();

	recalc();

	draw();

/*
	if( m_lock == lockx )
		m_lp.setX( m_fp.x() );
	else
		m_lp.setX( mouse_event->pos().x() );

	m_lp.setY( mouse_event->pos().y() );
*/
}

void
VSelectTool::mouseButtonRelease( const KoPoint& current )
{
	if( m_state == normal )
	{
		KoPoint fp = first();
		KoPoint lp = last();

		if( (fabs( lp.x() - fp.x() ) + fabs( lp.y() - fp.y() ) ) < 3.0 )
		{
			// AK - should take the middle point here
			fp = lp - KoPoint( 8.0, 8.0 );
			lp = lp + KoPoint( 8.0, 8.0 );
		}

		view()->part()->document().selection()->clear();
		view()->part()->document().selection()->append(
			KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize() );

		view()->selectionChanged();
		view()->part()->repaintAllViews( true );
	}
}

void
VSelectTool::mouseDragRelease( const KoPoint& current )
{
	if( m_state == normal )
	{
		// Y mirroring
		KoPoint fp = first();
		fp.setY( -fp.y() + view()->canvasWidget()->viewport()->height() );
		KoPoint lp = last();
		lp.setY( -lp.y() + view()->canvasWidget()->viewport()->height() );
		view()->part()->document().selection()->clear();
		view()->part()->document().selection()->append(
			KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize() );

		view()->selectionChanged();
		view()->part()->repaintAllViews( true );
	}
	else if( m_state == moving )
	{
		m_state = normal;
		view()->part()->addCommand(
			new VTranslateCmd(
				&view()->part()->document(),
				qRound( last().x() - first().x() ),
				qRound( first().y() - last().y() ) ),
			true );
	}

/*
	view()->part()->document().selection()->setState( VObject::selected );

	if( m_state == moving )
	{
		if( m_lock == lockx )
			lp.setX( fp.x() );

//			view()->part()->repaintAllViews();
	}
	else if( m_state == scaling )
	{
		m_state = normal;
		view()->part()->addCommand(
			new VScaleCmd( &view()->part()->document(), m_sp, m_s1, m_s2 ),
			true );
	}
	else
	{
		fp.setX( fp.x() / view()->zoom() );
		fp.setY( fp.y() / view()->zoom() );
		lp.setX( lp.x() / view()->zoom() );
		lp.setY( lp.y() / view()->zoom() );

		if ( (fabs(lp.x()-fp.x()) + fabs(lp.y()-fp.y())) < 3.0 )
		{
			// AK - should take the middle point here
			fp = lp - KoPoint(8.0, 8.0);
			lp = lp + KoPoint(8.0, 8.0);
		}

		// erase old object:
		drawTemporaryObject();

	}
*/
}

void
VSelectTool::mouseDragCtrlPressed( const KoPoint& current )
{
	m_lock = lockx;
}

void
VSelectTool::mouseDragCtrlReleased( const KoPoint& current )
{
	m_lock = none;
}

void
VSelectTool::recalc()
{
	if( m_state == normal )
	{
		m_current = last( true );
	}
	else
	{
		// Build affine matrix:
		QWMatrix mat;
		// Y mirroring
		mat.scale( 1, -1 );
		mat.translate( 0, -view()->canvasWidget()->viewport()->height() );
		mat.translate( last().x() - first().x(), first().y() - last().y() );


		// Copy selected objects and transform:
		m_objects.clear();
		VObject* copy;

		VObjectListIterator itr = view()->part()->document().selection()->objects();
		for ( ; itr.current() ; ++itr )
		{
			copy = itr.current()->clone();
			copy->transform( mat );
			copy->setState( VObject::edit );

			m_objects.append( copy );
		}
	}
}

