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
#include "vspiral.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <KoUnit.h>
#include <vdocument.h>

VSpiral::VSpiral( VObject* parent, VState state )
	: VPath( parent, state )
{
}

VSpiral::VSpiral( VObject* parent,
		const KoPoint& center, double radius, uint segments, double fade,
		bool clockwise, double angle, VSpiralType type )
	: VPath( parent ), m_center( center), m_radius( radius ), m_fade( fade ), m_segments( segments ), m_clockwise( clockwise ), m_angle( angle ), m_type( type )
{
	init();
}

void
VSpiral::init()
{
	// It makes sense to have at least one segment:
	if( m_segments < 1 )
		m_segments = 1;

	// Make sure the radius is positive:
	if( m_radius < 0.0 )
		m_radius = -m_radius;

	// Fall back, when fade is out of range:
	if( m_fade <= 0.0 || m_fade >= 1.0 )
		m_fade = 0.5;

	setFillRule( winding );

	// advance by pi/2 clockwise or cclockwise?
	double adv_ang = ( m_clockwise ? -1.0 : 1.0 ) * VGlobal::pi_2;
	// radius of first segment is non-faded radius:
	double r = m_radius;

	KoPoint oldP( 0.0, ( m_clockwise ? -1.0 : 1.0 ) * m_radius );
	KoPoint newP;
	KoPoint newCenter( 0.0, 0.0 );
	moveTo( oldP );

	for ( uint i = 0; i < m_segments; ++i )
	{
		newP.setX( r * cos( adv_ang * ( i + 2 ) ) + newCenter.x() );
		newP.setY( r * sin( adv_ang * ( i + 2 ) ) + newCenter.y() );

		if( m_type == round )
			arcTo( oldP + newP - newCenter, newP, r );
		else
			lineTo( newP );

		newCenter += ( newP - newCenter ) * ( 1.0 - m_fade );
		oldP = newP;
		r *= m_fade;
	}

	// translate path to center:
	QWMatrix m;
	m.translate( m_center.x(), m_center.y() );

	// sadly it's not feasible to simply add angle while creation.
	m.rotate(
		( m_angle + ( m_clockwise ? VGlobal::pi : 0.0 ) ) * // make cw-spiral start at mouse-pointer
			VGlobal::one_pi_180 );	// one_pi_180 = 1/(pi/180) = 180/pi.

	// only tranform the path data
	VTransformCmd cmd( 0L, m );
	cmd.VVisitor::visitVPath( *this );

	m_matrix.reset();
}

QString
VSpiral::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Spiral" );
}

void
VSpiral::save( QDomElement& element ) const
{
	VDocument *doc = document();
	if( doc && doc->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "SPIRAL" );
		element.appendChild( me );

		// save fill/stroke untransformed
		VPath path( *this );
		VTransformCmd cmd( 0L, m_matrix.invert() );
		cmd.visit( path );
		path.VObject::save( me );
		//VObject::save( me );

		me.setAttribute( "cx", m_center.x() );
		me.setAttribute( "cy", m_center.y() );

		me.setAttribute( "radius", m_radius );
		me.setAttribute( "angle", m_angle );
		me.setAttribute( "fade", m_fade );

		me.setAttribute( "segments", m_segments );

		me.setAttribute( "clockwise", m_clockwise );

		me.setAttribute( "type", m_type );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VSpiral::load( const QDomElement& element )
{
	setState( normal );

	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
		if( list.item( i ).isElement() )
			VObject::load( list.item( i ).toElement() );

	m_radius  = KoUnit::parseValue( element.attribute( "radius" ) );
	m_angle = element.attribute( "angle" ).toDouble();
	m_fade = element.attribute( "fade" ).toDouble();

	m_center.setX( KoUnit::parseValue( element.attribute( "cx" ) ) );
	m_center.setY( KoUnit::parseValue( element.attribute( "cy" ) ) );

	m_segments  = element.attribute( "segments" ).toUInt(),

	m_clockwise = element.attribute( "clockwise" ).toInt();

	m_type = (VSpiral::VSpiralType)element.attribute( "type" ).toInt();

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

VPath* 
VSpiral::clone() const
{
	return new VSpiral( *this );
}
