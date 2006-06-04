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

KarbonRectangle::KarbonRectangle() : KoRectangleShape()
{
}

// TODO: might still be usefull/needed
/*QString
VRectangle::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Rectangle" );
}*/

void KarbonRectangle::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
#if 0
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

	QMatrix tmpMat;
	tmpMat.scale( 1, -1 );
	tmpMat.translate( 0, -document()->height() );
	
	QString transform = buildOasisTransform( m_matrix*tmpMat );
	if( !transform.isEmpty() )
		docWriter->addAttribute( "draw:transform", transform );

	docWriter->endElement();
#endif
}

bool KarbonRectangle::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
{
#if 0
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
#endif
}

KoShape* KarbonRectangle::clone() const
{
    return new KarbonRectangle( *this );
}



// ------ old code ----------------------------------




/*
void
VRectangle::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( int i = 0; i < list.count(); ++i )
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
}*/

/*void
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
		VTransformCmd cmd( 0L, m_matrix.inverted() );
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
}*/
