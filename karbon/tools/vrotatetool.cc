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
	m_objects.setAutoDelete( true );
}

VRotateTool::~VRotateTool()
{
	m_objects.clear();
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
	VPainter* painter = view()->painterFactory()->editpainter();
	painter->setRasterOp( Qt::NotROP );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		itr.current()->draw( painter, itr.current()->boundingBox() );
	}

	painter->setZoomFactor( 1.0 );




/*
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
VRotateTool::setCursor() const
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
VRotateTool::mouseButtonPress()
{
	view()->painterFactory()->painter()->end();
//	m_activeNode = view()->part()->document().selection()->handleNode( current ) );
	recalc();

	// Draw new object:
	draw();
}

void
VRotateTool::mouseDrag()
{
	// Erase old object:
	draw();

	recalc();

	// Draw new object:
	draw();
}

void
VRotateTool::mouseDragRelease()
{
	view()->part()->addCommand(
		new VRotateCmd(
			&view()->part()->document(),
			m_center,
			m_angle ),
		true );

	view()->selectionChanged();
}

void
VRotateTool::recalc()
{
	// Get center:
	m_center = view()->part()->document().selection()->boundingBox().center();

	// Calculate angle between vector (last - center) and (first - center):
	m_angle = VGlobal::one_pi_180 * (
			atan2(
				last().y() - m_center.y(),
				last().x() - m_center.x() )
		-
			atan2(
				first().y() - m_center.y(),
				first().x() - m_center.x() ) );

	// Build affine matrix:
	QWMatrix mat;
	mat.translate( m_center.x(), m_center.y() );
	mat.rotate( m_angle );
	mat.translate( -m_center.x(), -m_center.y() );


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

