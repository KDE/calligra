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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdom.h>

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoOasisLoadingContext.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>

#include "vcomposite.h"
#include "shapes/vellipse.h"
#include "shapes/vrectangle.h"
#include "shapes/vsinus.h"
#include "shapes/vspiral.h"
#include "shapes/vstar.h"
#include "shapes/vpolyline.h"
#include "shapes/vpolygon.h"
#include "vfill.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vimage.h"
#include "vstroke.h"
#include "vvisitor.h"
#include "vclipgroup.h"
#ifdef HAVE_KARBONTEXT
#include "vtext.h"
#endif

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
VGroup::draw( VPainter* painter, const KoRect* rect ) const
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
		if( m_state == VObject::deleted || itr.current()->state() != VObject::deleted )
			itr.current()->setState( state );

	VObject::setState( state );
}

void
VGroup::save( QDomElement& element ) const
{
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "GROUP" );
		element.appendChild( me );

		// save objects:
		VObjectListIterator itr = m_objects;

		for ( ; itr.current(); ++itr )
			itr.current()->save( me );

		VObject::save( me );
	}
}

void
VGroup::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	// do not save deleted objects
	if( state() == deleted )
		return;

	docWriter->startElement( "draw:g" );

	// save objects:
	VObjectListIterator itr = m_objects;

	for ( ; itr.current(); ++itr )
		itr.current()->saveOasis( store, docWriter, mainStyles, ++index );

	docWriter->endElement();
}

bool
VGroup::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
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

			kdDebug(38000) << "VGroup::loadOasis: e.tagName() = " << e.tagName() << endl;
			kdDebug(38000) << "VGroup::loadOasis: e.namespaceURI() = " << e.namespaceURI() << endl;
			kdDebug(38000) << "VGroup::loadOasis: e.localName() = " << e.localName() << endl;

			if( e.namespaceURI() != KoXmlNS::draw )
				continue;

			context.styleStack().save();

			if( e.localName() == "path" || e.localName() == "custom-shape" )
			{
				VPath* composite = new VPath( this );
				composite->loadOasis( e, context );
				append( composite );
			}
			else if( e.localName() == "circle" || e.localName() == "ellipse" )
			{
				VEllipse* ellipse = new VEllipse( this );
				ellipse->loadOasis( e, context );
				append( ellipse );
			}
			else if( e.localName() == "rect" )
			{
				VRectangle* rectangle = new VRectangle( this );
				rectangle->loadOasis( e, context );
				append( rectangle );
			}
			else if( e.localName() == "g" )
			{
				VGroup* group = new VGroup( this );
				group->loadOasis( e, context );
				append( group );
			}
			else if( e.localName() == "polyline" || e.localName() == "line" )
			{
				VPolyline* polyline = new VPolyline( this );
				polyline->loadOasis( e, context );
				append( polyline );
			}
			else if( e.localName() == "polygon" )
			{
				VPolygon* polygon = new VPolygon( this );
				polygon->loadOasis( e, context );
				append( polygon );
			}

			context.styleStack().restore();
		}
	}

	return true;
}

void
VGroup::load( const QDomElement& element )
{
	m_objects.setAutoDelete( true );
	m_objects.clear();
	m_objects.setAutoDelete( false );

	VObject::load( element );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COMPOSITE" || e.tagName() == "PATH" ) // TODO : remove COMPOSITE later
			{
				VPath* composite = new VPath( this );
				composite->load( e );
				append( composite );
			}
			else if( e.tagName() == "ELLIPSE" )
			{
				VEllipse* ellipse = new VEllipse( this );
				ellipse->load( e );
				append( ellipse );
			}
			else if( e.tagName() == "RECT" )
			{
				VRectangle* rectangle = new VRectangle( this );
				rectangle->load( e );
				append( rectangle );
			}
			else if( e.tagName() == "POLYLINE" )
			{
				VPolyline* polyline = new VPolyline( this );
				polyline->load( e );
				append( polyline );
			}
			else if( e.tagName() == "POLYGON" )
			{
				VPolygon* polygon = new VPolygon( this );
				polygon->load( e );
				append( polygon );
			}
			else if( e.tagName() == "SINUS" )
			{
				VSinus* sinus = new VSinus( this );
				sinus->load( e );
				append( sinus );
			}
			else if( e.tagName() == "SPIRAL" )
			{
				VSpiral* spiral = new VSpiral( this );
				spiral->load( e );
				append( spiral );
			}
			else if( e.tagName() == "STAR" )
			{
				VStar* star = new VStar( this );
				star->load( e );
				append( star );
			}
			else if( e.tagName() == "GROUP" )
			{
				VGroup* group = new VGroup( this );
				group->load( e );
				append( group );
			}
			else if( e.tagName() == "CLIP" )
			{
				VClipGroup* grp = new VClipGroup( this );
				grp->load( e );
				append( grp );
			}
			else if( e.tagName() == "IMAGE" )
			{
				VImage* img = new VImage( this );
				img->load( e );
				append( img );
			}
			else if( e.tagName() == "TEXT" )
			{
#ifdef HAVE_KARBONTEXT
				VText *text = new VText( this );
				text->load( e );
				append( text );
#endif
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
VGroup::insertInfrontOf( VObject* newObject, VObject* oldObject )
{
	newObject->setParent( this );

	m_objects.insert( m_objects.find( oldObject ), newObject );

	invalidateBoundingBox();
}

void
VGroup::clear()
{
	m_objects.clear();

	invalidateBoundingBox();
}

