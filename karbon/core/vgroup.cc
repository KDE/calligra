/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vgroup.h"
#include "vpath.h"
#include "vlayer.h"

#include <qdom.h>

#include <koRect.h>

#include <kdebug.h>

VGroup::VGroup() : VObject()
{
}

VGroup::VGroup( const VObjectList &objects ) : VObject(), m_objects( objects )
{
	m_fill.setType( fill_unknown );
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
VGroup::draw( VPainter *painter, const KoRect& rect )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect );
}

void
VGroup::transform( const QWMatrix& m )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m );
}

void
VGroup::ungroup()
{
	VLayer *layer = static_cast<VLayer *>( parent() );
	if( !layer )
		return;

	// unregister from parent layer
	layer->removeRef( this );
	// inform all objects in this group their new parent
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		layer->appendObject( itr.current() );
	// done
	m_objects.clear();
}

void
VGroup::setState( const VState state )
{
    VObjectListIterator itr = m_objects;
    for ( ; itr.current() ; ++itr )
        itr.current()->setState( state );
}

bool
VGroup::isInside( const KoRect& rect ) const
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current() ; ++itr )
		if( itr.current()->isInside( rect ) )
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
	QDomElement me = element.ownerDocument().createElement( "GROUP" );
	element.appendChild( me );

	// save objects:
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->save( me );
}

void
VGroup::load( const QDomElement& element )
{
	m_objects.setAutoDelete( true );
	m_objects.clear();
	m_objects.setAutoDelete( false );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "PATH" )
			{
				VPath* path = new VPath();
				path->load( e );
				m_objects.append( path );
			}
			else if( e.tagName() == "TEXT" )
			{
				/*VText* text = new VText();
				text->load( e );
				insertObject( text );*/
			}
		}
	}
}

void
VGroup::insertObject( VObject* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
	object->setParent( this );
}
