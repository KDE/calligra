/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vgroup.h"

#include <qdom.h>

#include <koRect.h>

#include <kdebug.h>

VGroup::VGroup( const VObjectList &objects ) : VObject(), m_objects( objects )
{
	fill().setType( fill_unknown );
	//stroke().setType( stroke_unknown );
}

VGroup::VGroup( const VGroup& other ) : VObject()
{
	// copy objects
	VObjectListIterator itr = other.m_objects;
	for ( ; itr.current() ; ++itr )
		m_objects.append( itr.current()->clone() );
}

VGroup::~VGroup()
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
	{
		delete( itr.current() );
	}
}

void
VGroup::draw( VPainter *painter, const KoRect& rect,
	const double zoomFactor )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect, zoomFactor );
}

void
VGroup::transform( const QWMatrix& m )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m );
}

void
VGroup::empty()
{
	m_objects.clear();
}

void
VGroup::setState( const VState state )
{
    VObjectListIterator itr = m_objects;
    for ( ; itr.current() ; ++itr )
        itr.current()->setState( state );
}

KoRect
VGroup::boundingBox( const double zoomFactor ) const
{
	return m_objects.boundingBox( zoomFactor );
}

bool
VGroup::intersects( const KoRect& rect, const double zoomFactor ) const
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		if( itr.current()->intersects( rect, zoomFactor ) )
			return true;

	return false;
}

VObject*
VGroup::clone()
{
	return new VGroup( *this );
}

void
VGroup::setFill( const VFill &f )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->setFill( f );

	VObject::setFill( f );
}

void
VGroup::setStroke( const VStroke &s )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->setStroke( s );

	VObject::setStroke( s );
}

void
VGroup::save( QDomElement& element ) const
{
}

void
VGroup::load( const QDomElement& element )
{
}
