/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vgradient.h"

VGradient::VGradient( VGradientType type )
	: m_type( type )
{
	// set up dummy gradient
	addStop( VColor( Qt::red.rgb() ), 0.0, 0.5 );
	addStop( VColor( Qt::yellow.rgb() ), 1.0, 0.5 );
	setOrigin( KoPoint( 0, 0 ) );
	setVector( KoPoint( 0, 50 ) );
	setRepeatMethod( VGradient::reflect );
}

void
VGradient::clearStops()
{
	m_colorStops.clear();
}

void
VGradient::addStop( const VColor &color, float rampPoint, float midPoint )
{
	VColorStop stop;
	stop.color = color;
	// Clamping between 0.0 and 1.0
	rampPoint = rampPoint < 0.0 ? 0.0 : rampPoint;
	rampPoint = rampPoint > 1.0 ? 1.0 : rampPoint;
	stop.rampPoint = rampPoint;
	// Clamping between 0.0 and 1.0
	midPoint = midPoint < 0.0 ? 0.0 : midPoint;
	midPoint = midPoint > 1.0 ? 1.0 : midPoint;
	stop.midPoint = midPoint;
	m_colorStops.append( stop );
}

void
VGradient::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "GRADIENT" );

	me.setAttribute( "originX", m_origin.x() );
	me.setAttribute( "originY", m_origin.y() );
	me.setAttribute( "vectorX", m_vector.x() );
	me.setAttribute( "vectorY", m_vector.y() );
	me.setAttribute( "type", m_type );
	me.setAttribute( "repeatMethod", m_repeatMethod );

	// save stops
	QValueListConstIterator<VColorStop> itr;
	for( itr = m_colorStops.begin(); itr != m_colorStops.end(); ++itr )
	{
		QDomElement stop = element.ownerDocument().createElement( "COLORSTOP" );
		(*itr).color.save( stop );
		stop.setAttribute( "ramppoint", (*itr).rampPoint );
		stop.setAttribute( "midpoint", (*itr).midPoint );
		me.appendChild( stop );
	}

	element.appendChild( me );
}

void
VGradient::load( const QDomElement& element )
{
	m_origin.setX( element.attribute( "originX", "0.0" ).toDouble() );
	m_origin.setY( element.attribute( "originY", "0.0" ).toDouble() );
	m_vector.setX( element.attribute( "vectorX", "0.0" ).toDouble() );
	m_vector.setY( element.attribute( "vectorY", "0.0" ).toDouble() );
	m_type = (VGradientType)element.attribute( "type", 0 ).toInt();
	m_repeatMethod = (VGradientRepeatMethod)element.attribute( "repeatMethod", 0 ).toInt();

	m_colorStops.clear();

	// load stops
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement colorstop = list.item( i ).toElement();

			if( colorstop.tagName() == "COLORSTOP" )
			{
				VColorStop stop;
				stop.color.load( colorstop.firstChild().toElement() );
				stop.rampPoint = colorstop.attribute( "ramppoint", "0.0" ).toDouble();
				stop.midPoint = colorstop.attribute( "midpoint", "0.5" ).toDouble();
				m_colorStops.append( stop );
			}
		}
	}
}

void
VGradient::transform( const QWMatrix &m )
{
	m_origin = m_origin.transform( m );	
	m_vector = m_vector.transform( m );	
}


