/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vobject.h"
#include "vstroke.h"


VStroke::VStroke( VObject* parent, float width, const VLineCap cap, const VLineJoin join,
			float miterLimit )
{
	m_parent = parent;
	m_type = stroke_stroke;
	m_lineWidth = width;
	m_lineCap = cap;
	m_lineJoin = join;
	m_miterLimit = miterLimit;
}

VStroke::VStroke( const VColor &c, VObject* parent, float width, const VLineCap cap, const VLineJoin join,
			float miterLimit )
{
	m_parent = parent;
	m_type = stroke_stroke;
	m_lineWidth = width;
	m_lineCap = cap;
	m_lineJoin = join;
	m_miterLimit = miterLimit;
	m_color = c;
}

VStroke::VStroke( const VStroke& stroke )
{
	// doesnt copy parent:
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
	me.setAttribute( "lineWidth", m_lineWidth );
	me.setAttribute( "lineCap", m_lineCap );
	me.setAttribute( "lineJoin", m_lineJoin );
	me.setAttribute( "miterLimit", m_miterLimit );

	if( m_type == stroke_stroke )
	{
		// save color:
		m_color.save( me );
	}
	else if( m_type == stroke_gradient )
	{
		// save gradient:
		m_gradient.save( me );
	}

	// save dashpattern:
	m_dashPattern.save( me );
}

void
VStroke::load( const QDomElement& element )
{
	// load stroke parameters:
	m_lineWidth = element.attribute( "lineWidth", "0.0" ).toDouble();
	if( m_lineWidth < 0.0 )
		m_lineWidth = 0.0;

	switch( element.attribute( "lineCap", "0" ).toUShort() )
	{
		case 1:
			m_lineCap = cap_round; break;
		case 2:
			m_lineCap = cap_square; break;
		default:
			m_lineCap = cap_butt;
	}

	switch( element.attribute( "lineJoin", "0" ).toUShort() )
	{
		case 1:
			m_lineJoin = join_round; break;
		case 2:
			m_lineJoin = join_bevel; break;
		default:
			m_lineJoin = join_miter;
	}

	m_miterLimit = element.attribute( "miterLimit", "0.0" ).toDouble();
	if( m_miterLimit < 0.0 )
		m_miterLimit = 0.0;


	// load color:
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "COLOR" )
			{
				m_color.load( e );
			}
			else if( e.tagName() == "DASHPATTERN" )
			{
				m_dashPattern.load( e );
			}
			else if( e.tagName() == "GRADIENT" )
			{
				m_type = stroke_gradient;
				m_gradient.load( e );
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
		m_lineCap = stroke.m_lineCap;
		m_lineJoin = stroke.m_lineJoin;
		m_miterLimit = stroke.m_miterLimit;
		m_color = stroke.m_color;
		m_dashPattern = stroke.m_dashPattern;
	}

	return *this;
}

