/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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

#include <qdom.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qbrush.h>

#include <kdebug.h>

#include "vgradient.h"
#include "vkopainter.h"
#include "vfill.h"

int VGradient::VColorStopList::compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 )
{
	float r1 = ( (VColorStop*)item1 )->rampPoint;
	float r2 = ( (VColorStop*)item2 )->rampPoint;
	
	return ( r1 == r2 ? 0 : r1 < r2 ? -1 : 1 );
} // VGradient::VColorStopList::compareItems

VGradient::VGradient( VGradientType type )
	: m_type( type )
{
	m_colorStops.setAutoDelete( true );

	// set up dummy gradient
	VColor color;

	color.set( 1.0, 0.0, 0.0 );
	addStop( color, 0.0, 0.5 );

	color.set( 1.0, 1.0, 0.0 );
	addStop( color, 1.0, 0.5 );

	setOrigin( KoPoint( 0, 0 ) );
	setVector( KoPoint( 0, 50 ) );
	setRepeatMethod( VGradient::reflect );
}

VGradient::VGradient( const VGradient& gradient )
{
	m_colorStops.setAutoDelete( true );

	m_origin       = gradient.m_origin;
	m_vector       = gradient.m_vector;
	m_type         = gradient.m_type;
	m_repeatMethod = gradient.m_repeatMethod;

	m_colorStops.clear();
	QPtrVector<VColorStop> cs = gradient.colorStops();
	for( unsigned int i = 0; i < cs.count(); i++ )
		m_colorStops.append( new VColorStop( *cs[i] ) );
	m_colorStops.sort();
} // VGradient::VGradient

VGradient& VGradient::operator=( const VGradient& gradient )
{
	m_colorStops.setAutoDelete( true );

	if ( this == &gradient )
		return *this;
		
	m_origin       = gradient.m_origin;
	m_vector       = gradient.m_vector;
	m_type         = gradient.m_type;
	m_repeatMethod = gradient.m_repeatMethod;
	
	m_colorStops.clear();
	QPtrVector<VColorStop> cs = gradient.colorStops();
	for( unsigned int i = 0; i < cs.count(); i++ )
		m_colorStops.append( new VColorStop( *cs[i] ) );
	m_colorStops.sort();

	return *this;	
} // VGradient::operator=

const QPtrVector<VColorStop> VGradient::colorStops() const
{ 
	QPtrVector<VColorStop> v;
	m_colorStops.toVector( &v );
	v.setAutoDelete( false );
	return v; 
} // VGradient::colorStops()

void
VGradient::clearStops()
{
	m_colorStops.clear();
}

void
VGradient::addStop( const VColorStop& colorStop )
{
	 m_colorStops.inSort( new VColorStop( colorStop ) );
} // VGradient::addStop

void
VGradient::addStop( const VColor &color, float rampPoint, float midPoint )
{
	// Clamping between 0.0 and 1.0
	rampPoint = rampPoint < 0.0 ? 0.0 : rampPoint;
	rampPoint = rampPoint > 1.0 ? 1.0 : rampPoint;
	// Clamping between 0.0 and 1.0
	midPoint = midPoint < 0.0 ? 0.0 : midPoint;
	midPoint = midPoint > 1.0 ? 1.0 : midPoint;
	
	m_colorStops.inSort( new VColorStop( rampPoint, midPoint, color ) );
}

void VGradient::removeStop( const VColorStop& colorstop )
{
	m_colorStops.remove( &colorstop );
} // VGradient::removeStop

void
VGradient::save( QDomElement& element ) const
{
	QDomElement me = element.ownerDocument().createElement( "GRADIENT" );

	me.setAttribute( "originX", m_origin.x() );
	me.setAttribute( "originY", m_origin.y() );
	me.setAttribute( "vectorX", m_vector.x() );
	me.setAttribute( "vectorY", m_vector.y() );
	me.setAttribute( "type", m_type );
	me.setAttribute( "repeatMethod", m_repeatMethod );

	// save stops
	VColorStop* colorstop;
	QPtrList<VColorStop>& colorStops = const_cast<VColorStopList&>( m_colorStops );
	for( colorstop = colorStops.first(); colorstop; colorstop = colorStops.next() )
	{
		QDomElement stop = element.ownerDocument().createElement( "COLORSTOP" );
		colorstop->color.save( stop );
		stop.setAttribute( "ramppoint", colorstop->rampPoint );
		stop.setAttribute( "midpoint", colorstop->midPoint );
		me.appendChild( stop );
	}

	element.appendChild( me );
}

