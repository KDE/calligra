/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>

#include "vdocument.h"
#include "vshape.h"

#include <kdebug.h>


VDocument::VDocument() : VObject( 0L ),
		m_mime( "application/x-karbon" ), m_version( "0.1" ),
		m_editor( "karbon14 0.0.1" ), m_syntaxVersion( "0.1" )
{
	// create a layer. we need at least one:
	m_layers.setAutoDelete( true );
	m_layers.append( new VLayer() );
	m_activeLayer = m_layers.getLast();

	// set a default fill color of white
	float r = 1.0, g = 1.0, b = 1.0;
	m_defaultFillColor.setValues( &r, &g, &b, 0L );
}

VDocument::~VDocument()
{
}

void
VDocument::draw( VPainter *painter, const KoRect& rect )
{
	QPtrListIterator<VLayer> i = m_layers;
	for ( ; i.current(); ++i )
		if ( i.current()->visible() )
			i.current()->draw( painter, rect );
}

void
VDocument::insertLayer( VLayer* layer )
{
	qDebug ("insert layer");
	m_layers.append( layer );
	m_activeLayer = layer;
}

void
VDocument::appendObject( VShape* object )
{
	m_activeLayer->appendObject( object );
}

void
VDocument::saveXML( QDomDocument& doc ) const
{
	QDomElement me = doc.createElement( "DOC" );

	QDomAttr attr;
/*
  attr = doc.createAttribute ("mime");
  attr.setValue (m_mime);
  me.setAttributeNode (attr); */

	me.setAttribute( "mime", m_mime );

/*  attr = doc.createAttribute ("version");
  attr.setValue (m_version);
  me.setAttributeNode (attr); */

	me.setAttribute( "version", m_version );

/*  attr = doc.createAttribute ("editor");
  attr.setValue (m_editor);
  me.setAttributeNode (attr); */

	me.setAttribute( "editor", m_editor );

/*  attr = doc.createAttribute ("syntaxVersion");
  attr.setValue (m_syntaxVersion);
  me.setAttributeNode (attr); */

	me.setAttribute( "syntaxVersion", m_syntaxVersion );

	doc.appendChild( me );

	// save objects:
	VLayerListIterator itr = m_layers;
	for ( ; itr.current(); ++itr )
		itr.current()->save( me );
}

bool
VDocument::loadXML( const QDomElement& doc )
{
    if( doc.attribute( "mime" ) != "application/x-karbon" ||
		doc.attribute( "syntaxVersion" ) != "0.1" )
	{
		return false;
	}

	m_layers.clear();

	m_mime = doc.attribute( "mime" );
	m_version = doc.attribute( "version" );
	m_editor = doc.attribute( "editor" );
	m_syntaxVersion = doc.attribute( "syntaxVersion" );

	QDomNodeList list = doc.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "LAYER" )
			{
				VLayer* layer = new VLayer();
				layer->load( e );
				insertLayer( layer );
			}
		}
	}
	return true;
}

void
VDocument::selectObject( VShape& object, bool exclusive )
{
	if( exclusive )
		deselectAllObjects();

	object.setState( state_selected );
	m_selection.append( &object );
}

void
VDocument::deselectObject( VShape& object )
{
	object.setState( state_normal );
	m_selection.removeRef( &object );
}

void
VDocument::selectAllObjects()
{
	m_selection.clear();

	VObjectList objects;
	VLayerListIterator itr( m_layers );

	for ( ; itr.current(); ++itr )
	{
		objects = itr.current()->objects();
		VObjectListIterator itr2( objects );
		for ( ; itr2.current(); ++itr2 )
		{
			if( static_cast<VShape *>( itr2.current() )->state() != state_deleted )
			{
				static_cast<VShape *>( itr2.current() )->setState( state_selected );
				m_selection.append( itr2.current() );
			}
		}
	}
	m_selection.invalidateBoundingBox();
}

void
VDocument::selectObjectsWithinRect( const KoRect& rect, bool exclusive )
{
	if( exclusive )
		deselectAllObjects();

	VObjectList objects;
	VLayerListIterator itr( m_layers );

	for ( ; itr.current(); ++itr )
	{
		objects = itr.current()->objectsWithinRect( rect );
		VObjectListIterator itr2( objects );
		for ( ; itr2.current(); ++itr2 )
		{
			static_cast<VShape *>(itr2.current())->setState( state_selected );
			m_selection.append( itr2.current() );
		}
	}
	m_selection.invalidateBoundingBox();
}

