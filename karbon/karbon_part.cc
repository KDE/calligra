/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <qpainter.h>
#include <kdebug.h>

#include "vcommand.h"
#include "vpath.h"
#include "vpoint.h"

#include "karbon_part.h"
#include "karbon_view.h"

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
	QListIterator<VLayer> i( m_layers );
	for ( ; i.current() ; ++i )
	{
		delete( i.current() );
	}

	// delete the command-history:
	delete m_commandHistory;
}

bool
KarbonPart::initDoc()
{
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
	activeLayer()->insertObject( object );
	setModified( true );
}

void
KarbonPart::addCommand( VCommand* cmd )
{
	kdDebug() << "KarbonPart::addCommand " << cmd->name() << endl;
	m_commandHistory->addCommand( cmd );
}

void
KarbonPart::paintContent( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
	kdDebug() << "part->paintContent()" << endl;
}

#include "karbon_part.moc"
