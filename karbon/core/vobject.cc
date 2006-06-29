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
#include "vstroke.h"

#include <KoStore.h>
#include <KoGenStyles.h>
#include <KoStyleStack.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisStyles.h>

VObject::VObject( VObject* parent, VState state )
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

	/* TODO: porting to flake
	VDocument *srcDoc = obj.document();
	if( srcDoc && !srcDoc->objectName( &obj ).isEmpty() )
	{
		VDocument *dstDoc = document();
		if( dstDoc )
			dstDoc->setObjectName( this, srcDoc->objectName( &obj ) );
	}
	*/
}

VObject::~VObject()
{
	delete( m_stroke );
	delete( m_fill );
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

	/* TODO: porting to flake
	VDocument *doc = document();
	if( doc && !doc->objectName( this ).isEmpty() )
		element.setAttribute( "ID", QString( doc->objectName( this ) ) );
	*/
}

void
VObject::saveOasis( KoStore *, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	if( !name().isEmpty() )
		docWriter->addAttribute( "draw:name", name() );

	QMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0, -document()->height() );

	KoGenStyle styleobjectauto( VDocument::STYLE_GRAPHICAUTO, "graphic" );
	saveOasisFill( mainStyles, styleobjectauto );
	if( m_stroke )
	{
		// mirror stroke before saving
		VStroke stroke( *m_stroke );
		stroke.transform( mat );
		stroke.saveOasis( styleobjectauto );
	}
	QString st = mainStyles.lookup( styleobjectauto, "st" );
	if(document())
	        docWriter->addAttribute( "draw:id",  "obj" + QString::number( index ) );
	docWriter->addAttribute( "draw:style-name", st );
}

void
VObject::saveOasisFill( KoGenStyles &mainStyles, KoGenStyle &stylesobjectauto ) const
{
	if( m_fill )
	{
		QMatrix mat;
		mat.scale( 1, -1 );
		mat.translate( 0, -document()->height() );

		// mirror fill before saving
		VFill fill( *m_fill );
		fill.transform( mat );
		fill.saveOasis( mainStyles, stylesobjectauto );
	}
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

	/* TODO: porting to flake
	VDocument *doc = document();
	if( doc && !element.attribute( "ID" ).isEmpty() )
		doc->setObjectName( this, element.attribute( "ID" ) );
	*/
}

bool
VObject::loadOasis( const QDomElement &object, KoOasisLoadingContext &context )
{
	if( !m_stroke )
		m_stroke = new VStroke( this );

	if( !m_fill )
		m_fill = new VFill();

	if( object.hasAttributeNS( KoXmlNS::draw, "style-name" ) )
		context.fillStyleStack( object, KoXmlNS::draw, "style-name", "graphic" );

	KoStyleStack &styleStack = context.styleStack();
	styleStack.setTypeProperties( "graphic" );
	m_stroke->loadOasis( styleStack );
	m_fill->loadOasis( object, context, this );

	if( object.hasAttributeNS( KoXmlNS::draw, "name" ) )
		setName( object.attributeNS( KoXmlNS::draw, "name", QString::null ) );

	return true;
}

void
VObject::addStyles( const QDomElement* style, KoOasisLoadingContext & context )
{
	if(style)
	{
		// this function is necessary as parent styles can have parents themself
		if( style->hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
			addStyles( context.oasisStyles().findStyle( style->attributeNS( KoXmlNS::style, "parent-style-name", QString::null ) ), context );
		context.addStyles( style, "style-name" );
	}
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
	return QString();
	// TODO: porting to flake
	//return document() ? document()->objectName( this ) : QString();
}

void
VObject::setName( const QString &s )
{
	// TODO: porting to flake
	/*
	if( document() )
		document()->setObjectName( this, s );
	*/
}

