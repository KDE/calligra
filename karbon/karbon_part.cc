/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qpainter.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vcommand.h"
#include "vpainterfactory.h"

#include <kdebug.h>


KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
	QObject* parent, const char* name, bool singleViewMode )
	: KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
	m_commandHistory = new VCommandHistory( this );

	m_layers.setAutoDelete( true );

	// create a layer. we need at least one:
	m_layers.append( new VLayer() );
	m_activeLayer = m_layers.getLast();
}

KarbonPart::~KarbonPart()
{
	// delete all layers:
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current() ; ++itr )
		delete( itr.current() );

	// delete the command-history:
	delete m_commandHistory;
}

bool
KarbonPart::initDoc()
{
	return true;
}

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
	return  new KarbonView( this, parent, name );
}

bool
KarbonPart::loadXML( QIODevice*, const QDomDocument& document )
{
	QDomElement doc = document.documentElement();

	if(
		doc.attribute( "mime" ) != "application/x-karbon" ||
		doc.attribute( "version" ) != "0.1" )
	{
		return false;
	}

	m_layers.clear();

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
				m_layers.append( layer );
			}
		}
	}
	m_activeLayer = m_layers.getLast();

	return true;
}

QDomDocument
KarbonPart::saveXML()
{
	QDomDocument document( "DOC" );

	document.appendChild(
		document.createProcessingInstruction(
			"xml",
			"version=\"0.1\" encoding=\"UTF-8\"") );

	QDomElement doc = document.createElement( "DOC" );
	doc.setAttribute( "editor", "karbon14 0.0.1" );
	doc.setAttribute( "mime", "application/x-karbon" );
	doc.setAttribute( "version", "0.1" );
	document.appendChild( doc );

	// save layers:
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current(); ++itr )
	{
		itr.current()->save( doc );
	}

	return document;
}

void
KarbonPart::insertObject( const VObject* object )
{
	// don't repaint here explicitely. some commands might want to insert many
	// objects.
	activeLayer()->insertObject( object );
	setModified( true );
}

void
KarbonPart::selectObject( VObject& object, bool exclusive )
{
	if( exclusive )
		deselectAllObjects();

	object.setState( state_selected );
	m_selection.append( &object );
}

void
KarbonPart::deselectObject( VObject& object )
{
	object.setState( state_normal );
	m_selection.removeRef( &object );
}

void
KarbonPart::selectAllObjects()
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
			if( itr2.current()->state() != state_deleted )
			{
				itr2.current()->setState( state_selected );
				m_selection.append( itr2.current() );
			}
		}
	}
}

void
KarbonPart::selectObjectsWithinRect( const QRect& rect,
	const double zoomFactor, bool exclusive )
{
	if( exclusive )
		deselectAllObjects();

	VObjectList objects;
	VLayerListIterator itr( m_layers );

	for ( ; itr.current(); ++itr )
	{
		objects = itr.current()->objectsWithinRect( rect, zoomFactor );
		VObjectListIterator itr2( objects );
		for ( ; itr2.current(); ++itr2 )
		{
			itr2.current()->setState( state_selected );
			m_selection.append( itr2.current() );
		}
	}
}

void
KarbonPart::deselectAllObjects()
{
	// deselect objects:
	VObjectListIterator itr( m_selection );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->setState( state_normal );
	}
	
	m_selection.clear();
}

void
KarbonPart::addCommand( VCommand* cmd, bool repaint  )
{
	m_commandHistory->addCommand( cmd );
	setModified( true );

	if( repaint )
		repaintAllViews();
}

void
KarbonPart::purgeHistory()
{
	// remove "deleted" objects from all layers:
	VLayerListIterator itr( m_layers );
	for ( ; itr.current() ; ++itr )
	{
		itr.current()->removeDeletedObjects();
	}

	// clear command history:
	m_commandHistory->clear();
}

void
KarbonPart::repaintAllViews( bool /*erase*/ )
{
	QPtrListIterator<KoView> itr( views() );
	for ( ; itr.current() ; ++itr )
	{
 		static_cast<KarbonView*>( itr.current() )->
			canvasWidget()->repaintAll( true );
	}
}


void
KarbonPart::paintContent( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
	kdDebug() << "**** part->paintContent()" << endl;
}

#include "karbon_part.moc"
