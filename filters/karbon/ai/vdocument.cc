/***************************************************************************
                          vdocument.cpp  -  description
                             -------------------
    begin                : Tue Apr 16 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdom.h>

#include "vdocument.h"

VDocument::VDocument()
 : m_mime ("application/x-karbon"), m_version ("0.1"),
   m_editor ("aiimport 0.0.1"), m_syntaxVersion ("0.1")
{
}

VDocument::~VDocument(){
	VLayerListIterator itr = m_layers;
	for ( ; itr.current(); ++itr )
	{
		delete( itr.current() );
	}
}

void VDocument::insertLayer( const VLayer* layer ){
  qDebug ("insert layer");
	m_layers.append( layer );
}

void VDocument::save( QDomDocument &doc ) const{


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

/*
void VDocument::load( const QDomElement& element ){
	m_layers.setAutoDelete( true );
	m_layers.clear();
	m_layers.setAutoDelete( false );

	m_mime = element.attribute( "mime" );
	m_version = element.attribute( "version" );
	m_editor = element.attribute( "editor" );
	m_syntaxVersion = element.attribute( "syntaxVersion" );

	QDomNodeList list = element.childNodes();
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

}
*/
