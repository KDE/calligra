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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "vellipse.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <koUnit.h>
#include <vglobal.h>
#include <vdocument.h>
#include <qdom.h>

VEllipse::VEllipse( VObject* parent, VState state ) : VComposite( parent, state )
{
}

VEllipse::VEllipse( VObject* parent,
		const KoPoint& topLeft, double width, double height,
		VEllipseType type, double startAngle, double endAngle )
	: VComposite( parent ), m_type( type ), m_startAngle( startAngle ), m_endAngle( endAngle )
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
	    nsegs = 4 - floor( ( m_startAngle - m_endAngle ) / 90.0 );
	double startAngle = VGlobal::pi_2 * ( m_startAngle / 90.0 );
	double endAngle   = VGlobal::pi_2 * ( m_endAngle / 90.0 );
	// Create (half-)unity circle with topLeft at (0|0):
	double currentAngle = -startAngle - VGlobal::pi_2;
	KoPoint start( 0.5 * sin( -startAngle ), 0.5 * cos( -startAngle ) );
	moveTo( KoPoint( start.x(), start.y() ) );
	double midAngle = currentAngle + VGlobal::pi_2 / 2.0;
	for( int i = 0;i < nsegs;i++ )
	{
		KoPoint current = KoPoint( 0.5 * sin( currentAngle ), 0.5 * cos( currentAngle ) );
		currentAngle -= VGlobal::pi_2;
		midAngle -= VGlobal::pi_2;
		arcTo( KoPoint( cos( midAngle ) * ( 0.5 / sin( VGlobal::pi_2 / 2.0 ) ) ,
						-sin( midAngle ) * ( 0.5 / sin( VGlobal::pi_2 / 2.0 ) ) ) , current, 0.5 );
	}
	double rest = ( -endAngle - VGlobal::pi_2 - currentAngle ) * 90.0 / VGlobal::pi_2;
	if( rest > 0 )
		rest -= 360.0;
	if( rest != 0 )
	{
		midAngle = currentAngle - ( -rest / 360.0 ) * VGlobal::pi;
		KoPoint end( 0.5 * sin( -endAngle ), 0.5 * cos( -endAngle ) );
		arcTo( KoPoint( cos( midAngle ) * ( 0.5 / cos( currentAngle - midAngle ) ),
					-sin( midAngle ) * ( 0.5 / cos( currentAngle - midAngle ) ) ), end, 0.5 );
	}
	if( m_type == cut )
		lineTo( KoPoint( 0.0, 0.0 ) );
	if( m_type != arc )
		close();

	// Translate and scale:
	QWMatrix m;
	m.translate( m_center.x() - m_rx, m_center.y() - m_ry );
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
		VComposite::save( element );
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

		writeTransform( me );
	}
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

