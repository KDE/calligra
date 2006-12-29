/* This file is part of the KDE project
   Copyright (C) 2001-2005 The Karbon Developers
   Copyright (C) 2005-2006 Jan Hambecht <jaham@gmx.net>

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
#include <Q3ValueList>
#include <QRectF>

#include "vdocument.h"
#include "vselection.h"
#include "vvisitor.h"
#include "vstroke.h"
#include "vdashpattern.h"
#include "vpainter.h"

#include <KoStore.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>
#include <KoShapeLayer.h>

#include <kdebug.h>

VDocument::VDocument()
: VObject( 0L )
, m_width(0.)
, m_height(0.)
, m_selectionMode( VDocument::ActiveLayer )
, m_unit( KoUnit::Millimeter )
, m_saveAsPath(true)
{
    m_selection = new VSelection( this );
    // create a layer. we need at least one:
    insertLayer( new KoShapeLayer() );
}

VDocument::VDocument( const VDocument& document )
	: VObject( document ), m_width(0), m_height(0)
{
	m_selection = new VSelection( this );
	m_layers = document.m_layers;
// TODO
}

VDocument::~VDocument()
{
	delete( m_selection );
	foreach( KoShapeLayer* shape, m_layers )
		delete shape;
}

void
VDocument::drawPage( VPainter *p, const KoPageLayout &pl, bool showPageMargins ) const
{
	p->setPen( QColor( "black" ) );
	p->setBrush( Qt::white );
	p->drawRect( 0, 0, m_width, m_height );

	p->setPen( Qt::NoPen );
	p->setBrush( Qt::black );
	p->drawRect( m_width, - 2, 2, m_height );
	p->drawRect( 0, - 2, m_width, 2 );
	//p->drawRect( 0, m_height - 1, m_width, 1 );
	// Draw page margins
	if( showPageMargins )
	{
		int ml = int( pl.ptLeft );
		int mt = int( pl.ptTop );
		int mr = int( pl.ptRight );
		int mb = int( pl.ptBottom );

		VStroke s( 0, 1 );
		s.setColor( QColor( "blue" ) );
		Q3ValueList<float> dashes;
		s.dashPattern().setArray( dashes << 5 << 5 );
		p->setPen( s );
		p->setBrush( Qt::NoBrush );
		p->drawRect(ml, mt, m_width-ml-mr, m_height-mt-mb);
	}
}

void
VDocument::draw( VPainter *painter, const QRectF* rect ) const
{
	/*
	Q3PtrListIterator<VLayer> itr = m_layers;

	for ( ; itr.current(); ++itr )
	{
		itr.current()->draw( painter, rect );
	}
	*/
}

void
VDocument::insertLayer( KoShapeLayer* layer )
{
    m_layers.append( layer );
} // VDocument::insertLayer

void
VDocument::removeLayer( KoShapeLayer* layer )
{
	m_layers.removeAt( m_layers.indexOf( layer ) );
	if ( m_layers.count() == 0 )
		m_layers.append( new KoShapeLayer() );
} // VDocument::removeLayer

bool VDocument::canRaiseLayer( KoShapeLayer* layer )
{
    int pos = m_layers.indexOf( layer );
    return (pos != int( m_layers.count() ) - 1 && pos >= 0 );
}

bool VDocument::canLowerLayer( KoShapeLayer* layer )
{
    int pos = m_layers.indexOf( layer );
    return (pos>0);
}

void
VDocument::raiseLayer( KoShapeLayer* layer )
{
	int pos = m_layers.indexOf( layer );
	if( pos != int( m_layers.count() ) - 1 && pos >= 0 )
		m_layers.move( pos, pos + 1 );
} // VDocument::raiseLayer

void
VDocument::lowerLayer( KoShapeLayer* layer )
{
	int pos = m_layers.indexOf( layer );
	if ( pos > 0 )
		m_layers.move( pos, pos - 1 );
} // VDocument::lowerLayer

