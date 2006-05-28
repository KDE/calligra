/* This file is part of the KDE project
   Copyright (C) 2002 - 2005, The Karbon Developers

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
#include <qbuffer.h>
#include <QMatrix>
#include <Q3PtrList>

#include "vdocument.h"
#include "vglobal.h"
#include "vgradient.h"

#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>

#include <kdebug.h>

int VGradient::VColorStopList::compareItems( Q3PtrCollection::Item item1, Q3PtrCollection::Item item2 )
{
	float r1 = ( (VColorStop*)item1 )->rampPoint;
	float r2 = ( (VColorStop*)item2 )->rampPoint;

	return ( r1 == r2 ? 0 : r1 < r2 ? -1 : 1 );
} // VGradient::VColorStopList::compareItems

VGradient::VGradient( VGradientType type )
	: m_type( type )
{
	m_colorStops.setAutoDelete( true );

	// set up dummy gradient
	VColor color;

	color.set( 1.0, 0.0, 0.0 );
	addStop( color, 0.0, 0.5 );

	color.set( 1.0, 1.0, 0.0 );
	addStop( color, 1.0, 0.5 );

	setOrigin( QPointF( 0, 0 ) );
	setVector( QPointF( 0, 50 ) );
	setRepeatMethod( VGradient::reflect );
}

VGradient::VGradient( const VGradient& gradient )
{
	m_colorStops.setAutoDelete( true );

	m_origin	= gradient.m_origin;
	m_focalPoint	= gradient.m_focalPoint;
	m_vector	= gradient.m_vector;
	m_type		= gradient.m_type;
	m_repeatMethod	= gradient.m_repeatMethod;

	m_colorStops.clear();
	Q3PtrVector<VColorStop> cs = gradient.colorStops();
	for( uint i = 0; i < cs.count(); i++ )
		m_colorStops.append( new VColorStop( *cs[i] ) );
	m_colorStops.sort();
} // VGradient::VGradient

VGradient& VGradient::operator=( const VGradient& gradient )
{
	m_colorStops.setAutoDelete( true );

	if ( this == &gradient )
		return *this;

	m_origin	= gradient.m_origin;
	m_focalPoint	= gradient.m_focalPoint;
	m_vector	= gradient.m_vector;
	m_type		= gradient.m_type;
	m_repeatMethod	= gradient.m_repeatMethod;

	m_colorStops.clear();
	Q3PtrVector<VColorStop> cs = gradient.colorStops();
	for( uint i = 0; i < cs.count(); i++ )
		m_colorStops.append( new VColorStop( *cs[i] ) );
	m_colorStops.sort();

	return *this;
} // VGradient::operator=

const Q3PtrVector<VColorStop> VGradient::colorStops() const
{ 
	Q3PtrVector<VColorStop> v;
	m_colorStops.toVector( &v );
	v.setAutoDelete( false );
	return v;
} // VGradient::colorStops()

void
VGradient::clearStops()
{
	m_colorStops.clear();
}

void
VGradient::addStop( const VColorStop& colorStop )
{
	m_colorStops.inSort( new VColorStop( colorStop ) );
} // VGradient::addStop

void
VGradient::addStop( const VColor &color, float rampPoint, float midPoint )
{
	// Clamping between 0.0 and 1.0
	rampPoint = qMax( 0.0f, rampPoint );
	rampPoint = qMin( 1.0f, rampPoint );
	// Clamping between 0.0 and 1.0
	midPoint = qMax( 0.0f, midPoint );
	midPoint = qMin( 1.0f, midPoint );

	// Work around stops with the same position
	VColorStop *v;
	for(v = m_colorStops.first(); v; v = m_colorStops.next())
	{
		if(rampPoint == v->rampPoint)
			rampPoint += 0.001f;
	} 
	m_colorStops.inSort( new VColorStop( rampPoint, midPoint, color ) );
}

void VGradient::removeStop( const VColorStop& colorstop )
{
	m_colorStops.remove( &colorstop );
} // VGradient::removeStop

void
VGradient::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "GRADIENT" );

	me.setAttribute( "originX", m_origin.x() );
	me.setAttribute( "originY", m_origin.y() );
	me.setAttribute( "focalX", m_focalPoint.x() );
	me.setAttribute( "focalY", m_focalPoint.y() );
	me.setAttribute( "vectorX", m_vector.x() );
	me.setAttribute( "vectorY", m_vector.y() );
	me.setAttribute( "type", m_type );
	me.setAttribute( "repeatMethod", m_repeatMethod );

	// save stops
	VColorStop* colorstop;
	Q3PtrList<VColorStop>& colorStops = const_cast<VColorStopList&>( m_colorStops );
	for( colorstop = colorStops.first(); colorstop; colorstop = colorStops.next() )
	{
		QDomElement stop = element.ownerDocument().createElement( "COLORSTOP" );
		colorstop->color.save( stop );
		stop.setAttribute( "ramppoint", colorstop->rampPoint );
		stop.setAttribute( "midpoint", colorstop->midPoint );
		me.appendChild( stop );
	}

	element.appendChild( me );
}

QString
VGradient::saveOasis( KoGenStyles &mainStyles ) const
{
	bool radial = m_type == VGradient::radial;
	KoGenStyle gradientStyle( radial ? VDocument::STYLE_RADIAL_GRADIENT : VDocument::STYLE_LINEAR_GRADIENT /*no family name*/);
	if( radial )
	{
		gradientStyle.addAttribute( "draw:style", "radial" );
		gradientStyle.addAttributePt( "svg:cx", m_origin.x() );
		gradientStyle.addAttributePt( "svg:cy", m_origin.y() );
		double dx = m_vector.x() - m_origin.x();
		double dy = m_vector.y() - m_origin.y();
		gradientStyle.addAttributePt( "svg:r",  sqrt( dx * dx + dy * dy ) );
		gradientStyle.addAttributePt( "svg:fx", m_focalPoint.x() );
		gradientStyle.addAttributePt( "svg:fy", m_focalPoint.y() );
	}
	else
	{
		gradientStyle.addAttribute( "draw:style", "linear" );
		gradientStyle.addAttributePt( "svg:x1", m_origin.x() );
		gradientStyle.addAttributePt( "svg:y1", m_origin.y() );
		gradientStyle.addAttributePt( "svg:x2", m_vector.x() );
		gradientStyle.addAttributePt( "svg:y2", m_vector.y() );
	}
	if( m_repeatMethod == VGradient::repeat )
		gradientStyle.addAttribute( "svg:spreadMethod", "repeat" );
	else if( m_repeatMethod == VGradient::reflect )
		gradientStyle.addAttribute( "svg:spreadMethod", "reflect" );
	else
		gradientStyle.addAttribute( "svg:spreadMethod", "pad" );
	QBuffer buffer;
	buffer.open( QIODevice::WriteOnly );
	KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level

	// save stops
	VColorStop* colorstop;
	Q3PtrList<VColorStop>& colorStops = const_cast<VColorStopList&>( m_colorStops );
	for( colorstop = colorStops.first(); colorstop; colorstop = colorStops.next() )
	{
		elementWriter.startElement( "svg:stop" );
		elementWriter.addAttribute( "svg:offset", QString( "%1" ).arg( colorstop->rampPoint ) );
		elementWriter.addAttribute( "svg:color", QColor( colorstop->color ).name() );
		if( colorstop->color.opacity() < 1 )
			elementWriter.addAttribute( "svg:stop-opacity", QString( "%1" ).arg( colorstop->color.opacity() ) );
		elementWriter.endElement();
	}

	QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
	gradientStyle.addChildElement( "svg:stop", elementContents );
	return mainStyles.lookup( gradientStyle, "gradient" );
}

