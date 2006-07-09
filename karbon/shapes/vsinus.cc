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


#include <qwmatrix.h>

#include "vglobal.h"
#include "vsinus.h"
#include "vtransformcmd.h"
#include <klocale.h>
#include <KoUnit.h>
#include <qdom.h>
#include <vdocument.h>

VSinus::VSinus( VObject* parent, VState state )
	: VPath( parent, state )
{
}

VSinus::VSinus( VObject* parent,
		const KoPoint& topLeft, double width, double height, uint periods )
	: VPath( parent ), m_topLeft( topLeft ), m_width( width), m_height( height ), m_periods( periods )
{
	// We want at least 1 period:
	if( m_periods < 1 )
		m_periods = 1;
	init();
}

void
VSinus::init()
{
	KoPoint p1;
	KoPoint p2;
	KoPoint p3( 0.0, 0.0 );
	moveTo( p3 );

	for ( uint i = 0; i < m_periods; ++i )
	{
		p1.setX( i + 1.0/24.0 );
		p1.setY( ( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p2.setX( i + 1.0/12.0 );
		p2.setY( ( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0/8.0 );
		p3.setY( VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 1.0/6.0 );
		p1.setY( ( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 5.0/24.0 );
		p2.setY( 1.0 );
		p3.setX( i + 1.0/4.0 );
		p3.setY( 1.0 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 7.0/24.0 );
		p1.setY( 1.0 );
		p2.setX( i + 1.0/3.0 );
		p2.setY( ( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 3.0/8.0 );
		p3.setY( VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 5.0/12.0 );
		p1.setY( ( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 11.0/24.0 );
		p2.setY( ( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0/2.0 );
		p3.setY( 0.0 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 13.0/24.0 );
		p1.setY( -( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p2.setX( i + 7.0/12.0 );
		p2.setY( -( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 5.0/8.0 );
		p3.setY( -VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 2.0/3.0 );
		p1.setY( -( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 17.0/24.0 );
		p2.setY( -1.0 );
		p3.setX( i + 3.0/4.0 );
		p3.setY( -1.0 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 19.0/24.0 );
		p1.setY( -1.0 );
		p2.setX( i + 5.0/6.0 );
		p2.setY( -( 3.0 * VGlobal::sqrt2 + 2.0 ) * VGlobal::one_7 );
		p3.setX( i + 7.0/8.0 );
		p3.setY( -VGlobal::sqrt2 * 0.5 );
		curveTo( p1, p2, p3 );

		p1.setX( i + 11.0/12.0 );
		p1.setY( -( 4.0 * VGlobal::sqrt2 - 2.0 ) * VGlobal::one_7 );
		p2.setX( i + 23.0/24.0 );
		p2.setY( -( 2.0 * VGlobal::sqrt2 - 1.0 ) * VGlobal::one_7 );
		p3.setX( i + 1.0 );
		p3.setY( 0.0 );
		curveTo( p1, p2, p3 );
	}

	// Translate and scale:
	QWMatrix m;
	m.translate( m_topLeft.x(), m_topLeft.y() - m_height * 0.5 );
	m.scale( m_width / m_periods, m_height * 0.5 );

	// only tranform the path data
	VTransformCmd cmd( 0L, m );
	cmd.VVisitor::visitVPath( *this );

	m_matrix.reset();
}

QString
VSinus::name() const
{
	QString result = VObject::name();
	return !result.isEmpty() ? result : i18n( "Sinus" );
}

void
VSinus::save( QDomElement& element ) const
{
	VDocument *doc = document();
	if( doc && doc->saveAsPath() )
	{
		VPath::save( element );
		return;
	}

	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "SINUS" );
		element.appendChild( me );

		// save fill/stroke untransformed
		VPath path( *this );
		VTransformCmd cmd( 0L, m_matrix.invert() );
		cmd.visit( path );
		path.VObject::save( me );
		//VObject::save( me );

		me.setAttribute( "x", m_topLeft.x() );
		me.setAttribute( "y", m_topLeft.y() );

		me.setAttribute( "width", m_width );
		me.setAttribute( "height", m_height );

		me.setAttribute( "periods", m_periods );

		QString transform = buildSvgTransform();
		if( !transform.isEmpty() )
			me.setAttribute( "transform", transform );
	}
}

void
VSinus::load( const QDomElement& element )
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

	m_periods  = element.attribute( "periods" ).toUInt();

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

VPath* 
VSinus::clone() const
{
	return new VSinus( *this );
}

