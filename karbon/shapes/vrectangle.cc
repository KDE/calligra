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


#include "vrectangle.h"
#include <klocale.h>
#include <KoUnit.h>
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <qdom.h>
#include <kdebug.h>
#include "vglobal.h"
#include <vdocument.h>
#include "vtransformcmd.h"

VRectangle::VRectangle( VObject* parent, VState state )
	: VPath( parent, state )
{
}

VRectangle::VRectangle( VObject* parent,
		const KoPoint& topLeft, double width, double height, double rx, double ry )
	: VPath( parent ), m_topLeft( topLeft ), m_width( width), m_height( height ), m_rx( rx ), m_ry( ry )
{
	setDrawCenterNode();

	if( m_rx < 0.0 ) m_rx = 0.0;
	if( m_ry < 0.0 ) m_ry = 0.0;
	// Catch case, when radius is larger than width or height:
	if( m_rx > m_width * 0.5 )
		m_rx = m_width * 0.5;
	if( m_ry > m_height * 0.5 )
		m_ry = m_height * 0.5;

	init();
}

void
VRectangle::init()
{
	if( m_rx == 0 && m_ry == 0 )
	{
		moveTo( m_topLeft );
		lineTo( KoPoint( m_topLeft.x(), m_topLeft.y() - m_height ) );
		lineTo( KoPoint( m_topLeft.x() + m_width, m_topLeft.y() - m_height ) );
		lineTo( KoPoint( m_topLeft.x() + m_width, m_topLeft.y() ) );
	}
	else
	{
		double rx = m_rx;
		double ry = m_ry;
		double x = m_topLeft.x();
		double y = m_topLeft.y();
		moveTo( KoPoint( x + rx, y ) );
		curveTo( KoPoint( x + rx * ( 1 - 0.552 ), y ),
				 KoPoint( x, y - ry * ( 1 - 0.552 ) ),
				 KoPoint( x, y - ry ) );
		if( ry < m_height / 2 )
		 	lineTo( KoPoint( x, y - m_height + ry ) );
		curveTo( KoPoint( x, y - m_height + ry * ( 1 - 0.552 ) ),
				 KoPoint( x + rx * ( 1 - 0.552 ), y - m_height ),
				 KoPoint( x + rx, y - m_height ) );
		if( rx < m_width / 2 )
		 	lineTo( KoPoint( x + m_width - rx, y - m_height ) );
		curveTo( KoPoint( x + m_width - rx * ( 1 - 0.552 ), y - m_height ),
				 KoPoint( x + m_width, y - m_height + ry * ( 1 - 0.552 ) ),
				 KoPoint( x + m_width, y - m_height + ry ) );
		if( ry < m_height / 2 )
			lineTo( KoPoint( x + m_width, y - ry ) );
		curveTo( KoPoint( x + m_width, y - ry * ( 1 - 0.552 ) ),
				 KoPoint( x + m_width - rx * ( 1 - 0.552 ), y ),
				 KoPoint( x + m_width - rx, y ) );
		if( rx < m_width / 2 )
		 	lineTo( KoPoint( x + rx, y ) );
	}
	close();
}

QString
VRectangle::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Rectangle" );
}

void
VRectangle::save( QDomElement& element ) const
{
	VDocument *doc = document();
	if( doc && doc->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "RECT" );
		element.appendChild( me );

		// save fill/stroke untransformed
		VPath path( *this );
		VTransformCmd cmd( 0L, m_matrix.invert() );
		cmd.visit( path );
		path.VObject::save( me );
		//VObject::save( me );

		me.setAttribute( "x", m_topLeft.x() );
		me.setAttribute( "y", m_topLeft.y() );

		me.setAttribute( "width", QString("%1pt").arg( m_width ) );
		me.setAttribute( "height", QString("%1pt").arg( m_height ) );

		me.setAttribute( "rx", m_rx );
		me.setAttribute( "ry", m_ry );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VRectangle::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	// do not save deleted objects
	if( state() == deleted )
		return;

	// different rx/ry is not supported by oasis, so act like it is a normal path
	if( m_rx != 0. && m_ry != 0. && m_rx != m_ry )
		return VPath::saveOasis( store, docWriter, mainStyles, index );

	docWriter->startElement( "draw:rect" );

	//save all into pt
	docWriter->addAttributePt( "svg:x",      m_topLeft.x() );
	docWriter->addAttributePt( "svg:y",      m_topLeft.y()-m_height );
	docWriter->addAttributePt( "svg:width",  m_width );
	docWriter->addAttributePt( "svg:height", m_height );

	if( m_rx != 0. && m_ry != 0. && m_rx == m_ry )
		docWriter->addAttributePt( "draw:corner-radius", m_rx );

	VObject::saveOasis( store, docWriter, mainStyles, index );

	QWMatrix tmpMat;
	tmpMat.scale( 1, -1 );
	tmpMat.translate( 0, -document()->height() );
	
	QString transform = buildOasisTransform( m_matrix*tmpMat );
	if( !transform.isEmpty() )
		docWriter->addAttribute( "draw:transform", transform );

	docWriter->endElement();
}

bool
VRectangle::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
{
	setState( normal );

	m_width  = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "width", QString::null ), 10.0 );
	m_height = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "height", QString::null ), 10.0 );

	m_topLeft.setX( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x", QString::null ) ) );
	m_topLeft.setY( m_height + KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y", QString::null ) ) );

	m_rx = m_ry = KoUnit::parseValue( element.attributeNS( KoXmlNS::draw, "corner-radius", QString::null ) );

	init();

	transformByViewbox( element, element.attributeNS( KoXmlNS::svg, "viewBox", QString::null ) );

	QString trafo = element.attributeNS( KoXmlNS::draw, "transform", QString::null );
	if( !trafo.isEmpty() )
		transformOasis( trafo );

	return VObject::loadOasis( element, context );
}

void
VRectangle::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
		if( list.item( i ).isElement() )
			VObject::load( list.item( i ).toElement() );

	m_width  = KoUnit::parseValue( element.attribute( "width" ), 10.0 );
	m_height = KoUnit::parseValue( element.attribute( "height" ), 10.0 );

	m_topLeft.setX( KoUnit::parseValue( element.attribute( "x" ) ) );
	m_topLeft.setY( KoUnit::parseValue( element.attribute( "y" ) ) );

	m_rx  = KoUnit::parseValue( element.attribute( "rx" ) );
	m_ry  = KoUnit::parseValue( element.attribute( "ry" ) );

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

VPath* 
VRectangle::clone() const
{
	return new VRectangle( *this );
}
