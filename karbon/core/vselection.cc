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

#include <qrect.h>

#include "vdocument.h"
#include "vpainter.h"
#include "vselection.h"
#include "vselectnodes.h"
#include "vselectobjects.h"
#include "vdrawselection.h"


VSelection::VSelection( VObject* parent )
	: VObject( parent )
{
	m_qrect = new QRect[10];
	setStroke( VStroke( VColor( Qt::black ) ) );
	setFill( VFill() );
}

VSelection::VSelection( const VSelection& selection )
	: VObject( selection ), VVisitor()
{
	m_qrect = new QRect[10];

	VObjectListIterator itr = selection.m_objects;
	for ( ; itr.current() ; ++itr )
		append( itr.current() );	// Don't clone objects here.
}

VSelection::~VSelection()
{
	clear();
	delete[]( m_qrect );
}

VSelection*
VSelection::clone() const
{
	return new VSelection( *this );
}

void
VSelection::take( VObject& object )
{
	m_objects.removeRef( &object );
	object.setState( normal );
	invalidateBoundingBox();
}

void
VSelection::append()
{
	clear();

	VSelectObjects op( m_objects );
	op.visit( *static_cast<VDocument*>( parent() ) );

	invalidateBoundingBox();
}

void
VSelection::append( VObject* object )
{
	if( object->state() != deleted )
	{
		m_objects.append( object );
		object->setState( selected );
		invalidateBoundingBox();
	}
}

bool
VSelection::append( const KoRect& rect, bool selectObjects )
{
	bool success = false;

	if( selectObjects )
	{
		VSelectObjects op( m_objects, rect );

		if( op.visit( *static_cast<VDocument*>( parent() ) ) )
			success = true;
	}
	else
	{
		VObjectListIterator itr( m_objects );

		for ( ; itr.current(); ++itr )
		{
			VSelectNodes op( rect );

			if( op.visit( *itr.current() ) )
				success = true;
		}
	}

	invalidateBoundingBox();

	return success;
}

void
VSelection::clear()
{
	VSelectNodes op( false );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		op.visit( *itr.current() );

		if( itr.current()->state() != deleted )
			itr.current()->setState( normal );
	}

	m_objects.clear();
	invalidateBoundingBox();
}

void
VSelection::draw( VPainter* painter, double zoomFactor ) const
{
	if( objects().count() == 0 || state() == VObject::edit )
		return;

	VDrawSelection op( m_objects, painter );
	op.visit( *static_cast<VDocument*>( parent() ) );

	// get bounding box:
	const KoRect& rect = boundingBox();

	// calculate displaycoords of big handle rect:
	m_qrect[0].setCoords(	qRound( rect.left() ), qRound( rect.top() ),
							qRound( rect.right() ), qRound( rect.bottom() ) );

	QPoint center = m_qrect[ 0 ].center();

	// calculate displaycoords of nodes:
	m_qrect[node_lb].setRect( m_qrect[0].left(), m_qrect[0].top(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_mb].setRect( center.x(), m_qrect[0].top(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_rb].setRect( m_qrect[0].right(), m_qrect[0].top(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_rm].setRect( m_qrect[0].right(), center.y(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_rt].setRect( m_qrect[0].right(), m_qrect[0].bottom(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_mt].setRect( center.x(), m_qrect[0].bottom(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_lt].setRect( m_qrect[0].left(), m_qrect[0].bottom(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_lm].setRect( m_qrect[0].left(), center.y(), 2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );

	// draw handle rect:
	painter->setPen( Qt::blue.light() );
	painter->setBrush( Qt::NoBrush );

	painter->drawRect( KoRect( m_qrect[ 0 ].x() * zoomFactor, m_qrect[ 0 ].y() * zoomFactor,
							  m_qrect[ 0 ].width() * zoomFactor, m_qrect[ 0 ].height() * zoomFactor ) );
	painter->setPen( Qt::blue.light() );

	// draw nodes:
	if( state() == VObject::selected )
	{
		painter->setPen( Qt::blue.light() );
		painter->setBrush( Qt::white );

		KoRect temp;
		for( uint i = node_lt; i <= node_rb; ++i )
		{
			if( i != node_mm )
			{
				temp.setRect(	zoomFactor * m_qrect[ i ].left() - m_handleNodeSize,
								zoomFactor * m_qrect[ i ].top()  - m_handleNodeSize,
								2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
				painter->drawRect( temp );
			}
		}
	}

	// Draw a center node:
	//m_painter->setRasterOp( Qt::NotROP );
	painter->setPen( Qt::NoPen );
	painter->setBrush( Qt::blue.light() );
	painter->drawNode( boundingBox().center(), 3 );
}

const KoRect&
VSelection::boundingBox() const
{
// disable bbox caching for selection since there is no reliable
// way to get notified of a bbox change:
//	if( m_boundingBoxIsInvalid )
//	{
		// clear:
		m_boundingBox = KoRect();

		VObjectListIterator itr = m_objects;
		for( ; itr.current(); ++itr )
		{
			m_boundingBox |= itr.current()->boundingBox();
		}

//		m_boundingBoxIsInvalid = false;
//	}

	return m_boundingBox;
}


VHandleNode
VSelection::handleNode( const KoPoint &point ) const
{
	QPoint qpoint( point.x(), point.y() );
	for( uint i = node_lt; i <= node_rb; ++i )
	{
		if( m_qrect[i].contains( qpoint ) )
			return static_cast<VHandleNode>( i );
	}

	return node_none;
}

bool
VSelection::pathNode( const KoRect& rect )
{
	VSelectNodes op( rect, true );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		if( op.visit( *itr.current() ) )
			return true;
	}

	return false;
}

void
VSelection::clearNodes()
{
	VSelectNodes op( false );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		op.visit( *itr.current() );
	}
}