void
VGradient::loadOasis( const QDomElement &object, KoStyleStack &/*stack*/, VObject* parent )
{
	kDebug(38000) << "namespaceURI: " << object.namespaceURI() << endl;
	kDebug(38000) << "localName: " << object.localName() << endl;

	QRectF bb;
	
	if( parent )
		bb =  parent->boundingBox();

	if( object.namespaceURI() == KoXmlNS::draw && object.localName() == "gradient" )
	{
		m_repeatMethod = VGradient::reflect;
		QString strType = object.attributeNS( KoXmlNS::draw, "style", QString::null );
		if( strType == "radial" )
		{
			m_type = VGradient::radial;
			// TODO : find out whether Oasis works with boundingBox only?
			double cx = KoUnit::parseValue( object.attributeNS( KoXmlNS::draw, "cx", QString::null ).remove("%") );
			m_origin.setX( bb.bottomLeft().x() + bb.width() * 0.01 * cx );
			double cy = KoUnit::parseValue( object.attributeNS( KoXmlNS::draw, "cy", QString::null ).remove("%") );
			m_origin.setY(  bb.bottomLeft().y() - bb.height() * 0.01 * cy );
			m_focalPoint = m_origin;
			m_vector = bb.topRight();
		}
		else if( strType == "linear" )
		{
			m_type = VGradient::linear;
			double angle = 90 + object.attributeNS( KoXmlNS::draw, "angle", "0" ).toDouble();
			double radius = 0.5 * sqrt( bb.width()*bb.width() + bb.height()*bb.height() );
			double sx = cos( angle * VGlobal::pi / 180 ) * radius;
			double sy = sin( angle * VGlobal::pi / 180 ) * radius;
			m_origin.setX( bb.center().x() + sx );
			m_origin.setY( bb.center().y() + sy );
			m_vector.setX( bb.center().x() - sx );
			m_vector.setY( bb.center().y() - sy );
			m_focalPoint = m_origin;
		}
		else return;

		VColor startColor( QColor( object.attributeNS( KoXmlNS::draw, "start-color", QString::null ) ) );
		VColor endColor( QColor( object.attributeNS( KoXmlNS::draw, "end-color", QString::null ) ) );

		double startOpacity = 0.01 * object.attributeNS( KoXmlNS::draw, "start-intensity", "100" ).remove("%").toDouble();
		double endOpacity = 0.01 * object.attributeNS( KoXmlNS::draw, "end-intensity", "100" ).remove("%").toDouble();

		startColor.setOpacity( startOpacity );
		endColor.setOpacity( endOpacity );
		m_colorStops.clear();
		addStop( startColor, 0.0, 0.5 );
		addStop( endColor, 1.0, 0.0 );
		m_colorStops.sort();
		
	}
	else if( object.namespaceURI() == KoXmlNS::svg )
	{
		if( object.localName() == "linearGradient" )
		{
			m_type = VGradient::linear;
			m_origin.setX( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "x1", QString::null ) ) );
			m_origin.setY( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "y1", QString::null ) ) );
			m_vector.setX( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "x2", QString::null ) ) );
			m_vector.setY( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "y2", QString::null ) ) );
			m_focalPoint = m_origin;
		}
		else if( object.localName() == "radialGradient" )
		{
			m_type = VGradient::radial;
			m_origin.setX( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "cx", QString::null ) ) );
			m_origin.setY( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "cy", QString::null ) ) );
			double r = KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "r", QString::null ) );
			m_vector.setX( m_origin.x() + r );
			m_vector.setY( m_origin.y() );
			m_focalPoint.setX( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "fx", QString::null ) ) );
			m_focalPoint.setY( KoUnit::parseValue( object.attributeNS( KoXmlNS::svg, "fy", QString::null ) ) );
		}

		QString strSpread( object.attributeNS( KoXmlNS::svg, "spreadMethod", "pad" ) );
		if( strSpread == "repeat" )
			m_repeatMethod = VGradient::repeat;
		else if( strSpread == "reflect" )
			m_repeatMethod = VGradient::reflect;
		else
			m_repeatMethod = VGradient::none;
	
		m_colorStops.clear();
	
		// load stops
		QDomNodeList list = object.childNodes();
		for( int i = 0; i < list.count(); ++i )
		{
			if( list.item( i ).isElement() )
			{
				QDomElement colorstop = list.item( i ).toElement();
	
				if( colorstop.namespaceURI() == KoXmlNS::svg && colorstop.localName() == "stop" )
				{
					VColor color( QColor( colorstop.attributeNS( KoXmlNS::svg, "color", QString::null ) ) );
					color.setOpacity( colorstop.attributeNS( KoXmlNS::svg, "stop-opacity", "1.0" ).toDouble() );
					addStop( color, colorstop.attributeNS( KoXmlNS::svg, "offset", "0.0" ).toDouble(), 0.5 );
				}
			}
		}
		m_colorStops.sort();
	}
}

