/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vfill.h"

VFill::VFill()
	: m_type( fill_none ), m_fillRule( fillrule_evenOdd )
{
	/*m_gradient.addStop( VColor( Qt::red.rgb() ), 0.0 );
	m_gradient.addStop( VColor( Qt::yellow.rgb() ), 1.0 );
	m_gradient.setOrigin( KoPoint( 0, 0 ) );
	m_gradient.setVector( KoPoint( 0, 50 ) );
	m_gradient.setSpreadMethod( gradient_spread_reflect );*/
}

void
VFill::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "FILL" );
	element.appendChild( me );

	if( m_type == fill_fill )
	{
		// save color:
		m_color.save( me );
	}
	else if( m_type == fill_gradient )
	{
		// save gradient:
		m_gradient.save( me );
	}

	// save fill rule if necessary:
	if( m_fillRule != fillrule_evenOdd )
		me.setAttribute( "fillRule", m_fillRule );
}

void
VFill::load( const QDomElement& element )
{
	m_fillRule = element.attribute( "fillRule" ) == 0 ?
		fillrule_evenOdd : fillrule_winding;

	// load color:
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();
			if( e.tagName() == "COLOR" )
			{
				m_type = fill_fill;
				m_color.load( e );
			}
			else if( e.tagName() == "GRADIENT" )
			{
				m_type = fill_gradient;
				m_gradient.load( e );
			}
		}
	}
}



