/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include <klocale.h>
#include <koRect.h>

#include "vlayer.h"
#include "vobject.h"
#include "vpath.h"
#include "vgroup.h"
#include "vobject.h"
//#include "vtext.h"
#include "vvisitor.h"

#include <kdebug.h>

VLayer::VLayer( VObject* parent, VState state )
	: VGroup( parent, state ), m_name( i18n( "Layer" ) )
{
}

VLayer::VLayer( const VLayer& layer )
	: VGroup( layer )
{
}

VLayer::~VLayer()
{
}

void
VLayer::draw( VPainter* painter, const KoRect& rect ) const
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
VLayer::bringToFront( const VObject& /*object*/ )
{
}

void
VLayer::upwards( const VObject& object )
{
	if( m_objects.getLast() == &object ) return;

	m_objects.remove();

	if( m_objects.current() != m_objects.getLast() )
	{
		m_objects.next();
		m_objects.insert( m_objects.at(), &object );
	}
	else
		m_objects.append( &object );
}

void
VLayer::downwards( const VObject& object )
{
	if( m_objects.getFirst() == &object ) return;

//	int index = m_objects.find( object );
	bool bLast = m_objects.getLast() == &object;
	m_objects.remove();

	if( !bLast ) m_objects.prev();

	m_objects.insert( m_objects.at(), &object );
}

void
VLayer::sentToBack( const VObject& /*object*/ )
{
}

VObjectList
VLayer::objectsWithinRect( const KoRect& rect ) const
{
	VObjectList list;
	VObjectListIterator itr( m_objects );

	for ( ; itr.current(); ++itr )
	{
		if( itr.current()->state() != state_deleted &&
			itr.current()->boundingBox().intersects( rect ) )
		{
			list.append( itr.current() );
		}
	}

	return list;
}

void
VLayer::removeDeletedObjects()
{
	for( m_objects.first(); m_objects.current(); m_objects.next() )
	{
		if( m_objects.current()->state() == state_deleted )
		{
			delete( m_objects.current() );
			m_objects.remove();
		}
	}
}

void
VLayer::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "LAYER" );
	element.appendChild( me );

	me.setAttribute( "name", m_name );

	if( state() == state_normal || state() == state_normal_locked )
		me.setAttribute( "visible", 1 );

	// save objects:
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->save( me );
}

void
VLayer::load( const QDomElement& element )
{
	m_objects.setAutoDelete( true );
	m_objects.clear();
	m_objects.setAutoDelete( false );

	m_name = element.attribute( "name" );
	setState( element.attribute( "visible" ) == 0 ? state_hidden : state_normal );

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
			else if( e.tagName() == "GROUP" )
			{
				VGroup* grp = new VGroup( this );
				grp->load( e );
				append( grp );
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


VLayer*
VLayer::clone() const
{
	return new VLayer( *this );
}

void
VLayer::accept( VVisitor& visitor )
{
	visitor.visitVLayer( *this );
}

