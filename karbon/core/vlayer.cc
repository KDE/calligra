/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>

#include <klocale.h>
#include <KoRect.h>

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
#include "vfill.h"
#include "vstroke.h"

VLayer::VLayer( VObject* parent, VState state )
	: VGroup( parent, state )
{
	setName( "Layer" );
	// HACK : vlayer just shouldn't have fill/stroke at all
	delete m_fill;
	m_fill = 0L;
	delete m_stroke;
	m_stroke = 0L;
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
    if ( !m_dcop )
		m_dcop = new VLayerIface( this );

    return m_dcop;
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
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "LAYER" );
		element.appendChild( me );

		if( state() == normal || state() == normal_locked || state() == VObject::selected )
			me.setAttribute( "visible", 1 );

		// save objects:
		VObjectListIterator itr = m_objects;
		for ( ; itr.current(); ++itr )
			itr.current()->save( me );

		VObject::save( me );
	}
}

void
VLayer::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	// do not save deleted objects
	if( state() == deleted )
		return;

	// save objects:
	VObjectListIterator itr = m_objects;

	for ( ; itr.current(); ++itr )
		itr.current()->saveOasis( store, docWriter, mainStyles, ++index );
}

void
VLayer::load( const QDomElement& element )
{
	setState( element.attribute( "visible" ) == 0 ? hidden : normal );
	VGroup::load( element );
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

