/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdom.h>

#include <klocale.h>
#include <koRect.h>

#include "vcomposite.h"
#include "vdocument.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vobject.h"
#include "vtext.h"
#include "vvisitor.h"
#include "vlayer_iface.h"
#include <kdebug.h>
#include "vclipgroup.h"

VLayer::VLayer( VObject* parent, VState state )
	: VGroup( parent, state ), m_selected( true ), m_name( i18n( "Layer" ) )
{
}

VLayer::VLayer( const VLayer& layer )
	: VGroup( layer )
{
}

VLayer::~VLayer()
{
}

DCOPObject* VLayer::dcopObject()
{
    if ( !dcop )
	dcop = new VLayerIface( this );

    return dcop;
}

void
VLayer::draw( VPainter* painter, const KoRect* rect ) const
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
VLayer::bringToFront( const VObject& object )
{
	if( m_objects.getLast() == &object ) return;

	m_objects.remove( &object );

	m_objects.append( &object );
}

void
VLayer::upwards( const VObject& object )
{
	if( m_objects.getLast() == &object ) return;

	m_objects.remove( &object );

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

	int index = m_objects.find( &object );
	bool bLast = m_objects.getLast() == &object;
	m_objects.remove( index );

	if( !bLast ) m_objects.prev();

	m_objects.insert( m_objects.at(), &object );
}

void
VLayer::sendToBack( const VObject& object )
{
	if( m_objects.getFirst() == &object ) return;

	m_objects.remove( &object );

	m_objects.prepend( &object );
}

void
VLayer::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "LAYER" );
	element.appendChild( me );

	me.setAttribute( "name", m_name );

	if( state() == normal || state() == normal_locked )
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
	setState( element.attribute( "visible" ) == 0 ? hidden : normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COMPOSITE" )
			{
				VComposite* composite = new VComposite( this );
				composite->load( e );
				append( composite );
			}
			else if( e.tagName() == "GROUP" )
			{
				VGroup* grp = new VGroup( this );
				grp->load( e );
				append( grp );
			}
			else if( e.tagName() == "CLIP" )
			{
				VClipGroup* grp = new VClipGroup( this );
				grp->load( e );
				append( grp );
			}
			else if( e.tagName() == "TEXT" )
			{
				VText* text = new VText( this );
				text->load( e );
				append( text );
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

