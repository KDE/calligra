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
#include "veditnodetool.h"
#include "vtransformcmd.h"

#include <kdebug.h>


VEditNodeTool::VEditNodeTool( KarbonView* view )
	: VTool( view ), m_state( normal ), m_isDragging( false )
{
}

VEditNodeTool::~VEditNodeTool()
{
}

void
VEditNodeTool::activate()
{
	//if( m_state == normal )
		view()->statusMessage()->setText( i18n( "EditNode" ) );
	//else
//		view()->statusMessage()->setText( i18n( "Scale" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	view()->part()->document().selection()->setState( VObject::edit );
}

void
VEditNodeTool::setCursor( const QPoint &p ) const
{
	if( view()->part()->document().selection()->selectNode( KoPoint( p ) ) )
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::CrossCursor ) );
	else
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VEditNodeTool::drawTemporaryObject()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
	KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x() / view()->zoom(), m_lp.y() / view()->zoom() ) );

	KoRect rect = view()->part()->document().selection()->boundingBox();

	kdDebug() << " x: " << rect.x() << " y: " << rect.y() << " rect.width: " << rect.width() << " rect.height: " << rect.height() << endl;
	if( view()->part()->document().selection()->objects().count() > 0 &&
		( m_state != normal || rect.contains( fp * ( 1.0 /  view()->zoom() ) ) ) )
	{
		if( m_state == normal )
			m_state = moving;

		// move operation
		QWMatrix mat;
		mat.translate(	( m_lp.x() - fp.x() ) / view()->zoom(),
						( m_lp.y() - fp.y() ) / view()->zoom() );

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
VEditNodeTool::eventFilter( QEvent* event )
{
	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
	QPoint lp = view()->canvasWidget()->viewportToContents( mouse_event->pos() );
	if( !m_isDragging )
		setCursor( lp );

	if ( event->type() == QEvent::MouseMove && m_isDragging )
	{
		// erase old object:
		drawTemporaryObject();

		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// paint new object:
		drawTemporaryObject();

		return true;
	}

	if ( event->type() == QEvent::MouseButtonRelease && m_isDragging )
	{
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		// adjust to real viewport contents instead of raw mouse coords:
		KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
		KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x(), m_lp.y() ) );

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
			m_isDragging = false;

			// erase old object:
			drawTemporaryObject();

			return true;
		}
	}

	// the whole story starts with this event:
	if ( event->type() == QEvent::MouseButtonPress )
	{
		view()->painterFactory()->painter()->end();

		m_fp.setX( mouse_event->pos().x() );
		m_fp.setY( mouse_event->pos().y() );
		m_lp.setX( mouse_event->pos().x() );
		m_lp.setY( mouse_event->pos().y() );

		//m_activeNode = view()->part()->document().selection()->node( lp );

		// draw initial object:
		drawTemporaryObject();
		m_isDragging = true;

		return true;
	}

	return false;
}

