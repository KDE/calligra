/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vfill.h"

VFill::VFill()
	: m_type( none ), m_fillRule( evenOdd )
{
	/*m_gradient.addStop( VColor( Qt::red.rgb() ), 0.0 );
	m_gradient.addStop( VColor( Qt::yellow.rgb() ), 1.0 );
	m_gradient.setOrigin( KoPoint( 0, 0 ) );
	m_gradient.setVector( KoPoint( 0, 50 ) );
	m_gradient.setSpreadMethod( gradient_spread_reflect );*/
}

VFill::VFill( const VColor &c )
	: m_type( solid ), m_fillRule( evenOdd )
{
	m_color = c;
}

VFill::VFill( const VFill& fill )
{
	// doesnt copy parent:
	*this = fill;
}

void
VFill::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "FILL" );
	element.appendChild( me );

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

	// save fill rule if necessary:
	if( m_fillRule != evenOdd )
		me.setAttribute( "fillRule", m_fillRule );
}

void
VFill::load( const QDomElement& element )
{
	m_fillRule = element.attribute( "fillRule" ) == 0 ?
		evenOdd : winding;

	// load color:
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "COLOR" )
			{
				m_type = solid;
				m_color.load( e );
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


