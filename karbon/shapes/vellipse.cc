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


#include "vellipse.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <koUnit.h>
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <vglobal.h>
#include <vdocument.h>
#include <qdom.h>

VEllipse::VEllipse( VObject* parent, VState state ) : VPath( parent, state )
{
}

VEllipse::VEllipse( VObject* parent,
		const KoPoint& topLeft, double width, double height,
		VEllipseType type, double startAngle, double endAngle )
	: VPath( parent ), m_type( type ), m_startAngle( startAngle ), m_endAngle( endAngle )
{
	setDrawCenterNode();

	m_rx = width / 2.0;
	m_ry = height / 2.0;
	m_center.setX( topLeft.x() + m_rx );
	m_center.setY( topLeft.y() + m_ry );

	init();
}

void
VEllipse::init()
{
	// to radials
	int nsegs;
	if( m_startAngle < m_endAngle )
	    nsegs = floor( ( m_endAngle - m_startAngle ) / 90.0 );
	else
	    nsegs = 4 - ceil( ( m_startAngle - m_endAngle ) / 90.0 );
	double startAngle = m_startAngle - 90.0;
	if( startAngle < 0 ) startAngle += 360.0;
	startAngle = VGlobal::pi_2 * ( startAngle / 90.0 );
	double endAngle   = m_endAngle - 90.0;
	if( endAngle < 0 ) endAngle += 360.0;
	endAngle   = VGlobal::pi_2 * ( endAngle / 90.0 );
	// Create (half-)unity circle with topLeft at (0|0):
	double currentAngle = -startAngle - VGlobal::pi_2;
	KoPoint start( 0.5 * sin( -startAngle ), 0.5 * cos( -startAngle ) );
	moveTo( KoPoint( start.x(), start.y() ) );
	double midAngle = currentAngle + VGlobal::pi_2 / 2.0;
	double midAmount = 0.5 / sin( VGlobal::pi_2 / 2.0 );
	for( int i = 0;i < nsegs;i++ )
	{
		midAngle -= VGlobal::pi_2;
		arcTo( KoPoint( cos( midAngle ) * midAmount, -sin( midAngle ) * midAmount ),
						KoPoint( 0.5 * sin( currentAngle ), 0.5 * cos( currentAngle ) ), 0.5 );
		currentAngle -= VGlobal::pi_2;
	}
	double rest = ( -endAngle - VGlobal::pi_2 - currentAngle ) * 90.0 / VGlobal::pi_2;
	if( rest > 0 )
		rest -= 360.0;
	if( rest != 0 )
	{
		midAngle = currentAngle - ( -rest / 360.0 ) * VGlobal::pi;
		midAmount = 0.5 / cos( currentAngle - midAngle );
		KoPoint end( 0.5 * sin( -endAngle ), 0.5 * cos( -endAngle ) );
		arcTo( KoPoint( cos( midAngle ) * midAmount, -sin( midAngle ) * midAmount ),
				KoPoint( 0.5 * sin( -endAngle ), 0.5 * cos( -endAngle ) ), 0.5 );
	}
	if( m_type == cut )
		lineTo( KoPoint( 0.0, 0.0 ) );
	if( m_type != arc )
		close();

	// Translate and scale:
	QWMatrix m;
	m.translate( m_center.x(), m_center.y() );
	m.scale( 2.0 * m_rx, 2.0 * m_ry );

	VTransformCmd cmd( 0L, m );
	cmd.visit( *this );

	m_matrix.reset();
}

QString
VEllipse::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Ellipse" );
}

void
VEllipse::save( QDomElement& element ) const
{
	if( document()->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "ELLIPSE" );
		element.appendChild( me );

		VObject::save( me );

		me.setAttribute( "cx", m_center.x() );
		me.setAttribute( "cy", m_center.y() );

		me.setAttribute( "rx", m_rx );
		me.setAttribute( "ry", m_ry );

		me.setAttribute( "start-angle", m_startAngle );
		me.setAttribute( "end-angle", m_endAngle );

		if( m_type == cut )
			me.setAttribute( "kind", "cut" );
		else if( m_type == section )
			me.setAttribute( "kind", "section" );
		else if( m_type == arc )
			me.setAttribute( "kind", "arc" );
		else
			me.setAttribute( "kind", "full" );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VEllipse::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles ) const
{
	docWriter->startElement( "draw:ellipse" );

	//save all into pt
	docWriter->addAttributePt( "svg:cx", m_center.x() );
	docWriter->addAttributePt( "svg:cy", m_center.y() );
	docWriter->addAttributePt( "svg:rx", m_rx );
	docWriter->addAttributePt( "svg:ry", m_ry );

	if( m_type == full )
		docWriter->addAttribute( "draw:kind", "full" );
	else
	{
		if( m_type == cut )
			docWriter->addAttribute( "draw:kind", "cut" );
		else if( m_type == section )
			docWriter->addAttribute( "draw:kind", "section" );
		else
			docWriter->addAttribute( "draw:kind", "arc" );

		docWriter->addAttribute( "draw:start-angle", m_startAngle );
		docWriter->addAttribute( "draw:end-angle", m_endAngle );
	}

	VObject::saveOasis( store, docWriter, mainStyles );

	QString transform = buildSvgTransform();
	if( !transform.isEmpty() )
		docWriter->addAttribute( "draw:transform", transform );

	docWriter->endElement();
}

bool
VEllipse::loadOasis( const QDomElement &element, KoOasisContext &context )
{
	setState( normal );

	m_rx = KoUnit::parseValue( element.attribute( "svg:rx" ) );
	m_ry = KoUnit::parseValue( element.attribute( "svg:ry" ) );

	m_center.setX( KoUnit::parseValue( element.attribute( "svg:cx" ) ) );
	m_center.setY( KoUnit::parseValue( element.attribute( "svg:cy" ) ) );

	m_startAngle = element.attribute( "draw:start-angle" ).toDouble();
	m_endAngle = element.attribute( "draw:end-angle" ).toDouble();

	if( element.attribute( "draw:kind" ) == "cut" )
		m_type = cut;
	else if( element.attribute( "draw:kind" ) == "section" )
		m_type = section;
	else if( element.attribute( "draw:kind" ) == "arc" )
		m_type = arc;
	else
		m_type = full;

	init();

	QString trafo = element.attribute( "draw:transform" );
	if( !trafo.isEmpty() )
		transform( trafo );

	return VObject::loadOasis( element, context );
}

void
VEllipse::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
		if( list.item( i ).isElement() )
			VObject::load( list.item( i ).toElement() );

	m_rx = KoUnit::parseValue( element.attribute( "rx" ) );
	m_ry = KoUnit::parseValue( element.attribute( "ry" ) );

	m_center.setX( KoUnit::parseValue( element.attribute( "cx" ) ) );
	m_center.setY( KoUnit::parseValue( element.attribute( "cy" ) ) );

	m_startAngle = element.attribute( "start-angle" ).toDouble();
	m_endAngle = element.attribute( "end-angle" ).toDouble();

	if( element.attribute( "kind" ) == "cut" )
		m_type = cut;
	else if( element.attribute( "kind" ) == "section" )
		m_type = section;
	else if( element.attribute( "kind" ) == "arc" )
		m_type = arc;
	else
		m_type = full;

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

VPath* 
VEllipse::clone() const
{
	return new VEllipse( *this );
}
