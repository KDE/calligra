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

#include <KoGenStyles.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>

#include "vobject.h"
#include "vstroke.h"
#include <kdebug.h>

VStroke::VStroke( VObject* parent, float width, const VLineCap cap, const VLineJoin join,
			float miterLimit )
{
	m_parent = parent;
	m_type = solid;
	m_lineWidth = width;
	m_lineCap = cap;
	m_lineJoin = join;
	m_miterLimit = miterLimit;
}

VStroke::VStroke( const VColor &c, VObject* parent, float width, const VLineCap cap, const VLineJoin join,
			float miterLimit )
{
	m_parent = parent;
	m_type = solid;
	m_lineWidth = width;
	m_lineCap = cap;
	m_lineJoin = join;
	m_miterLimit = miterLimit;
	m_color = c;
}

VStroke::VStroke( const VStroke& stroke )
{
	// doesn't copy parent:
	*this = stroke;
}

void
VStroke::setLineWidth( float width )
{
	m_lineWidth = width;

	// tell our parent so he can update his bbox:
	if( m_parent )
		m_parent->invalidateBoundingBox();
}

void
VStroke::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "STROKE" );
	element.appendChild( me );

	// save stroke parameters:
	if( m_lineWidth != 1.0 )
		me.setAttribute( "lineWidth", m_lineWidth );
	if( !( m_lineCap == capButt ) )
		me.setAttribute( "lineCap", m_lineCap );
	if( !( m_lineJoin == joinMiter ) )
		me.setAttribute( "lineJoin", m_lineJoin );
	if( m_miterLimit != 10.0 )
		me.setAttribute( "miterLimit", m_miterLimit );

	if( m_type == solid )
	{
		// save color:
		m_color.save( me );
	}
	else if( m_type == grad )
	{
		// save gradient:
		m_gradient.save( me );
	}
	else if( m_type == patt )
	{
		// save pattern:
		m_pattern.save( me );
	}

	// save dashpattern:
	m_dashPattern.save( me );
}

void
VStroke::saveOasis( KoGenStyle &style ) const
{
	if( m_type == solid )
	{
		style.addProperty( "draw:stroke", "solid" );
		style.addProperty( "svg:stroke-color", QColor( m_color ).name() );
		style.addPropertyPt( "svg:stroke-width", m_lineWidth );
		if( m_color.opacity() < 1 )
			style.addProperty( "svg:stroke-opacity", QString( "%1%" ).arg( m_color.opacity() * 100. ) );
	}
	else if( m_type == none )
		style.addProperty( "draw:stroke", "none" );
	/*else if( m_type == grad )
		style.addProperty( "draw:stroke", "gradient" );
	else if( m_type == patt )
		style.addProperty( "draw:stroke", "hatch" );*/

	if( m_lineJoin == joinRound )
		style.addProperty( "draw:stroke-linejoin", "round" );
	else if( m_lineJoin == joinBevel )
		style.addProperty( "draw:stroke-linejoin", "bevel" );
	else if( m_lineJoin == joinMiter )
		style.addProperty( "draw:stroke-linejoin", "miter" );
}

void
VStroke::loadOasis( const KoStyleStack &stack )
{
	if( stack.hasAttributeNS( KoXmlNS::draw, "stroke" ))
	{
		if( stack.attributeNS( KoXmlNS::draw, "stroke" ) == "solid" )
		{
			setType( VStroke::solid );
			setColor( QColor( stack.attributeNS( KoXmlNS::svg, "stroke-color" ) ) );
			if( stack.hasAttributeNS( KoXmlNS::svg, "stroke-opacity" ) )
				m_color.setOpacity( stack.attributeNS( KoXmlNS::svg, "stroke-opacity" ).remove( '%' ).toFloat() / 100. );
			QString join = stack.attributeNS( KoXmlNS::draw, "stroke-linejoin" );
			if( !join.isEmpty() )
			{
				if( join == "round" )
					m_lineJoin = joinRound;
				else if( join == "bevel" )
					m_lineJoin = joinBevel;
				else
					m_lineJoin = joinMiter;
			}
		}
		else if( stack.attributeNS( KoXmlNS::draw, "stroke" ) == "none" )
			setType( VStroke::none );
	}
	if( stack.hasAttributeNS( KoXmlNS::svg, "stroke-width" ) )
		m_lineWidth = KoUnit::parseValue( stack.attributeNS( KoXmlNS::svg, "stroke-width" ) );
	if( m_lineWidth < 0.0 )
		m_lineWidth = 0.0;
}

void
VStroke::load( const QDomElement& element )
{
	m_type = none;
	// load stroke parameters:
	m_lineWidth = element.attribute( "lineWidth", "1.0" ).toDouble();
	if( m_lineWidth < 0.0 )
		m_lineWidth = 0.0;

	switch( element.attribute( "lineCap", "0" ).toUShort() )
	{
		case 1:
			m_lineCap = capRound; break;
		case 2:
			m_lineCap = capSquare; break;
		default:
			m_lineCap = capButt;
	}

	switch( element.attribute( "lineJoin", "0" ).toUShort() )
	{
		case 1:
			m_lineJoin = joinRound; break;
		case 2:
			m_lineJoin = joinBevel; break;
		default:
			m_lineJoin = joinMiter;
	}

	m_miterLimit = element.attribute( "miterLimit", "10.0" ).toDouble();
	if( m_miterLimit < 0.0 )
		m_miterLimit = 0.0;


	// load color:
	QDomNodeList list = element.childNodes();
	for( int i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "COLOR" )
			{
				m_color.load( e );
				m_type = solid;
			}
			else if( e.tagName() == "DASHPATTERN" )
			{
				m_dashPattern.load( e );
			}
			else if( e.tagName() == "GRADIENT" )
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


VStroke&
VStroke::operator=( const VStroke& stroke )
{
	if( this != &stroke )
	{
		// dont copy the parent!
		m_type = stroke.m_type;

		m_lineWidth = stroke.m_lineWidth;
		// Tell our parent about the linewidth change, so he can update his bbox:
		//if( m_parent )
		//	m_parent->invalidateBoundingBox();

		m_lineCap = stroke.m_lineCap;
		m_lineJoin = stroke.m_lineJoin;
		m_miterLimit = stroke.m_miterLimit;
		m_color = stroke.m_color;
		m_dashPattern = stroke.m_dashPattern;
		m_gradient = stroke.m_gradient;
		m_pattern = stroke.m_pattern;
	}

	return *this;
}

void 
VStroke::transform( const QMatrix& m )
{
	if( type() == VStroke::grad )
		gradient().transform( m );
	else if( type() == VStroke::patt )
		pattern().transform( m );
}
