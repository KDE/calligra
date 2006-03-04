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


#include <math.h>

#include <qwmatrix.h>
#include <qdom.h>

#include "vglobal.h"
#include "vstar.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <KoUnit.h>
#include <vdocument.h>

VStar::VStar( VObject* parent, VState state )
	: VPath( parent, state )
{
}

VStar::VStar( VObject* parent,
		const KoPoint& center, double outerRadius, double innerRadius,
		uint edges, double angle, uint innerAngle, double roundness, VStarType type )
	: VPath( parent ), m_center( center), m_outerRadius( outerRadius ), m_innerRadius( innerRadius), m_edges( edges ), m_angle( angle ), m_innerAngle( innerAngle ), m_roundness( roundness ), m_type( type )
{
	init();
}

void
VStar::init()
{
	double angle = m_angle;

	// A star should have at least 3 edges:
	if( m_edges < 3 )
		m_edges = 3;

	// Make sure, radii are positive:
	if( m_outerRadius < 0.0 )
		m_outerRadius = -m_outerRadius;

	if( m_innerRadius < 0.0 )
		m_innerRadius = -m_innerRadius;

	// trick for spoke, wheel (libart bug?)
	if( m_type == spoke || m_type == wheel && m_roundness == 0.0 )
		m_roundness = 0.01;

	// We start at angle + VGlobal::pi_2:
	KoPoint p2, p3;
	KoPoint p(
		m_outerRadius * cos( angle + VGlobal::pi_2 ),
		m_outerRadius * sin( angle + VGlobal::pi_2 ) );
	moveTo( p );

	double inAngle = VGlobal::twopi / 360 * m_innerAngle;

	if( m_type == star )
	{
		int j = ( m_edges % 2 == 0 ) ? ( m_edges - 2 ) / 2 : ( m_edges - 1 ) / 2;
		//innerRadius = getOptimalInnerRadius( outerRadius, edges, innerAngle );
		int jumpto = 0;
		bool discontinueous = ( m_edges % 4 == 2 );

		double outerRoundness = ( VGlobal::twopi * m_outerRadius * m_roundness ) / m_edges;
		double nextOuterAngle;

		for ( uint i = 1; i < m_edges + 1; ++i )
		{
			double nextInnerAngle = angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( jumpto + 0.5 );
			p.setX( m_innerRadius * cos( nextInnerAngle ) );
			p.setY( m_innerRadius * sin( nextInnerAngle ) );
			if( m_roundness == 0.0 )
				lineTo( p );
			else
			{
				nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * jumpto;
				p2.setX( m_outerRadius * cos( nextOuterAngle ) -
					cos( angle + VGlobal::twopi / m_edges * jumpto ) * outerRoundness );
				p2.setY( m_outerRadius * sin( nextOuterAngle ) -
					sin( angle + VGlobal::twopi / m_edges * jumpto ) * outerRoundness );

				curveTo( p2, p, p );
			}

			jumpto = ( i * j ) % m_edges;
			nextInnerAngle = angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( jumpto - 0.5 );
			p.setX( m_innerRadius * cos( nextInnerAngle ) );
			p.setY( m_innerRadius * sin( nextInnerAngle ) );
			lineTo( p );

			nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * jumpto;
			p.setX( m_outerRadius * cos( nextOuterAngle ) );
			p.setY( m_outerRadius * sin( nextOuterAngle ) );

			if( m_roundness == 0.0 )
				lineTo( p );
			else
			{
				p2.setX( m_innerRadius * cos( nextInnerAngle ) );
				p2.setY( m_innerRadius * sin( nextInnerAngle ) );

				p3.setX( m_outerRadius * cos( nextOuterAngle ) +
					cos( angle + VGlobal::twopi / m_edges * jumpto ) * outerRoundness );
				p3.setY( m_outerRadius * sin( nextOuterAngle ) +
					sin( angle + VGlobal::twopi / m_edges * jumpto ) * outerRoundness );

				curveTo( p2, p3, p );
			}
			if( discontinueous && i == ( m_edges / 2 ) )
			{
				angle += VGlobal::pi;
				nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * jumpto;
				p.setX( m_outerRadius * cos( nextOuterAngle ) );
				p.setY( m_outerRadius * sin( nextOuterAngle ) );
				moveTo( p );
			}
		}
	}
	else
	{
		if( m_type == wheel || m_type == spoke )
			m_innerRadius = 0.0;

		double innerRoundness = ( VGlobal::twopi * m_innerRadius * m_roundness ) / m_edges;
		double outerRoundness = ( VGlobal::twopi * m_outerRadius * m_roundness ) / m_edges;

		for ( uint i = 0; i < m_edges; ++i )
		{
			double nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 );
			double nextInnerAngle = angle + inAngle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 0.5 );
			if( m_type != polygon )
			{
				p.setX( m_innerRadius * cos( nextInnerAngle ) );
				p.setY( m_innerRadius * sin( nextInnerAngle ) );

				if( m_roundness == 0.0 )
					lineTo( p );
				else
				{
					p2.setX( m_outerRadius *
						cos( angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i ) ) -
						cos( angle + VGlobal::twopi / m_edges * ( i ) ) * outerRoundness );
					p2.setY( m_outerRadius *
						sin( angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i ) ) -
						sin( angle + VGlobal::twopi / m_edges * ( i ) ) * outerRoundness );

					p3.setX( m_innerRadius * cos( nextInnerAngle ) +
						cos( angle + inAngle + VGlobal::twopi / m_edges * ( i + 0.5 ) ) * innerRoundness );
					p3.setY( m_innerRadius * sin( nextInnerAngle ) +
						sin( angle + inAngle + VGlobal::twopi / m_edges * ( i + 0.5 ) ) * innerRoundness );

					if( m_type == gear )
					{
						lineTo( p2 );
						lineTo( p3 );
						lineTo( p );
					}
					else
						curveTo( p2, p3, p );
				}
			}

			p.setX( m_outerRadius * cos( nextOuterAngle ) );
			p.setY( m_outerRadius * sin( nextOuterAngle ) );

			if( m_roundness == 0.0 )
				lineTo( p );
			else
			{
				p2.setX( m_innerRadius * cos( nextInnerAngle ) -
					cos( angle + inAngle + VGlobal::twopi / m_edges * ( i + 0.5 ) ) * innerRoundness );
				p2.setY( m_innerRadius * sin( nextInnerAngle ) -
					sin( angle + inAngle + VGlobal::twopi / m_edges * ( i + 0.5 ) ) * innerRoundness );

				p3.setX( m_outerRadius * cos( nextOuterAngle ) +
					cos( angle + VGlobal::twopi / m_edges * ( i + 1.0 ) ) * outerRoundness );
				p3.setY( m_outerRadius * sin( nextOuterAngle ) +
					sin( angle + VGlobal::twopi / m_edges * ( i + 1.0 ) ) * outerRoundness );

				if( m_type == gear )
				{
					lineTo( p2 );
					lineTo( p3 );
					lineTo( p );
				}
				else
					curveTo( p2, p3, p );
			}
		}
	}
	if( m_type == wheel || m_type == framed_star )
	{
		close();
		for ( int i = m_edges - 1; i >= 0; --i )
		{
			double nextOuterAngle = angle + VGlobal::pi_2 + VGlobal::twopi / m_edges * ( i + 1.0 );
			p.setX( m_outerRadius * cos( nextOuterAngle ) );
			p.setY( m_outerRadius * sin( nextOuterAngle ) );
			lineTo( p );
		}
	}
	close();

	// translate path to center:
	QWMatrix m;
	m.translate( m_center.x(), m_center.y() );

	// only tranform the path data
	VTransformCmd cmd( 0L, m );
	cmd.VVisitor::visitVPath( *this );

	setFillRule( evenOdd );

	m_matrix.reset();
}

