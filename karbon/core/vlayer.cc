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

#include <kdebug.h>

VLayer::VLayer()
	: m_visible( true ), m_readOnly( false ), m_name( i18n( "Layer" ) )
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
VLayer::draw( VPainter *painter, const QRect& rect,
	const double zoomFactor )
{
	VObjectListIterator itr = m_objects;
	for ( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect, zoomFactor );
}

void
VLayer::insertObject( const VObject* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
}

VObjectList
VLayer::objectsWithinRect( const QRect& rect,
	const double zoomFactor ) const
{
	VObjectList list;
	VObjectListIterator itr( m_objects );

	for ( ; itr.current(); ++itr )
	{
		if( itr.current()->state() != VObject::deleted &&
			itr.current()->intersects( rect, zoomFactor ) )
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
		if( m_objects.current()->state() == VObject::deleted )
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
				insertObject( path );
			}
		}
	}
}
