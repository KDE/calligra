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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <kdebug.h>

#include <KoGenStyles.h>
#include <KoOasisLoadingContext.h>
#include <KoOasisStyles.h>
#include <KoXmlNS.h>

#include "vfill.h"

VFill::VFill()
	: m_type( none )
{
	/*m_gradient.addStop( VColor( Qt::red.rgb() ), 0.0 );
	m_gradient.addStop( VColor( Qt::yellow.rgb() ), 1.0 );
	m_gradient.setOrigin( QPointF( 0, 0 ) );
	m_gradient.setVector( QPointF( 0, 50 ) );
	m_gradient.setSpreadMethod( gradient_spread_reflect );*/
	//kDebug(38000) << "Size of VFill : " << sizeof(*this) << endl;
}

VFill::VFill( const VColor &c )
	: m_type( solid )
{
	m_color = c;
	//kDebug(38000) << "Size of VFill : " << sizeof(*this) << endl;
}

VFill::VFill( const VFill& fill )
{
	// doesn't copy parent:
	*this = fill;
}

void
VFill::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "FILL" );
	element.appendChild( me );

	if( !( m_type == none ) )
	{
		// save color:
		m_color.save( me );
	}
	if( m_type == grad )
	{
		// save gradient:
		m_gradient.save( me );
	}
	else if( m_type == patt )
	{
		// save pattern:
		m_pattern.save( me );
	}
}

void
VFill::saveOasis( KoGenStyles &mainStyles, KoGenStyle &style ) const
{
	if( m_type == solid )
	{
		style.addProperty( "draw:fill", "solid" );
		style.addProperty( "draw:fill-color", QColor( m_color ).name() );
		if( m_color.opacity() < 1 )
			style.addProperty( "draw:opacity", QString( "%1%" ).arg( m_color.opacity() * 100. ) );
	}
	else if( m_type == grad )
	{
		style.addProperty( "draw:fill", "gradient" );
		QString grad = m_gradient.saveOasis( mainStyles );
		style.addProperty( "draw:fill-gradient-name", grad );
		if( m_color.opacity() < 1 )
			style.addProperty( "draw:opacity", QString( "%1%" ).arg( m_color.opacity() * 100. ) );
	}
	else if( m_type == patt )
		style.addProperty( "draw:fill", "hatch" );
	else
		style.addProperty( "draw:fill", "none" );
}

void
VFill::loadOasis( const QDomElement &/*object*/, KoOasisLoadingContext &context, VObject* parent )
{
	KoStyleStack &stack = context.styleStack();
	if( stack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
	{
		if( stack.attributeNS( KoXmlNS::draw, "fill" ) == "solid" )
		{
			setType( VFill::solid );
			setColor( QColor( stack.attributeNS( KoXmlNS::draw, "fill-color" ) ) );
		}
		else if( stack.attributeNS( KoXmlNS::draw, "fill" ) == "gradient" )
		{
			setType( VFill::grad );
			QString style = stack.attributeNS( KoXmlNS::draw, "fill-gradient-name" );
			kDebug()<<" style gradient name :"<<style<<endl;
			QDomElement *grad = context.oasisStyles().drawStyles()[ style ];
			kDebug()<<" style gradient name :"<< grad <<endl;
			if( grad )
				m_gradient.loadOasis( *grad, stack, parent );
		}
		if( stack.hasAttributeNS( KoXmlNS::draw, "opacity" ) )
			m_color.setOpacity( stack.attributeNS( KoXmlNS::draw, "opacity" ).remove( '%' ).toFloat() / 100. );
	}
}

void
VFill::load( const QDomElement& element )
{
	m_type = none;

	// load color:
	QDomNodeList list = element.childNodes();
	for( int i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "COLOR" )
			{
				m_type = solid;
				m_color.load( e );
			}
			if( e.tagName() == "GRADIENT" )
			{
				m_type = grad;
				m_gradient.load( e );
			}
			else if( e.tagName() == "PATTERN" )
			{
				m_type = patt;
				m_pattern.load( e );
			}
		}
	}
}

VFill&
VFill::operator=( const VFill& fill )
{
	if( this != &fill )
	{
		// dont copy the parent!
		m_type = fill.m_type;
		m_color = fill.m_color;
		m_gradient = fill.m_gradient;
		m_pattern = fill.m_pattern;
	}

	return *this;
}

void 
VFill::transform( const QMatrix& m )
{
	if( type() == VFill::grad )
		gradient().transform( m );
	else if( type() == VFill::patt )
		pattern().transform( m );
}
