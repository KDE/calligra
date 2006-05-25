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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "vdocument.h"
#include "vdrawselection.h"
#include "vpainter.h"
#include "vselection.h"
#include "vselectnodes.h"
#include "vselectobjects.h"
#include "vvisitor.h"
#include "vcolor.h"
#include "vfill.h"
#include "vstroke.h"
//Added by qt3to4:
#include <Q3PtrList>

uint VSelection::m_handleNodeSize = 3;

VSelection::VSelection( VObject* parent )
	: VObject( parent ), m_showhandle( true )
{
	m_handleRect = new KoRect[ 10 ];
	setStroke( VStroke( VColor( Qt::black ) ) );
	setFill( VFill() );

	m_selectObjects = true;
}

VSelection::VSelection( const VSelection& selection )
	: VObject( selection ), VVisitor()
{
	m_handleRect = new KoRect[ 10 ];

	VObjectListIterator itr = selection.m_objects;
	for ( ; itr.current() ; ++itr )
		append( itr.current() );	// Don't clone objects here.

	m_showhandle = true;
	m_selectObjects = selection.m_selectObjects;
}

VSelection::~VSelection()
{
	//clear();
	delete[]( m_handleRect );
}

VSelection*
VSelection::clone() const
{
	return new VSelection( *this );
}

void
VSelection::accept( VVisitor& visitor )
{
	visitor.visitVSelection( *this );
}

void
VSelection::take( VObject& object )
{
	m_objects.removeRef( &object );
	if( object.state() >= selected )
		object.setState( normal );
	invalidateBoundingBox();
}

bool
VSelection::take( const KoRect& rect, bool selectObjects, bool exclusive )
{
	bool success = false;

	if( selectObjects )
	{
		VSelectObjects op( m_objects, rect, false );
		if( op.visit( *static_cast<VDocument*>( parent() ) ) )
		{
			selectNodes();
			success = true;
		}
	}
	else
	{
		VObjectListIterator itr( m_objects );

		// Try to deselect all that have at least one node contained in the rect
		for ( ; itr.current(); ++itr )
		{
			VSelectNodes op( rect, false, exclusive );

			if( op.visit( *itr.current() ) )
			{
				success = true;
			}
		}
	}

	invalidateBoundingBox();

	return success;
}

void
VSelection::append()
{
	clear();

	VSelectObjects op( m_objects );
	op.visit( *static_cast<VDocument*>( parent() ) );
	selectNodes();

	invalidateBoundingBox();
}

void
VSelection::append( VObject* object )
{
	// only append if item is not deleted or not already in list
	if( object->state() != deleted )
	{
		if( ! m_objects.containsRef( object ) )
			m_objects.append( object );
		object->setState( selected );
		invalidateBoundingBox();
	}
}

void
VSelection::append( const VObjectList &objects )
{
	VObjectListIterator itr = objects;
	for( ; itr.current(); ++itr )
		append( itr.current() );
}

bool
VSelection::append( const KoRect& rect, bool selectObjects, bool exclusive )
{
	bool success = false;

	if( selectObjects )
	{
		//m_objects.clear();
		VSelectObjects op( m_objects, rect );
		if( op.visit( *static_cast<VDocument*>( parent() ) ) )
		{
			selectNodes();
			success = true;
		}
	}
	else
	{
		VObjectListIterator itr( m_objects );
		VObjectList notSelected;

		// Try to select all that have at least one node contained in the rect
		for ( ; itr.current(); ++itr )
		{
			VSelectNodes op( rect, true, exclusive );

			if( op.visit( *itr.current() ) )
				success = true;
			else
				notSelected.append( itr.current());
		}
		// Remove all that were not selected from this selection
		VObjectListIterator jtr( notSelected );
		for ( ; jtr.current(); ++jtr )
			take( *( jtr.current() ) );
	}

	invalidateBoundingBox();

	return success;
}

void
VSelection::clear()
{
	VSelectNodes op( true );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		op.visit( *itr.current() );

		//if( itr.current()->state() != deleted )
		//	itr.current()->setState( normal );
	}

	m_objects.clear();
	invalidateBoundingBox();
}

void
VSelection::draw( VPainter* painter, double zoomFactor ) const
{
	if( objects().count() == 0 || state() == VObject::edit )
		return;

	VDrawSelection op( m_objects, painter, !m_selectObjects, m_handleNodeSize );
	op.visitVSelection( (VSelection &)*this );

	// get bounding box:
	const KoRect& rect = boundingBox();

	// calculate displaycoords of big handle rect:
	m_handleRect[ 0 ].setCoords(	qRound( rect.left() ), qRound( rect.top() ),
									qRound( rect.right() ), qRound( rect.bottom() ) );

	KoPoint center = m_handleRect[ 0 ].center();

	double handleNodeSize = m_handleNodeSize / zoomFactor;

	// calculate displaycoords of nodes:
	m_handleRect[ node_lb ].setRect( m_handleRect[0].left() - handleNodeSize, m_handleRect[0].top() - handleNodeSize, 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_mb ].setRect( center.x() - handleNodeSize, m_handleRect[0].top() - handleNodeSize, 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_rb ].setRect( m_handleRect[0].right() - handleNodeSize - (1 / zoomFactor), m_handleRect[0].top() - handleNodeSize, 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_rm ].setRect( m_handleRect[0].right() - handleNodeSize - (1 / zoomFactor), center.y() - handleNodeSize, 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_rt ].setRect( m_handleRect[0].right() - handleNodeSize - (1 / zoomFactor) , m_handleRect[0].bottom() - handleNodeSize - (1 / zoomFactor), 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_mt ].setRect( center.x() - handleNodeSize, m_handleRect[0].bottom() - handleNodeSize - (1 / zoomFactor), 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_lt ].setRect( m_handleRect[0].left() - handleNodeSize, m_handleRect[0].bottom() - handleNodeSize - (1 / zoomFactor), 2 * handleNodeSize, 2 * handleNodeSize );
	m_handleRect[ node_lm ].setRect( m_handleRect[0].left() - handleNodeSize, center.y() - handleNodeSize, 2 * handleNodeSize, 2 * handleNodeSize );

	if( !m_showhandle ) return;

	// draw handle rect:
	painter->setPen( Qt::blue.light() );
	painter->setBrush( Qt::NoBrush );

	painter->drawRect( KoRect( m_handleRect[ 0 ].x() * zoomFactor, m_handleRect[ 0 ].y() * zoomFactor,
							  m_handleRect[ 0 ].width() * zoomFactor, m_handleRect[ 0 ].height() * zoomFactor ) );
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
				temp.setRect(	zoomFactor * m_handleRect[ i ].left(),
								zoomFactor * m_handleRect[ i ].top(),
								2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
				painter->drawRect( temp );
			}
		}
	}
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
			m_boundingBox |= itr.current()->boundingBox();

//		m_boundingBoxIsInvalid = false;
//	}

	return m_boundingBox;
}


VHandleNode
VSelection::handleNode( const KoPoint &point ) const
{
	for( uint i = node_lt; i <= node_rb; ++i )
	{
		if( m_handleRect[i].contains( point ) )
			return static_cast<VHandleNode>( i );
	}

	return node_none;
}

Q3PtrList<VSegment>
VSelection::getSegments( const KoRect& rect )
{
	VTestNodes op( rect );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
		op.visit( *itr.current() );

	return op.result();
}

void
VSelection::selectNodes( bool select )
{
	VSelectNodes op( select );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		op.visit( *itr.current() );
	}
}

