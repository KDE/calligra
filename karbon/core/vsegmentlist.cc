/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vsegmentlist.h"

#include <kdebug.h>

VSegmentList::VSegmentList()
	: m_isClosed( false )
{
	setAutoDelete( true );

	// add initial segment:
	append( new VSegment() );
}

VSegmentList::VSegmentList( const VSegmentList& list )
	: m_isClosed( false )
{
	setAutoDelete( true );

	VSegmentListIterator itr( list );
	for( ; itr.current() ; ++itr )
	{
		append( new VSegment( *( itr.current() ) ) );
	}

}

VSegmentList::~VSegmentList()
{
}

void
VSegmentList::close()
{
	// move end-segment if one already exists:
	if( getLast()->type() == VSegment::end )
	{
		getLast()->
			setPoint( 3, getFirst()->point( 3 ) );
	}
	// append one, if no end-segment exists:
	else if( getLast()->point( 3 ) != getFirst()->point( 3 ) )
	{
		VSegment* s = new VSegment();
		s->setType( VSegment::end );
		s->setPoint( 3, getFirst()->point( 3 ) );
		append( s );
	}
	

	m_isClosed = true;
}

void
VSegmentList::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "SEGMENTS" );
	element.appendChild( me );

	if( m_isClosed )
		me.setAttribute( "isClosed", m_isClosed );

	// save segments:
	VSegmentListIterator itr( *this );
	for( ; itr.current() ; ++itr )
	{
		itr.current()->save( me );
	}
}

void
VSegmentList::load( const QDomElement& element )
{
	clear();
	m_isClosed   = element.attribute( "isClosed" ) == 0 ? false : true;

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement segment = list.item( i ).toElement();

			VSegment* s = new VSegment();
			s->load( segment );
			append( s );
		}
	}
}