void
VGradient::load( const QDomElement& element )
{
	m_origin.setX( element.attribute( "originX", "0.0" ).toDouble() );
	m_origin.setY( element.attribute( "originY", "0.0" ).toDouble() );
	m_focalPoint.setX( element.attribute( "focalX", "0.0" ).toDouble() );
	m_focalPoint.setY( element.attribute( "focalY", "0.0" ).toDouble() );
	m_vector.setX( element.attribute( "vectorX", "0.0" ).toDouble() );
	m_vector.setY( element.attribute( "vectorY", "0.0" ).toDouble() );
	m_type = (VGradientType)element.attribute( "type", 0 ).toInt();
	m_repeatMethod = (VGradientRepeatMethod)element.attribute( "repeatMethod", 0 ).toInt();

	m_colorStops.clear();

	// load stops
	QDomNodeList list = element.childNodes();
	for( int i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement colorstop = list.item( i ).toElement();

			if( colorstop.tagName() == "COLORSTOP" )
			{
				VColor color;
				color.load( colorstop.firstChild().toElement() );
				addStop( color, colorstop.attribute( "ramppoint", "0.0" ).toDouble(), colorstop.attribute( "midpoint", "0.5" ).toDouble() );
			}
		}
	}
	m_colorStops.sort();
}

void
VGradient::transform( const QMatrix &m )
{
	double x, y;

	m.map(m_origin.x(), m_origin.y(), &x, &y);
	m_origin	= QPointF(x,y);

	m.map(m_focalPoint.x(), m_focalPoint.y(), &x, &y);
	m_focalPoint	= QPointF(x,y);

	m.map(m_vector.x(), m_vector.y(), &x, &y);
	m_vector	= QPointF(x,y);
}
