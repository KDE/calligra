/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <math.h>

#include <qcursor.h>
#include <QLabel>

#include <klocale.h>
#include <KoRect.h>

#include <karbon_part.h>
#include <karbon_view.h>
#include <render/vpainter.h>
#include <render/vpainterfactory.h>
#include <vselection.h>
#include "vsheartool.h"
#include <commands/vtransformcmd.h>

#include <kactioncollection.h>

VShearTool::VShearTool( KarbonView *view ) : VTool( view, "sheartool" )
{
	setName( "tool_shear" );
	m_objects.setAutoDelete( true );
	registerTool( this );
}

VShearTool::~VShearTool()
{
}

void
VShearTool::activate()
{
	view()->setCursor( QCursor( Qt::ArrowCursor ) );
	view()->part()->document().selection()->showHandle( true );
	view()->part()->document().selection()->setState( VObject::selected );
	VTool::activate();
}

QString
VShearTool::statusText()
{
	return i18n( "Shear" );
}

void
VShearTool::draw()
{
	VPainter* painter = view()->painterFactory()->editpainter();

	// TODO: rasterops need porting to Qt4
	// painter->setRasterOp( Qt::NotROP );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
		itr.current()->draw( painter, &itr.current()->boundingBox() );
}

void
VShearTool::setCursor() const
{
	if( isDragging() ) return;
	switch( view()->part()->document().selection()->handleNode( last() ) )
	{
		case node_lt:
		case node_rb:
			view()->setCursor( QCursor( Qt::SizeFDiagCursor ) );
			break;
		case node_rt:
		case node_lb:
			view()->setCursor( QCursor( Qt::SizeBDiagCursor ) );
			break;
		case node_lm:
		case node_rm:
			view()->setCursor( QCursor( Qt::SizeHorCursor ) );
			break;
		case node_mt:
		case node_mb:
			view()->setCursor( QCursor( Qt::SizeVerCursor ) );
			break;
		default:
			view()->setCursor( QCursor( Qt::ArrowCursor ) );
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
		new VShearCmd( &view()->part()->document(), m_center, m_s1, m_s2, altPressed() ),
		true );
}

void
VShearTool::cancel()
{
	// Erase old object:
	if ( isDragging() )
	{
		draw();
		view()->repaintAll( view()->part()->document().selection()->boundingBox() );
	}
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
		m_s2 = ( last().y() - first().y() ) / double( ( rect.height() / 2 ) );
	}
	else if( m_activeNode == node_rt )
	{
	}
	else if( m_activeNode == node_rm)
	{
		m_s1 = ( last().x() - first().x() ) / double( ( rect.width() / 2 ) );
		m_s2 = 0;
	}
	else if( m_activeNode == node_rb )
	{
	}
	else if( m_activeNode == node_mb )
	{
		m_s1 = 0;
		m_s2 = ( last().y() - first().y() ) / double( ( rect.height() / 2 ) );
	}
	else if( m_activeNode == node_lb )
	{
	}
	else if( m_activeNode == node_lm )
	{
		m_s1 = ( last().x() - first().x() ) / double( ( rect.width() / 2 ) );
		m_s2 = 0;
	}

	// Get center:
	m_center = view()->part()->document().selection()->boundingBox().center();

	VShearCmd cmd( 0L, m_center, m_s1, m_s2 );

	// Copy selected objects and transform:
	m_objects.clear();
	VObject* copy;

	VObjectListIterator itr = view()->part()->document().selection()->objects();
	for ( ; itr.current() ; ++itr )
	{
		if( itr.current()->state() != VObject::deleted )
		{
			copy = itr.current()->clone();

			cmd.visit( *copy );

			copy->setState( VObject::edit );

			m_objects.append( copy );
		}
	}
}

void
VShearTool::setup( KActionCollection *collection )
{
	m_action = static_cast<KAction *>(collection -> action( name() ) );

	if( m_action == 0 )
	{
		m_action = new KAction( i18n( "Shear Tool" ), "14_shear", Qt::SHIFT+Qt::Key_H, this, SLOT( activate() ), collection, name() );
		m_action->setToolTip( i18n( "Shear" ) );
		// TODO porting needed: m_action->setExclusiveGroup( "manipulation" );
		//m_ownAction = true;
	}
}