double
VStar::getOptimalInnerRadius( uint edges, double outerRadius, uint /*innerAngle*/ )
{
	int j = (edges % 2 == 0 ) ? ( edges - 2 ) / 2 : ( edges - 1 ) / 2;

	// get two well chosen lines of the star
	KoPoint p1( outerRadius * cos( VGlobal::pi_2 ), outerRadius * sin( VGlobal::pi_2 ) );
	int jumpto = ( j ) % edges;
	double nextOuterAngle = VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
	KoPoint p2( outerRadius * cos( nextOuterAngle ), outerRadius * sin( nextOuterAngle ) );

	nextOuterAngle = VGlobal::pi_2 + VGlobal::twopi / edges;
	KoPoint p3( outerRadius * cos( nextOuterAngle ),
				outerRadius * sin( nextOuterAngle ) );
	jumpto = ( edges - j + 1 ) % edges;
	nextOuterAngle = VGlobal::pi_2 + VGlobal::twopi / edges * jumpto;
	KoPoint p4( outerRadius * cos( nextOuterAngle ), outerRadius * sin( nextOuterAngle ) );

	// calc (x, y) -> intersection point
	double b1 = ( p2.y() - p1.y() ) / ( p2.x() - p1.x() );
	double b2 = ( p4.y() - p3.y() ) / ( p4.x() - p3.x() );
	double a1 = p1.y() - b1 * p1.x();
	double a2 = p3.y() - b2 * p3.x();
	double x = -( a1 - a2 ) / ( b1 - b2 );
	double y = a1 + b1 * x;
	// calc inner radius from intersection point and center
	return sqrt( x * x + y * y );
}

QString
VStar::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Star" );
}

void
VStar::save( QDomElement& element ) const
{
	VDocument *doc = document();
	if( doc && doc->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "STAR" );
		element.appendChild( me );

		// save fill/stroke untransformed
		VPath path( *this );
		VTransformCmd cmd( 0L, m_matrix.invert() );
		cmd.visit( path );
		path.VObject::save( me );
		//VObject::save( me );

		me.setAttribute( "cx", m_center.x() );
		me.setAttribute( "cy", m_center.y() );

		me.setAttribute( "outerradius", m_outerRadius );
		me.setAttribute( "innerradius", m_innerRadius );
		me.setAttribute( "edges", m_edges );

		me.setAttribute( "angle", m_angle );
		me.setAttribute( "innerangle", m_innerAngle );

		me.setAttribute( "roundness", m_roundness );

		me.setAttribute( "type", m_type );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VStar::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
		if( list.item( i ).isElement() )
			VObject::load( list.item( i ).toElement() );

	m_center.setX( KoUnit::parseValue( element.attribute( "cx" ) ) );
	m_center.setY( KoUnit::parseValue( element.attribute( "cy" ) ) );

	m_outerRadius  = KoUnit::parseValue( element.attribute( "outerradius" ) );
	m_innerRadius  = KoUnit::parseValue( element.attribute( "innerradius" ) );
	m_edges  = element.attribute( "edges" ).toUInt();

	m_innerAngle  = element.attribute( "innerangle" ).toUInt();
	m_angle = element.attribute( "angle" ).toDouble();

	m_roundness  = element.attribute( "roundness" ).toDouble();

	m_type =(VStar::VStarType) element.attribute( "type" ).toInt();

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

VPath* 
VStar::clone() const
{
	return new VStar( *this );
}
