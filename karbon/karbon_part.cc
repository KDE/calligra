/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qpainter.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vcommand.h"

#include <kdebug.h>

// TODO: remove these after debugging:
#include "vpath.h"
#include <qwmatrix.h>
#include "vccmd_ellipse.h"
#include "vccmd_star.h"
#include "vccmd_rectangle.h"

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
	QObject* parent, const char* name, bool singleViewMode )
	: KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
	m_commandHistory = new VCommandHistory( this );

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
// TODO: remove the whole test code:

	VCCmdRectangle w(this, 100, 100, 200, 150 );
	VPath *spi = w.createPath();

	insertObject(spi);


	VCCmdEllipse e( this, 100, 100, 200, 200 );
	VPath* elly = e.createPath();

	VCCmdEllipse r( this, 120, 140, 140, 160 );
	VPath* rect = r.createPath();

	elly->combine( *rect );
	delete( rect );

	VCCmdEllipse s( this, 140, 120, 160, 140 );
	rect = s.createPath();

	elly->combine( *rect );
	delete( rect );

	VCCmdEllipse t( this, 160, 140, 180, 160 );
	rect = t.createPath();

	elly->combine( *rect );
	delete( rect );

	VCCmdEllipse u( this, 140, 160, 160, 180 );
	rect = u.createPath();

	elly->combine( *rect );
	delete( rect );

	insertObject( elly );
//	insertObject( rect );


/*
	VPath* obj = elly->booleanOp( rect, 0 );

	if ( obj )
	{
		QWMatrix m;
		m.translate( 250, 200 );
		obj->transform( m );
		insertObject( obj );
	}

	obj = rect->booleanOp( elly, 0 );

	if ( obj )
	{
		QWMatrix m;
		m.translate( 250, 0 );
		obj->transform( m );
		insertObject( obj );
	}
*/

	return true;
}

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
	return new KarbonView( this, parent, name );
}

bool
KarbonPart::loadXML( QIODevice*, const QDomDocument& )
{
	// TODO load the document from the QDomDocument
	return true;
}

QDomDocument
KarbonPart::saveXML()
{
	QDomDocument doc( "DOC" );

	doc.appendChild(
		doc.createProcessingInstruction(
			"xml",
			"version=\"0.1\" encoding=\"UTF-8\"") );

	QDomElement app = doc.createElement( "DOC" );
	app.setAttribute( "editor", "karbon14 0.0.1" );
	app.setAttribute( "mime", "application/x-karbon" );
	app.setAttribute( "version", "0.1" );
	doc.appendChild( app );

	// save layers:
	QPtrListIterator<VLayer> itr( m_layers );
	for ( ; itr.current(); ++itr )
	{
		itr.current()->save( app );
	}

	return doc;
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

	object.setState( VObject::selected );
	m_selection.append( &object );
}

void
KarbonPart::deselectObject( VObject& object )
{
	object.setState( VObject::normal );
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
			if( itr2.current()->state() != VObject::deleted )
			{
				itr2.current()->setState( VObject::selected );
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
			itr2.current()->setState( VObject::selected );
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
		itr.current()->setState( VObject::normal );
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
