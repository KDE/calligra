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
}

VSelectTool::~VSelectTool()
{
}

void
VSelectTool::mousePressed( QMouseEvent *mouse_event )
{
	view()->painterFactory()->painter()->end();

	m_fp.setX( mouse_event->pos().x() );
	m_fp.setY( mouse_event->pos().y() );
	m_lp.setX( mouse_event->pos().x() );
	m_lp.setY( mouse_event->pos().y() );

	QPoint lp = view()->canvasWidget()->viewportToContents( mouse_event->pos() );
	m_activeNode = view()->part()->document().selection()->handleNode( lp );
	view()->part()->document().selection()->setState( VObject::edit );
	view()->part()->repaintAllViews();

	// draw initial object:
	drawTemporaryObject();
	m_isDragging = true;
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
VSelectTool::setCursor( const QPoint &p ) const
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
VSelectTool::drawTemporaryObject()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
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
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( m_fp.x(), m_fp.y() ) );
		painter->lineTo( KoPoint( m_lp.x(), m_fp.y() ) );
		painter->lineTo( KoPoint( m_lp.x(), m_lp.y() ) );
		painter->lineTo( KoPoint( m_fp.x(), m_lp.y() ) );
		painter->lineTo( KoPoint( m_fp.x(), m_fp.y() ) );
		painter->strokePath();

		m_state = normal;
	}
}


bool
VSelectTool::eventFilter( QEvent* event )
{
	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
	QPoint lp = view()->canvasWidget()->viewportToContents( mouse_event->pos() );
	setCursor( lp );

	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		mouseMoved( static_cast<QMouseEvent *>( event ) );
		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// adjust to real viewport contents instead of raw mouse coords:
		KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
		KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );

		view()->part()->document().selection()->setState( VObject::selected );

		if( m_state == moving )
		{
			m_state = normal;
			view()->part()->addCommand(
				new VTranslateCmd(
					&view()->part()->document(),
					qRound( ( lp.x() - fp.x() ) * ( 1.0 / view()->zoom() ) ),
					qRound( ( lp.y() - fp.y() ) * ( 1.0 / view()->zoom() ) ) ),
				true );

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

			view()->part()->document().selection()->clear();
			view()->part()->document().selection()->append(
				KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize() );

			view()->selectionChanged();
			view()->part()->repaintAllViews();
		}

		m_isDragging = false;

		return true;
	}

	// handle pressing of keys:
	if ( event->type() == QEvent::KeyPress )
	{
		QKeyEvent* key_event = static_cast<QKeyEvent*>( event );

		// cancel dragging with ESC-key:
		if ( key_event->key() == Qt::Key_Escape && m_isDragging )
		{
			cancel();
			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		mousePressed( static_cast<QMouseEvent*>( event ) );
		return true;
	}

	return false;
}

