/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include <koRect.h>

#include "vfill.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vstroke.h"
#include "vvisitor.h"

#include <kdebug.h>


VGroup::VGroup( VObject* parent, VState state )
	: VObject( parent, state )
{
	m_stroke = new VStroke( this );
	m_fill = new VFill();
}

VGroup::VGroup( const VGroup& group )
	: VObject( group )
{
	m_stroke = new VStroke( *group.m_stroke );
	m_stroke->setParent( this );
	m_fill = new VFill( *group.m_fill );

	VObjectListIterator itr = group.m_objects;
	for ( ; itr.current() ; ++itr )
		append( itr.current()->clone() );
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
		state() == deleted ||
		state() == hidden ||
		state() == hidden_locked )
	{
		return;
	}

	VObjectListIterator itr = m_objects;

	for ( ; itr.current(); ++itr )
		itr.current()->draw( painter, rect );
}

void
VGroup::transform( const QWMatrix& m, bool selectedSubObjects )
{
	VObjectListIterator itr = m_objects;

	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m, selectedSubObjects );
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
VGroup::insertNewDeleteOld( VObject* newObject, VObject* oldObject )
{
	m_objects.insert( m_objects.find( oldObject ), newObject );

	oldObject->setState( VObject::deleted );
}

void
VGroup::clear()
{
	m_objects.clear();

	invalidateBoundingBox();
}

