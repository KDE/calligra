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


#include "vrectangle.h"
#include <klocale.h>
#include <qdom.h>

VRectangle::VRectangle( VObject* parent, VState state )
	: VComposite( parent, state )
{
}

VRectangle::VRectangle( VObject* parent,
		const KoPoint& topLeft, double width, double height )
	: VComposite( parent ), m_topLeft( topLeft ), m_width( width), m_height( height )
{
	setDrawCenterNode();

	init();
}

void
VRectangle::init()
{
	moveTo( m_topLeft );
	lineTo( KoPoint( m_topLeft.x(),         m_topLeft.y() - m_height ) );
	lineTo( KoPoint( m_topLeft.x() + m_width, m_topLeft.y() - m_height ) );
	lineTo( KoPoint( m_topLeft.x() + m_width, m_topLeft.y() ) );
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
	if( state() != deleted )
	{
		QDomElement me = element.ownerDocument().createElement( "RECT" );
		element.appendChild( me );

		VObject::save( me );

		me.setAttribute( "x", m_topLeft.x() );
		me.setAttribute( "y", m_topLeft.y() );

		me.setAttribute( "width", m_width );
		me.setAttribute( "height", m_height );

		writeTransform( me );
	}
}

void
VRectangle::load( const QDomElement& element )
{
	setState( normal );

	VObject::load( element );

	m_width  = element.attribute( "width" ).toDouble(),
	m_height = element.attribute( "height" ).toDouble(),

	m_topLeft.setX( element.attribute( "x" ).toDouble() );
	m_topLeft.setY( element.attribute( "y" ).toDouble() );

	init();

	QString trafo = element.attribute( "transform" );
	if( !trafo.isEmpty() )
		transform( trafo );
}

