/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qpainter.h>
#include <qrect.h>

#include "vselection.h"

#include <kdebug.h>


VSelection::VSelection( VObject* parent )
	: VObject( parent )
{
	m_qrect = new QRect[9];
}

VSelection::VSelection( const VSelection& selection )
	: VObject( selection )
{
	m_qrect = new QRect[9];

	VObjectListIterator itr = selection.m_objects;
	for ( ; itr.current() ; ++itr )
		append( itr.current() );
}

VSelection::~VSelection()
{
	delete[]( m_qrect );

	clear();
}

VSelection*
VSelection::clone() const
{
	return new VSelection( *this );
}

void
VSelection::take( const VObject& object )
{
	m_objects.removeRef( &object );

	invalidateBoundingBox();
}

void
VSelection::append( VObject* object )
{
	m_objects.append( object );

	invalidateBoundingBox();
}

void
VSelection::clear()
{
	m_objects.clear();

	invalidateBoundingBox();
}

void
VSelection::draw( QPainter* painter, double zoomFactor ) const
{
	if( objects().count() == 0 )
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
		m_qrect[0].left() - m_nodeSize,
		m_qrect[0].top() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_mt].setRect(
		center.x() - m_nodeSize,
		m_qrect[0].top() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_rt].setRect(
		m_qrect[0].right() - m_nodeSize,
		m_qrect[0].top() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_rm].setRect(
		m_qrect[0].right() - m_nodeSize,
		center.y() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_rb].setRect(
		m_qrect[0].right() - m_nodeSize,
		m_qrect[0].bottom() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_mb].setRect(
		center.x() - m_nodeSize,
		m_qrect[0].bottom() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_lb].setRect(
		m_qrect[0].left() - m_nodeSize,
		m_qrect[0].bottom() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );
	m_qrect[node_lm].setRect(
		m_qrect[0].left() - m_nodeSize,
		center.y() - m_nodeSize,
		2 * m_nodeSize + 1, 2 * m_nodeSize + 1 );


	// draw handle rect:
	painter->setPen( Qt::blue.light() );
	painter->setBrush( Qt::NoBrush );

	painter->drawRect( m_qrect[0] );
	painter->setPen( Qt::blue.light() );


	// draw nodes:
	painter->setPen( Qt::blue.light() );
	painter->setBrush( Qt::white );

	for( uint i = node_lt; i <= node_rb; ++i )
		painter->drawRect( m_qrect[i] );
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
VSelection::node( const QPoint& point ) const
{
	for( uint i = node_lt; i <= node_rb; ++i )
	{
		if( m_qrect[i].contains( point ) )
			return static_cast<VHandleNode>( i );
	}

	return node_none;
}

