/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <kdebug.h>

#include "vcommand.h"
#include "vpath.h"

#include "karbon_part.h"
#include "karbon_view.h"


// TODO: remove these after debugging:
#include <qwmatrix.h>
#include "vccmd_ellipse.h"
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
/*
	VCCmdEllipse e( this, 50, 100, 150, 250 );
	VPath* elly = e.createPath();
*/
/*
	VCCmdRectangle e( this, 50, 100, 100, 350 );
	VPath* elly = e.createPath();
	QWMatrix l;

	l.translate(-40,60);
	l.rotate(-45.0);
	elly->transform( l );
*/
/*
	VCCmdRectangle r( this, 80, 70, 250, 200 );
	VPath* rect = r.createPath();
*/

	VPath* elly = new VPath();
	elly->moveTo(200,100);
	elly->lineTo(100,200);

	VPath* rect = new VPath();
	rect->moveTo(200,200);
	rect->lineTo(100,100);


	insertObject( elly );
	insertObject( rect );

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

	// If nothing is loaded, do initialize here
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
	// TODO save the document into a QDomDocument
	return QDomDocument();
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
KarbonPart::addCommand( VCommand* cmd )
{
	kdDebug() << "KarbonPart::addCommand " << cmd->name() << endl;
	m_commandHistory->addCommand( cmd );
	setModified( true );
	repaintAllViews();
}

void
KarbonPart::repaintAllViews( bool erase )
{
	QPtrListIterator<KoView> itr( views() );
	for ( ; itr.current() ; ++itr )
// TODO: any better solution for this?
//		static_cast<KarbonView*> ( itr.current() )->canvasWidget()->repaintAll(
// erase );
 		static_cast<KarbonView*> ( itr.current() )->canvasWidget()->repaintAll( true
 );
}


void
KarbonPart::paintContent( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
	kdDebug() << "part->paintContent()" << endl;
}

#include "karbon_part.moc"
