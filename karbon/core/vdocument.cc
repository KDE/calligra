/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include "vdocument.h"

#include <qdom.h>

VDocument::VDocument()
	: m_mime ("application/x-karbon"), m_version ("0.1"),
	  m_editor ("karbon14 0.0.1"), m_syntaxVersion ("0.1")
{
	// create a layer. we need at least one:
	m_layers.setAutoDelete( true );
	m_layers.append( new VLayer() );
}

VDocument::~VDocument()
{
}

void
VDocument::insertLayer( const VLayer* layer )
{
	qDebug ("insert layer");
	m_layers.append( layer );
}

void
VDocument::save( QDomDocument &doc ) const
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
VDocument::load( const QDomElement& doc )
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
