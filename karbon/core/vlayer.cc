/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vlayer.h"
#include "vobject.h"
#include <kdebug.h>

VLayer::VLayer()
	: m_isVisible( true ), m_isReadOnly( false )
{
}

VLayer::~VLayer()
{
	QPtrListIterator<VObject> itr = m_objects;
	for ( ; itr.current() ; ++itr )
	{
		delete( itr.current() );
	}
}

void
VLayer::draw( QPainter& painter, const QRect& rect, const double& zoomFactor )
{
	QPtrListIterator<VObject> itr = m_objects;
	for ( ; itr.current() ; ++itr )
		itr.current()->draw( painter, rect, zoomFactor );
}

void
VLayer::insertObject( const VObject* object )
{
	// put new objects "on top" by appending them:
	m_objects.append( object );
}

void
VLayer::selectAllObjects()
{
	// select all objects within this layer
    QPtrListIterator<VObject> itr = m_objects;
    for ( ; itr.current() ; ++itr )
        if( itr.current()->state() != VObject::deleted )
            itr.current()->setState( VObject::selected );
}

void
VLayer::selectObjects( const QRect &rect, QPtrList<VObject> &list )
{
	// select all objects within the rect coords
	QPtrListIterator<VObject> itr = m_objects;
    for ( ; itr.current() ; ++itr )
    {
		if( itr.current()->state() != VObject::deleted &&
			itr.current()->boundingBox().intersects( rect ) )
		{
			itr.current()->setState( VObject::selected );
			list.append( itr.current() );
		}
    }
}

void
VLayer::unselectObjects()
{
	// unselect all objects in this layer that were selected
	QPtrListIterator<VObject> itr = m_objects;
    for ( ; itr.current() ; ++itr )
    {
		if( itr.current()->state() == VObject::selected )
			itr.current()->setState( VObject::normal );
    }
}

void
VLayer::deleteObjects( QPtrList<VObject> &list )
{
	QPtrListIterator<VObject> itr = m_objects;
    for ( ; itr.current() ; ++itr )
    {
		if( itr.current()->state() == VObject::selected )
		{
			itr.current()->setState( VObject::deleted );
			list.append( itr.current() );
		}
	}
}
