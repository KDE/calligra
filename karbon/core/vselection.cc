/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <qrect.h>

#include "vdocument.h"
#include "vselection.h"
#include "vselectnodes.h"

VSelection::VSelection( VObject* parent )
	: VObject( parent )
{
	m_qrect = new QRect[10];
}

VSelection::VSelection( const VSelection& selection )
	: VObject( selection )
{
	m_qrect = new QRect[10];

	VObjectListIterator itr = selection.m_objects;
	for ( ; itr.current() ; ++itr )
		append( itr.current() );
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

	VObjectList objects;
	VLayerListIterator itr(
		static_cast<VDocument*>( parent() )->layers() );

	for ( ; itr.current(); ++itr )
	{
		objects = itr.current()->objects();

		VObjectListIterator itr2( objects );

		for ( ; itr2.current(); ++itr2 )
		{
			if( static_cast<VObject *>( itr2.current() )->state() != deleted )
			{
				append( itr2.current() );
			}
		}
	}

	invalidateBoundingBox();
}

void
VSelection::append( VObject* object )
{
	m_objects.append( object );
	object->setState( selected );
	invalidateBoundingBox();
}

void
VSelection::append( const KoRect& rect )
{
	VObjectList objects;
	VLayerListIterator itr(
		static_cast<VDocument*>( parent() )->layers() );

	for ( ; itr.current(); ++itr )
	{
		VObjectListIterator itr2( itr.current()->objects() );

		for ( ; itr2.current(); ++itr2 )
		{
			if(
				itr2.current()->state() == normal &&
// TODO: use a zoom dependant vflatten visitor to achieve finer resolution:
				itr2.current()->boundingBox().intersects( rect ) )
			{
				append( itr2.current() );
				VSelectNodes op( rect, true, 2 );
				op.visit( *itr2.current() );
			}
		}
	}

	invalidateBoundingBox();
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
VSelection::draw( QPainter* painter, double zoomFactor ) const
{
	if( objects().count() == 0 || state() == VObject::edit )
		return;


	// get bounding box:
	const KoRect& rect = boundingBox();

	// calculate displaycoords of big handle rect:
	m_qrect[0].setCoords(
		qRound( rect.left() * zoomFactor ),
		qRound( rect.top() * zoomFactor ),
		qRound( rect.right() * zoomFactor ),
		qRound( rect.bottom() * zoomFactor ) );

	QPoint center = m_qrect[0].center();

	// calculate displaycoords of nodes:
	m_qrect[node_lt].setRect(
		m_qrect[0].left() - m_handleNodeSize,
		m_qrect[0].top() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_mt].setRect(
		center.x() - m_handleNodeSize,
		m_qrect[0].top() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_rt].setRect(
		m_qrect[0].right() - m_handleNodeSize,
		m_qrect[0].top() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_rm].setRect(
		m_qrect[0].right() - m_handleNodeSize,
		center.y() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_rb].setRect(
		m_qrect[0].right() - m_handleNodeSize,
		m_qrect[0].bottom() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_mb].setRect(
		center.x() - m_handleNodeSize,
		m_qrect[0].bottom() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_lb].setRect(
		m_qrect[0].left() - m_handleNodeSize,
		m_qrect[0].bottom() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );
	m_qrect[node_lm].setRect(
		m_qrect[0].left() - m_handleNodeSize,
		center.y() - m_handleNodeSize,
		2 * m_handleNodeSize + 1, 2 * m_handleNodeSize + 1 );


	// draw handle rect:
	painter->setPen( Qt::blue.light() );
	painter->setBrush( Qt::NoBrush );

	painter->drawRect( m_qrect[0] );
	painter->setPen( Qt::blue.light() );


	// draw nodes:
	if( state() == VObject::selected )
	{
		painter->setPen( Qt::blue.light() );
		painter->setBrush( Qt::white );

		for( uint i = node_lt; i <= node_rb; ++i )
			painter->drawRect( m_qrect[i] );
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
		{
			m_boundingBox |= itr.current()->boundingBox();
		}

//		m_boundingBoxIsInvalid = false;
//	}

	return m_boundingBox;
}


VHandleNode
VSelection::handleNode( const QPoint& point ) const
{
	for( uint i = node_lt; i <= node_rb; ++i )
	{
		if( m_qrect[i].contains( point ) )
			return static_cast<VHandleNode>( i );
	}

	return node_none;
}

bool
VSelection::checkNode( const KoPoint &p )
{
	VSelectNodes op( p, false, 2 );

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

bool
VSelection::appendNode( const KoPoint &p )
{
	bool success = false;

	VSelectNodes op( p, true, 2 );

	VObjectListIterator itr = m_objects;
	for( ; itr.current(); ++itr )
	{
		if( op.visit( *itr.current() ) )
			success = true;
	}

	return success;
}

