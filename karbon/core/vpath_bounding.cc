/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <koRect.h>

#include "vpath_bounding.h"

VPathBounding::VPathBounding()
	: VSegmentListTraverser()
{
}

void
VPathBounding::calculate( KoRect& rect, const VSegmentList& list  )
{
	m_rect = &rect;

	VSegment::traverse( list, *this );
}

bool
VPathBounding::begin( const KoPoint& p )
{
	m_rect->setCoords( p.x(), p.y(), p.x(), p.y() );

	setPreviousPoint( p );

	return true;
}

bool
VPathBounding::curveTo ( const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 )
{
	if( p1.x() < m_rect->left() )
		m_rect->setLeft( p1.x() );
	else if( p1.x() > m_rect->right() )
		m_rect->setRight( p1.x() );
	if( p1.y() > m_rect->top() )
		m_rect->setTop( p1.y() );
	else if( p1.y() < m_rect->bottom() )
		m_rect->setBottom( p1.y() );

	if( p2.x() < m_rect->left() )
		m_rect->setLeft( p2.x() );
	else if( p2.x() > m_rect->right() )
		m_rect->setRight( p2.x() );
	if( p2.y() > m_rect->top() )
		m_rect->setTop( p2.y() );
	else if( p2.y() < m_rect->bottom() )
		m_rect->setBottom( p2.y() );

	if( p3.x() < m_rect->left() )
		m_rect->setLeft( p3.x() );
	else if( p3.x() > m_rect->right() )
		m_rect->setRight( p3.x() );
	if( p3.y() > m_rect->top() )
		m_rect->setTop( p3.y() );
	else if( p3.y() < m_rect->bottom() )
		m_rect->setBottom( p3.y() );

	setPreviousPoint( p3 );

	return true;
}

bool
VPathBounding::lineTo( const KoPoint& p )
{
	if( p.x() < m_rect->left() )
		m_rect->setLeft( p.x() );
	else if( p.x() > m_rect->right() )
		m_rect->setRight( p.x() );
	if( p.y() > m_rect->top() )
		m_rect->setTop( p.y() );
	else if( p.y() < m_rect->bottom() )
		m_rect->setBottom( p.y() );

	setPreviousPoint( p );

	return true;
}