int
VDocument::layerPos( KoShapeLayer* layer )
{
	return m_layers.indexOf( layer );
} // VDocument::layerPos

void
VDocument::add( KoShape* shape )
{
    if( ! m_objects.contains( shape ) )
        m_objects.append( shape );
}

void
VDocument::remove( KoShape* shape )
{
    m_objects.removeAt( m_objects.indexOf( shape ) );
}

QDomDocument
VDocument::saveXML() const
{
	QDomDocument doc;
	QDomElement me = doc.createElement( "DOC" );
	doc.appendChild( me );
	save( me );
	return doc;
 }

void
VDocument::saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles ) const
{
	docWriter->startElement( "draw:page" );
	docWriter->addAttribute( "draw:name", name());
	docWriter->addAttribute( "draw:id", "page1");
	docWriter->addAttribute( "draw:master-page-name", "Default");

	// save objects:
    /* TODO implement saving of layers
	int index = 0;
	foreach( KoShapeLayer* layer, m_layers )
		layer->saveOasis( store, docWriter, mainStyles, ++index );
    */
	docWriter->endElement(); // draw:page
}

void
VDocument::save( QDomElement& me ) const
{
	me.setAttribute( "mime", "application/x-karbon" ),
	me.setAttribute( "version", "0.1" );
	me.setAttribute( "editor", "Karbon14" );
	me.setAttribute( "syntaxVersion", "0.1" );
	if( m_width > 0. )
			me.setAttribute( "width", m_width );
	if( m_height > 0. )
			me.setAttribute( "height", m_height );
	me.setAttribute( "unit", KoUnit::unitName( m_unit ) );

	// save objects:
	/* TODO: porting to flake
	VLayerListIterator itr( m_layers );

	for ( ; itr.current(); ++itr )
			itr.current()->save( me );
	*/
}

VDocument*
VDocument::clone() const
{
	return new VDocument( *this );
}

void
VDocument::load( const QDomElement& doc )
{
	loadXML( doc );
}

bool
VDocument::loadXML( const QDomElement& doc )
{
    if( doc.attribute( "mime" ) != "application/x-karbon" ||
		doc.attribute( "syntaxVersion" ) != "0.1" )
	{
		return false;
	}

	qDeleteAll(m_layers);
	m_layers.clear();

	m_width  = doc.attribute( "width", "800.0" ).toDouble();
	m_height = doc.attribute( "height", "550.0" ).toDouble();

	m_unit = KoUnit::unit( doc.attribute( "unit", KoUnit::unitName( m_unit ) ) );

	loadDocumentContent( doc );

	return true;
}

void
VDocument::loadDocumentContent( const QDomElement& doc )
{
	QDomNodeList list = doc.childNodes();
	for( int i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "LAYER" )
			{
				KoShapeLayer* layer = new KoShapeLayer();
                // TODO implement layer loading
				//layer->load( e );
				insertLayer( layer );
			}
		}
	}
}

bool
VDocument::loadOasis( const QDomElement &element, KoOasisLoadingContext &context )
{
    // TODO implement loading layers
    //return m_layers.first()->loadOasis( element, context );
    return true;
}

void
VDocument::accept( VVisitor& visitor )
{
	visitor.visitVDocument( *this );
}

QString
VDocument::objectName( const KoShape *obj ) const
{
	QMap<const KoShape *, QString>::ConstIterator it = m_objectNames.find( obj );
	return it == m_objectNames.end() ? 0L : it.value();
}

void
VDocument::setObjectName( const KoShape *obj, const QString name )
{
	m_objectNames.insert( obj, name );
}

QRectF 
VDocument::boundingRect()
{
	QRectF bb( 0, 0, m_width, m_height );
	foreach( KoShape* layer, m_layers )
	{
		bb = bb.unite(  layer->boundingRect() );
	}

	return bb;
}

const QList<KoShape*>
VDocument::shapes() const
{
    return m_objects;
}
