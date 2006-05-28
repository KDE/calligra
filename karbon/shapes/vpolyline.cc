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


#include "vpolyline.h"
#include <qdom.h>
#include <QPointF>

#include "vglobal.h"
#include <klocale.h>
#include <vdocument.h>
#include "vtransformcmd.h"

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>

VPolyline::VPolyline( VObject* parent, VState state )
	: VPath( parent, state )
{
}

/*VPolyline::VPolyline( VObject* parent, VState state ) 
	: VPath( parent, state )
{
}*/

/*VPolyline::VPolyline( VObject* parent, const QString &points ) 
	: VPath( parent ), m_points( points )
{
	init();
}*/

void
VPolyline::init()
{
	bool bFirst = true;

	QString points = m_points.simplified();
	points.replace( ',', ' ' );
	points.remove( '\r' );
	points.remove( '\n' );
	QStringList pointList = QStringList::split( ' ', points );
	QStringList::Iterator end(pointList.end());
	for( QStringList::Iterator it = pointList.begin(); it != end; ++it )
	{
		QPointF point;
		point.setX( (*it).toDouble() );
		point.setY( (*++it).toDouble() );
		if( bFirst )
		{
			moveTo( point );
			bFirst = false;
		}
		else
			lineTo( point );
	}
}

QString
VPolyline::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Polyline" );
}

void
VPolyline::save( QDomElement& element ) const
{
	VDocument *doc = document();
	if( doc && doc->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "POLYLINE" );
		element.appendChild( me );

		// save fill/stroke untransformed
		VPath path( *this );
		VTransformCmd cmd( 0L, m_matrix.invert() );
		cmd.visit( path );
		path.VObject::save( me );
		//VObject::save( me );

		me.setAttribute( "points", m_points );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VPolyline::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	docWriter->startElement( "draw:polyline" );

	docWriter->addAttribute( "svg:points", m_points );

	VObject::saveOasis( store, docWriter, mainStyles, index );

	docWriter->endElement();
}

void
VPolyline::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
		if( list.item( i ).isElement() )
			VObject::load( list.item( i ).toElement() );

	m_points = element.attribute( "points" );

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

bool
VPolyline::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
{
	setState( normal );

	if( element.localName() == "line" )
	{
		QPointF p1, p2;
		p1.setX( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x1", QString::null ) ) );
		p1.setY( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y1", QString::null ) ) );
		p2.setX( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x2", QString::null ) ) );
		p2.setY( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y2", QString::null ) ) );
		
		m_points = QString( "%1,%2 %3,%4" ).arg( p1.x() ).arg( p1.y() ).arg( p2.x() ).arg( p2.y() );

		moveTo( p1 );
		lineTo( p2 );
	}
	else if( element.localName() == "polyline" )
	{
		m_points = element.attributeNS( KoXmlNS::draw, "points", QString::null );
		init();
	}

	transformByViewbox( element, element.attributeNS( KoXmlNS::svg, "viewBox", QString::null ) );

	QString trafo = element.attributeNS( KoXmlNS::draw, "transform", QString::null );
	if( !trafo.isEmpty() )
		transformOasis( trafo );

	return VObject::loadOasis( element, context );
}

VPath* 
VPolyline::clone() const
{
	return new VPolyline( *this );
}
