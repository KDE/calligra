/* This file is part of the KDE project
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
#include "vselectnodestool.h"
#include "vtransformcmd.h"
#include "vtransformnodes.h"

#include <kdebug.h>


VSelectNodesTool::VSelectNodesTool( KarbonView* view )
	: VTool( view ), m_state( normal ), m_isDragging( false )
{
}

VSelectNodesTool::~VSelectNodesTool()
{
}

void
VSelectNodesTool::activate()
{
	//if( m_state == normal )
		view()->statusMessage()->setText( i18n( "EditNode" ) );
	//else
//		view()->statusMessage()->setText( i18n( "Scale" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	view()->part()->document().selection()->setState( VObject::edit );
}

void
VSelectNodesTool::setCursor( const KoPoint &p ) const
{
	double tolerance = 1.0 / view()->zoom();

	if( view()->part()->document().selection()->pathNode(
		KoRect(
			p.x() - tolerance,
			p.y() - tolerance,
			2 * tolerance + 1,
			2 * tolerance * 1 ) ) )
	{
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::CrossCursor ) );
	}
	else
		view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VSelectNodesTool::drawTemporaryObject()
{
	VPainter *painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	KoPoint fp = view()->canvasWidget()->viewportToContents( QPoint( m_fp.x(), m_fp.y() ) );
	KoPoint lp = view()->canvasWidget()->viewportToContents( QPoint( m_lp.x() / view()->zoom(), m_lp.y() / view()->zoom() ) );

	double tolerance = 1.0 / view()->zoom();

	if( view()->part()->document().selection()->objects().count() > 0 &&
		m_state != dragging &&
		( m_state == moving || view()->part()->document().selection()->pathNode(
			KoRect(
				lp.x() - tolerance,
				lp.y() - tolerance,
				2 * tolerance + 1.0,
				2 * tolerance + 1.0 ) ) ) )
	{
		if( m_state == normal )
		{
			double tolerance = 1.0 / view()->zoom();
			view()->part()->document().selection()->append(
				KoRect(
					fp.x() - tolerance,
					fp.y() - tolerance,
					2 * tolerance + 1.0,
					2 * tolerance + 1.0 ).normalize(),
				false );
			m_state = moving;
		}

		// move operation
		QWMatrix mat;
		mat.translate(	( m_lp.x() - fp.x() ) / view()->zoom(),
						( m_lp.y() - fp.y() ) / view()->zoom() );

		VTransformNodes op( mat );

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
			op.visit( *itr2.current() );

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

		m_state = dragging;
	}
}


bool
VSelectNodesTool::eventFilter( QEvent* event )
{
	QMouseEvent* mouse_event = static_cast<QMouseEvent*> ( event );
	QPoint lp = view()->canvasWidget()->viewportToContents( mouse_event->pos() );
	if( !m_isDragging )
	{
		KoPoint lpp = KoPoint( lp.x() / view()->zoom(), lp.y() / view()->zoom() );
		setCursor( lpp );
	}

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

			VTranslateCmd *cmd = new VTranslateCmd(
					&view()->part()->document(),
					qRound( ( lp.x() - fp.x() ) * ( 1.0 / view()->zoom() ) ),
					qRound( ( lp.y() - fp.y() ) * ( 1.0 / view()->zoom() ) ) );
			view()->part()->addCommand( cmd, true );

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
				KoRect( fp.x(), fp.y(), lp.x() - fp.x(), lp.y() - fp.y() ).normalize(),
				false );

			view()->selectionChanged();
			view()->part()->repaintAllViews();
			m_state = normal;
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
		//view()->part()->document().selection()->clearNodes();

		// draw initial object:
		drawTemporaryObject();
		m_isDragging = true;

		return true;
	}

	return false;
}

