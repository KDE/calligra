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
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vsheartool.h"
#include "vtransformcmd.h"


VShearTool::VShearTool( KarbonView* view ) : VTool( view )
{
	m_objects.setAutoDelete( true );
}

VShearTool::~VShearTool()
{
}

void
VShearTool::activate()
{
	view()->statusMessage()->setText( i18n( "Shear" ) );
	view()->canvasWidget()->viewport()->setCursor( QCursor( Qt::arrowCursor ) );
}

void
VShearTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();
	view()->canvasWidget()->setYMirroring( true );
	painter->setRasterOp( Qt::NotROP );

	VObjectListIterator itr = m_objects;
	painter->setZoomFactor( view()->zoom() );
	for( ; itr.current(); ++itr )
		itr.current()->draw( painter, &itr.current()->boundingBox() );

	painter->setZoomFactor( 1.0 );
}

void
VShearTool::setCursor() const
{
	switch( view()->part()->document().selection()->handleNode( last() ) )
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
VShearTool::mouseButtonPress()
{
	view()->painterFactory()->painter()->end();
	m_activeNode = view()->part()->document().selection()->handleNode( first() );
	recalc();

	// Draw new object:
	draw();
}

void
VShearTool::mouseDrag( )
{
	// Erase old object:
	draw();

	recalc();

	// Draw new object:
	draw();
}


void
VShearTool::mouseDragRelease()
{
	view()->part()->addCommand(
		new VShearCmd( &view()->part()->document(), last(), m_s1, m_s2 ),
		true );
}

void
VShearTool::recalc()
{
	KoRect rect = view()->part()->document().selection()->boundingBox();

	if( m_activeNode == node_lt )
	{
	}
	else if( m_activeNode == node_mt )
	{
		m_s1 = 0;
		m_s2 = ( last().y() - first().y() ) / double( ( rect.height() / 2 ) * view()->zoom() );
	}
	else if( m_activeNode == node_rt )
	{
	}
	else if( m_activeNode == node_rm)
	{
		m_s1 = ( last().x() - first().x() ) / double( ( rect.width() / 2 ) * view()->zoom() );
		m_s2 = 0;
	}
	else if( m_activeNode == node_rb )
	{
	}
	else if( m_activeNode == node_mb )
	{
		m_s1 = 0;
		m_s2 = ( last().y() - first().y() ) / double( ( rect.height() / 2 ) * view()->zoom() );
	}
	else if( m_activeNode == node_lb )
	{
	}
	else if( m_activeNode == node_lm )
	{
		m_s1 = ( last().x() - first().x() ) / double( ( rect.width() / 2 ) * view()->zoom() );
		m_s2 = 0;
	}

	// Get center:
	KoPoint m_center = view()->part()->document().selection()->boundingBox().center();

	// Build affine matrix:
	QWMatrix mat;
	mat.translate( m_center.x(), m_center.y() );
	mat.shear( m_s1, m_s2 );
	mat.translate( -m_center.x(), -m_center.y() );

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
