/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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
	view()->canvasWidget()->setYMirroring( true );
	painter->setRasterOp( Qt::NotROP );

	//KoPoint current = view()->canvasWidget()->toContents( QPoint( m_current.x(), m_current.y() ) );
	//current.setY( -current.y() + view()->canvasWidget()->contentsHeight() );

	KoRect rect = view()->part()->document().selection()->boundingBox();

	if( m_state != normal || rect.contains( first() ) || m_activeNode != node_none )
	{
		if( m_state == normal )
		{
			m_state = ( m_activeNode == node_none ) ? moving : scaling;
			recalc();
		}

		VObjectListIterator itr = m_objects;
		for( ; itr.current(); ++itr )
		{
			itr.current()->draw( painter, &itr.current()->boundingBox() );
		}

		painter->setZoomFactor( 1.0 );
	}
	else if( m_state == normal )
	{
		painter->setPen( Qt::DotLine );
		painter->newPath();
		painter->moveTo( KoPoint( first().x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), first().y() ) );
		painter->lineTo( KoPoint( m_current.x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), m_current.y() ) );
		painter->lineTo( KoPoint( first().x(), first().y() ) );
		painter->strokePath();

		m_state = normal;
	}
}

void
VSelectTool::setCursor() const
{
	switch( view()->part()->document().selection()->handleNode( last() ) )
	{
		case node_lt:
		case node_rb:
			view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeFDiagCursor ) );
			break;
		case node_rt:
		case node_lb:
			view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeBDiagCursor ) );
			break;
		case node_lm:
		case node_rm:
			view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeHorCursor ) );
			break;
		case node_mt:
		case node_mb:
			view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::SizeVerCursor ) );
			break;
		default:
			view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
	}
}

void
VSelectTool::mouseButtonPress()
{
	m_current = first();

	m_activeNode = view()->part()->document().selection()->handleNode( first() );

	recalc();

	//view()->part()->document().selection()->setState( VObject::edit );
	//view()->painterFactory()->painter()->end();

	draw();

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
VSelectTool::mouseDrag()
{
	draw();

	recalc();

	draw();
}

void
VSelectTool::mouseButtonRelease()
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
VSelectTool::mouseDragRelease()
{
	//draw();

	if( m_state == normal )
	{
		// Y mirroring
		KoPoint fp = first();
		//fp.setY( -fp.y() + view()->canvasWidget()->contentsHeight() );
		KoPoint lp = last();
		//lp.setY( -lp.y() + view()->canvasWidget()->contentsHeight() );
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
				qRound( last().y() - first().y() ) ),
			true );
	}
	else if( m_state == scaling )
	{
		m_state = normal;
		view()->part()->addCommand(
			new VScaleCmd( &view()->part()->document(), m_sp, m_s1, m_s2 ),
			true );
		m_s1 = m_s2 = 1;
	}
}

void
VSelectTool::mouseDragCtrlPressed()
{
	m_lock = lockx;
}

void
VSelectTool::mouseDragCtrlReleased()
{
	m_lock = none;
}

void
VSelectTool::recalc()
{
	if( m_state == normal )
	{
		m_current = last();
	}
	else 
	{
		// Build affine matrix:
		QWMatrix mat;
		if( m_state == moving )
			mat.translate( last().x() - first().x(), last().y() - first().y() );
		else
		{
			KoRect rect = view()->part()->document().selection()->boundingBox();

			if( m_activeNode == node_lb )
			{
				m_sp = KoPoint( rect.right(), rect.bottom() );
				m_s1 = ( rect.right() - last().x() ) / double( rect.width() );
				m_s2 = ( rect.bottom() - last().y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_mb )
			{
				m_sp = KoPoint( ( ( rect.right() + rect.left() ) / 2 ), rect.bottom() );
				m_s1 = 1;
				m_s2 = ( rect.bottom() - last().y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_rb )
			{
				m_sp = KoPoint( rect.x(), rect.bottom() );
				m_s1 = ( last().x() - rect.x() ) / double( rect.width() );
				m_s2 = ( rect.bottom() - last().y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_rm)
			{
				m_sp = KoPoint( rect.x(), ( rect.bottom() + rect.top() )  / 2 );
				m_s1 = ( last().x() - rect.x() ) / double( rect.width() );
				m_s2 = 1;
			}
			else if( m_activeNode == node_rt )
			{
				m_sp = KoPoint( rect.x(), rect.y() );
				m_s1 = ( last().x() - rect.x() ) / double( rect.width() );
				m_s2 = ( last().y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_mt )
			{
				m_sp = KoPoint( ( ( rect.right() + rect.left() ) / 2 ), rect.y() );
				m_s1 = 1;
				m_s2 = ( last().y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_lt )
			{
				m_sp = KoPoint( rect.right(), rect.y() );
				m_s1 = ( rect.right() - last().x() ) / double( rect.width() );
				m_s2 = ( last().y() - rect.y() ) / double( rect.height() );
			}
			else if( m_activeNode == node_lm )
			{
				m_sp = KoPoint( rect.right(), ( rect.bottom() + rect.top() )  / 2 );
				m_s1 = ( rect.right() - last().x() ) / double( rect.width() );
				m_s2 = 1;
			}
			KoPoint sp = KoPoint( m_sp.x(), m_sp.y() );
			mat.translate( sp.x(), sp.y() );
			mat.scale( m_s1, m_s2 );
			mat.translate(-sp.x(), -sp.y() );
		}

		// Copy selected objects and transform:
		m_objects.clear();
		VObject* copy;

		VObjectListIterator itr = view()->part()->document().selection()->objects();
		for ( ; itr.current() ; ++itr )
		{
			if( itr.current()->state() != VObject::deleted )
			{
				copy = itr.current()->clone();
				copy->transform( mat );
				copy->setState( VObject::edit );

				m_objects.append( copy );
			}
		}
	}
}