void
VGradient::load( const QDomElement& element )
{
	m_origin.setX( element.attribute( "originX", "0.0" ).toDouble() );
	m_origin.setY( element.attribute( "originY", "0.0" ).toDouble() );
	m_vector.setX( element.attribute( "vectorX", "0.0" ).toDouble() );
	m_vector.setY( element.attribute( "vectorY", "0.0" ).toDouble() );
	m_type = (VGradientType)element.attribute( "type", 0 ).toInt();
	m_repeatMethod = (VGradientRepeatMethod)element.attribute( "repeatMethod", 0 ).toInt();

	m_colorStops.clear();

	// load stops
	QDomNodeList list = element.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement colorstop = list.item( i ).toElement();

			if( colorstop.tagName() == "COLORSTOP" )
			{
				VColor color;
				color.load( colorstop.firstChild().toElement() );
				addStop( color, colorstop.attribute( "ramppoint", "0.0" ).toDouble(), colorstop.attribute( "midpoint", "0.5" ).toDouble() );
			}
		}
	}
	m_colorStops.sort();
}

void
VGradient::transform( const QWMatrix &m )
{
	m_origin = m_origin.transform( m );	
	m_vector = m_vector.transform( m );	
}

VGradientListItem::VGradientListItem( const VGradient& gradient, QString filename )
		: QListBoxItem( 0L ), m_filename( filename )
{
	m_gradient = new VGradient( gradient );

	m_pixmap.resize( 200, 16 );
	VKoPainter gp( &m_pixmap, m_pixmap.width(), m_pixmap.height() );
	gp.setRasterOp( Qt::XorROP );
	gp.newPath();
	VGradient grad( *m_gradient );
	grad.setOrigin( KoPoint( 0, 0 ) );
	grad.setVector( KoPoint( m_pixmap.width() - 1, 0 ) );
	grad.setType( VGradient::linear );
	VFill fill;
	fill.gradient() = grad;
	fill.setType( VFill::grad );
	gp.setBrush( fill );
	gp.moveTo( KoPoint( 0, 0 ) );
	gp.lineTo( KoPoint( 0, m_pixmap.height() - 1 ) );
	gp.lineTo( KoPoint( m_pixmap.width() - 1, m_pixmap.height() - 1 ) );
	gp.lineTo( KoPoint( m_pixmap.width() - 1, 0 ) );
	gp.lineTo( KoPoint( 0, 0 ) );
	gp.fillPath();
	gp.end();

	m_delete = QFileInfo( filename ).isWritable();
} // VGradientListItem::VGradientListItem

VGradientListItem::VGradientListItem( const VGradientListItem& gradient )
		: QListBoxItem( 0L )
{
	m_pixmap = gradient.m_pixmap;
	m_delete = gradient.m_delete;
	m_gradient = new VGradient( *gradient.gradient() );
	m_filename = gradient.m_filename;
} // VGradientListItem::VGradientListItem

VGradientListItem::~VGradientListItem()
{
	delete m_gradient;
} // VGradientListItem::~VGradientListItem

int VGradientListItem::width( const QListBox* lb ) const
{
	return lb->width() - 25;
} // VGradientListItem::width

void VGradientListItem::paint( QPainter* painter )
{
	painter->save();
	painter->setRasterOp( Qt::CopyROP );
	QRect r ( 0, 0, width( listBox() ), height( listBox() ) );
	painter->scale( ( (float)( width( listBox() ) ) ) / 200., 1. );
	painter->drawPixmap( 0, 0, m_pixmap );
	painter->restore();
	if ( isSelected() )
		painter->setPen( listBox()->colorGroup().highlightedText() );
	else
		painter->setPen( listBox()->colorGroup().base() );
	painter->drawRect( r );
	painter->flush();
} // VGradientListItem::paint
