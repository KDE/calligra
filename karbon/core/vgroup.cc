/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include <koRect.h>

#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vvisitor.h"

#include <kdebug.h>


VGroup::VGroup( VObject* parent, VState state )
	: VObject( parent, state )
{
}

VGroup::VGroup( const VGroup& group, bool copy )
	: VObject( group )
{
	VObjectListIterator itr = group.m_objects;

	if( copy )
	{
		for ( ; itr.current() ; ++itr )
			append( itr.current()->clone() );
	}
	else
	{
		for ( ; itr.current() ; ++itr )
			append( itr.current() );
	}
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
VGroup::draw( VPainter* painter, const KoRect& rect ) const
{
	if(
		state() == state_deleted ||
		state() == state_hidden ||
		state() == state_hidden_locked )
	{
		return;
	}

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

const KoRect&
VGroup::boundingBox() const
{
	if( m_boundingBoxIsInvalid )
	{
		// clear:
		m_boundingBox = KoRect();

		VObjectListIterator itr = m_objects;
		for( ; itr.current(); ++itr )
		{
			m_boundingBox |= itr.current()->boundingBox();
		}

		m_boundingBoxIsInvalid = false;
	}

	return m_boundingBox;
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

VGroup*
VGroup::clone() const
{
	return new VGroup( *this );
}

void
VGroup::setFill( const VFill& fill )
{
	VObjectListIterator itr = m_objects;

	for ( ; itr.current() ; ++itr )
		itr.current()->setFill( fill );

	VObject::setFill( fill );
}

void
VGroup::setStroke( const VStroke& stroke )
{
	VObjectListIterator itr = m_objects;

	for ( ; itr.current() ; ++itr )
		itr.current()->setStroke( stroke );

	VObject::setStroke( stroke );
}

void
VGroup::setState( const VState state )
{
	VObjectListIterator itr = m_objects;

	for ( ; itr.current() ; ++itr )
		itr.current()->setState( state );

	VObject::setState( state );
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
				VPath* path = new VPath( this );
				path->load( e );
				append( path );
			}
			else if( e.tagName() == "TEXT" )
			{
				/*VText* text = new VText( this );
				text->load( e );
				append( text );*/
			}
		}
	}
}

void
VGroup::accept( VVisitor& visitor )
{
	visitor.visitVGroup( *this );
}


void
VGroup::take( const VObject& object )
{
	m_objects.removeRef( &object );
}

void
VGroup::prepend( VObject* object )
{
	object->setParent( this );

	m_objects.prepend( object );

	invalidateBoundingBox();
}

void
VGroup::append( VObject* object )
{
	object->setParent( this );

	m_objects.append( object );

	invalidateBoundingBox();
}

void
VGroup::clear()
{
	m_objects.clear();
}
