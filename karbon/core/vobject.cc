/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>

#include "vdocument.h"
#include "vfill.h"
#include "vobject.h"
#include "vobject_iface.h"
#include "vstroke.h"

#include <koStore.h>
#include <koGenStyles.h>
#include <koStyleStack.h>
#include <koxmlwriter.h>
#include <koxmlns.h>
#include <kooasiscontext.h>
#include <koOasisStyles.h>

VObject::VObject( VObject* parent, VState state ) : m_dcop( 0L )
{
	m_stroke = 0L;
	m_fill = 0L;
	
	m_parent = parent;
	m_state = state;

	invalidateBoundingBox();
}

VObject::VObject( const VObject& obj )
{
	m_stroke = 0L;
	m_fill = 0L;

	m_parent = obj.m_parent;
	m_state = obj.m_state;

	invalidateBoundingBox();
	m_dcop = 0L;

	if( obj.document() && !obj.document()->objectName( &obj ).isEmpty() )
		if( document() )
			document()->setObjectName( this, obj.document()->objectName( &obj ) );
}

VObject::~VObject()
{
	delete( m_stroke );
	delete( m_fill );
	delete m_dcop;
}

DCOPObject *
VObject::dcopObject()
{
    if ( !m_dcop )
		m_dcop = new VObjectIface( this );

    return m_dcop;
}

void
VObject::setStroke( const VStroke& stroke )
{
	if( !m_stroke )
		m_stroke = new VStroke( this );

	*m_stroke = stroke;
}

void
VObject::setFill( const VFill& fill )
{
	if( !m_fill )
		m_fill = new VFill();

	*m_fill = fill;
}

void
VObject::save( QDomElement& element ) const
{
	if( m_stroke )
		m_stroke->save( element );

	if( m_fill )
		m_fill->save( element );

	if( document() && !document()->objectName( this ).isEmpty() )
		element.setAttribute( "ID", QString( document()->objectName( this ) ) );
}

void
VObject::saveOasis( KoStore *, KoXmlWriter *docWriter, KoGenStyles &mainStyles ) const
{
	if( !name().isEmpty() )
		docWriter->addAttribute( "draw:name", name() );

	KoGenStyle styleobjectauto( VDocument::STYLE_GRAPHICAUTO, "graphics" );
	if( m_fill )
		m_fill->saveOasis( mainStyles, styleobjectauto );
	if( m_stroke )
		m_stroke->saveOasis( styleobjectauto );
	QString st = mainStyles.lookup( styleobjectauto, "st" );
	docWriter->addAttribute( "draw:style-name", st );
}

void
VObject::load( const QDomElement& element )
{
	if( !m_stroke )
		m_stroke = new VStroke( this );

	if( !m_fill )
		m_fill = new VFill();


	if( element.tagName() == "STROKE" )
	{
		m_stroke->load( element );
	}
	else if( element.tagName() == "FILL" )
	{
		m_fill->load( element );
	}

	if( document() && !element.attribute( "ID" ).isEmpty() )
		document()->setObjectName( this, element.attribute( "ID" ) );
}

bool
VObject::loadOasis( const QDomElement &object, KoOasisContext &context )
{
	if( !m_stroke )
		m_stroke = new VStroke( this );

	if( !m_fill )
		m_fill = new VFill();

	if( object.hasAttributeNS( KoXmlNS::draw, "style-name" ) )
		addStyles( context.oasisStyles().styles()[object.attributeNS( KoXmlNS::draw, "style-name", QString::null )], context );

	KoStyleStack &styleStack = context.styleStack();
	styleStack.setTypeProperties( "graphic" );
	m_stroke->loadOasis( styleStack );
	m_fill->loadOasis( object, context );
	return true;
}

void
VObject::addStyles( const QDomElement* style, KoOasisContext & context )
{
	// this function is necessary as parent styles can have parents themself
	if( style->hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
		addStyles( context.oasisStyles().styles()[style->attributeNS( KoXmlNS::style, "parent-style-name", QString::null )], context );
	context.addStyles( style );
}

VDocument *
VObject::document() const
{
	VObject *obj = (VObject *)this;
	while( obj->parent() )
		obj = obj->parent();
	return dynamic_cast<VDocument *>( obj );
}

QString
VObject::name() const
{
	return document() ? document()->objectName( this ) : QString();
}

void
VObject::setName( const QString &s )
{
	if( document() )
		document()->setObjectName( this, s );
}