void
VDocument::deselectAllObjects()
{
	// deselect objects:
	VObjectListIterator itr( m_selection );
	for ( ; itr.current() ; ++itr )
	{
		static_cast<VShape *>(itr.current())->setState( state_normal );
	}

	m_selection.clear();
}

void
VDocument::moveSelectionUp()
{
	//kdDebug() << "KarbonPart::moveSelectionUp" << endl;
	VObjectList selection = m_selection;

	VObjectList objects;

	VLayerListIterator litr( m_layers );
	while( !selection.isEmpty() )
	{
		kdDebug() << "!selection.isEmpty()" << endl;
		for ( ; litr.current(); ++litr )
		{
			VObjectList todo;
			VObjectListIterator itr( selection );
			for ( ; itr.current() ; ++itr )
			{
				objects = litr.current()->objects();
				VObjectListIterator itr2( objects );
				// find all selected VObjects that are in the current layer
				for ( ; itr2.current(); ++itr2 )
					if( itr2.current() == itr.current() )
					{
						todo.append( itr.current() );
						// remove from selection
						selection.removeRef( itr.current() );
					}
			}
			// we have found the affected vobjects in this vlayer
			VObjectListIterator itr3( todo );
			for ( ; itr3.current(); ++itr3 )
				litr.current()->moveObjectUp( itr3.current() );
		}
	}
}

void
VDocument::moveSelectionDown()
{
	//kdDebug() << "KarbonPart::moveSelectionDown" << endl;
	VObjectList selection = m_selection;

	VObjectList objects;

	VLayerListIterator litr( m_layers );
	while( !selection.isEmpty() )
	{
		//kdDebug() << "!selection.isEmpty()" << endl;
		for ( ; litr.current(); ++litr )
		{
			VObjectList todo;
			VObjectListIterator itr( selection );
			for ( ; itr.current() ; ++itr )
			{
				objects = litr.current()->objects();
				VObjectListIterator itr2( objects );
				// find all selected VObjects that are in the current layer
				for ( ; itr2.current(); ++itr2 )
					if( itr2.current() == itr.current() )
					{
						todo.append( itr.current() );
						// remove from selection
						selection.removeRef( itr.current() );
					}
			}
			// we have found the affected vobjects in this vlayer
			VObjectListIterator itr3( todo );
			for ( ; itr3.current(); ++itr3 )
				litr.current()->moveObjectDown( itr3.current() );
		}
	}
}

void
VDocument::moveSelectionToTop()
{
	VLayer *topLayer = m_layers.getLast();
	//
	VObjectListIterator itr( m_selection );
	for ( ; itr.current() ; ++itr )
	{
		// remove from old layer
		VObjectList objects;
		VLayerListIterator litr( m_layers );

		for ( ; litr.current(); ++litr )
		{
			objects = litr.current()->objects();
			VObjectListIterator itr2( objects );
			for ( ; itr2.current(); ++itr2 )
				if( itr2.current() == itr.current() )
				{
					litr.current()->removeRef( itr2.current() );
					// add to new top layer
					topLayer->appendObject( itr.current() );
					break;
				}
		}
	}

	m_activeLayer = topLayer;
}

void
VDocument::moveSelectionToBottom()
{
	VLayer *bottomLayer = m_layers.getFirst();
	//
	VObjectListIterator itr( m_selection );
	for ( ; itr.current() ; ++itr )
	{
		// remove from old layer
		VObjectList objects;
		VLayerListIterator litr( m_layers );

		for ( ; litr.current(); ++litr )
		{
			objects = litr.current()->objects();
			VObjectListIterator itr2( objects );
			for ( ; itr2.current(); ++itr2 )
				if( itr2.current() == itr.current() )
				{
					litr.current()->removeRef( itr2.current() );
					// add to new top layer
					bottomLayer->prependObject( itr.current() );
					break;
				}
		}
	}

	m_activeLayer = bottomLayer;
}

void
VDocument::applyDefaultColors( VShape& obj ) const
{
	VStroke stroke( obj.stroke() );
	VFill fill( obj.fill() );

	stroke.setColor( m_defaultStrokeColor );
	fill.setColor( m_defaultFillColor );

	obj.setStroke( stroke );
	obj.setFill( fill );
}

