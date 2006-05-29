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


#include <qdom.h>

#include "vglobal.h"
#include "vpolygon.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <KoUnit.h>
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <vdocument.h>

VPolygon::VPolygon( VObject* parent, VState state ) 
	: VPath( parent, state )
{
}

VPolygon::VPolygon( VObject* parent, const QString &points,
		const QPointF& topLeft, double width, double height )
	: VPath( parent ), m_topLeft( topLeft ), m_width( width), m_height( height ), m_points( points )
{
	init();
}

void
VPolygon::init()
{
	bool bFirst = true;

	QString points = m_points.simplified();
	points.replace( ',', ' ' );
	points.remove( '\r' );
	points.remove( '\n' );
	QStringList pointList = points.split( ' ' );
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
	close();

	QMatrix m;
	m.translate( m_topLeft.x(), m_topLeft.y() );

	// only tranform the path data
	VTransformCmd cmd( 0L, m );
	cmd.VVisitor::visitVPath( *this );
}

QString
VPolygon::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Polygon" );
}

void
VPolygon::save( QDomElement& element ) const
{
	VDocument *doc = document();
	if( doc && doc->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "POLYGON" );
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

		me.setAttribute( "points", m_points );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VPolygon::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const
{
	docWriter->startElement( "draw:polygon" );

	docWriter->addAttribute( "draw:points", m_points );

	VObject::saveOasis( store, docWriter, mainStyles, index );

	docWriter->endElement();
}

void
VPolygon::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( int i = 0; i < list.count(); ++i )
		if( list.item( i ).isElement() )
			VObject::load( list.item( i ).toElement() );

	m_points = element.attribute( "points" );

	m_width  = KoUnit::parseValue( element.attribute( "width" ), 10.0 );
	m_height = KoUnit::parseValue( element.attribute( "height" ), 10.0 );

	m_topLeft.setX( KoUnit::parseValue( element.attribute( "x" ) ) );
	m_topLeft.setY( KoUnit::parseValue( element.attribute( "y" ) ) );

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

bool
VPolygon::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
{
	setState( normal );

	m_points = element.attributeNS( KoXmlNS::draw, "points", QString::null );

	init();

	transformByViewbox( element, element.attributeNS( KoXmlNS::svg, "viewBox", QString::null ) );

	QString trafo = element.attributeNS( KoXmlNS::draw, "transform", QString::null );
	if( !trafo.isEmpty() )
		transformOasis( trafo );

	return VObject::loadOasis( element, context );
}

VPath* 
VPolygon::clone() const
{
	return new VPolygon( *this );
}
