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

#include <kdebug.h>

VLayer::VLayer()
	: m_name( i18n( "Layer" ) ), m_visible( true ), m_readOnly( false )
{
}

VLayer::~VLayer()
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
	{
		delete( itr.current() );
	}
}

void
VLayer::draw( VPainter *painter, const KoRect& rect )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect );
}

void
VLayer::appendObject( VShape* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
	object->setParent( this );
}

void
VLayer::prependObject( VShape* object )
{
	// prepend object
	m_objects.prepend( object );
	object->setParent( this );
}

void
VLayer::removeRef( const VShape* object )
{
	//
	m_objects.removeRef( object );
}

void
VLayer::moveObjectDown( const VShape* object )
{
	if( m_objects.getFirst() == object ) return;

//	int index = m_objects.find( object );
	bool bLast = m_objects.getLast() == object;
	m_objects.remove();
	if( !bLast ) m_objects.prev();
	m_objects.insert( m_objects.at(), object );
}

void
VLayer::moveObjectUp( const VShape* object )
{
	if( m_objects.getLast() == object ) return;

//	int index = m_objects.find( object );
	//kdDebug() << "Index : " << index << endl;
	m_objects.remove();
	if( m_objects.current() != m_objects.getLast() )
	{
		m_objects.next();
		m_objects.insert( m_objects.at(), object );
	}
	else m_objects.append( object );
}

VObjectList
VLayer::objectsWithinRect( const KoRect& rect ) const
{
	VObjectList list;
	VObjectListIterator itr( m_objects );

	for ( ; itr.current(); ++itr )
	{
		if( itr.current()->state() != state_deleted &&
			itr.current()->isInside( rect ) )
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
	me.setAttribute( "visible", m_visible );

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
	m_visible = element.attribute( "visible" ) == 0 ? false : true;

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
				appendObject( path );
			}
			else if( e.tagName() == "GROUP" )
			{
				VGroup* grp = new VGroup();
				grp->load( e );
				appendObject( grp );
			}
			else if( e.tagName() == "TEXT" )
			{
				/*VText* text = new VText();
				text->load( e );
				appendObject( text );*/
			}
		}
	}
}
