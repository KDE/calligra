/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
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
#include <qpaintdevicemetrics.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <koTemplateChooseDia.h>

#include "karbon_factory.h"
#include "karbon_part.h"
#include "karbon_part_iface.h"
#include "karbon_view.h"
#include "vcommand.h"
#include "vglobal.h"
#include "vpainter.h"
#include "vpainterfactory.h"
#include "vselection.h"
#include "vcanvas.h"
#include "vlayersdocker.h"


// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
						QObject* parent, const char* name, bool singleViewMode )
		: KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
		m_unit( KoUnit::U_MM )
{
	setInstance( KarbonFactory::instance(), false );
	m_bShowStatusBar = true;
	m_maxRecentFiles = VGlobal::maxRecentFiles;
	dcop = 0L;

	m_commandHistory = new VCommandHistory( this );
	connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
	connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

	initConfig();

	if( name )
		dcopObject();
}

KarbonPart::~KarbonPart()
{
	// delete the command-history:
	delete m_commandHistory;
	delete dcop;
}

DCOPObject* KarbonPart::dcopObject()
{
	if( !dcop )
		dcop = new KarbonPartIface( this );

	return dcop;
}


bool
KarbonPart::initDoc()
{
	QString file;
	KoTemplateChooseDia::ReturnType result;

	result = KoTemplateChooseDia::choose( KarbonFactory::instance(), file, "application/x-karbon",
										  "*.karbon", i18n( "Karbon14" ), KoTemplateChooseDia::Everything, "karbon_template" );
	m_doc.setWidth( KoUnit::ptFromUnit( PG_A4_WIDTH, KoUnit::U_MM ) );
	m_doc.setHeight( KoUnit::ptFromUnit( PG_A4_HEIGHT, KoUnit::U_MM ) );

	if( result == KoTemplateChooseDia::Template )
	{
		QFileInfo fileInfo( file );
		QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".karbon" );
		resetURL();
		bool ok = loadNativeFormat( fileName );
		initUnit();
		setEmpty();
		return ok;
	}
	else if( result == KoTemplateChooseDia::Empty )
	{
		initUnit();
		return true;
	}
	else if( result == KoTemplateChooseDia::File )
	{
		KURL url;
		url.setPath( file );
		bool ok = openURL( url );
		return ok;
	}

	return false;
}

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
	return new KarbonView( this, parent, name );
}

void
KarbonPart::removeView( KoView *view )
{
	kdDebug() << "KarbonPart::removeView" << endl;
	KoDocument::removeView( view );
}

bool
KarbonPart::loadXML( QIODevice*, const QDomDocument& document )
{
	bool success = false;

	QDomElement root = document.documentElement();

	success = m_doc.loadXML( root );

	m_pageLayout.ptWidth	= m_doc.width();
	m_pageLayout.ptHeight	= m_doc.height();

	setUnit( KoUnit::unit( m_doc.unitName() ) );

	return success;
}

QDomDocument
KarbonPart::saveXML()
{
	return m_doc.saveXML();
}

void
KarbonPart::insertObject( VObject* object )
{
	// don't repaint here explicitely. some commands might want to insert many
	// objects.
	m_doc.append( object );
	setModified( true );
}

void
KarbonPart::addCommand( VCommand* cmd, bool repaint )
{
	m_commandHistory->addCommand( cmd );
	setModified( true );

	if( repaint )
		repaintAllViews();
}

void
KarbonPart::slotDocumentRestored()
{
	setModified( false );
}

void
KarbonPart::slotCommandExecuted()
{
	setModified( true );
}

void
KarbonPart::clearHistory()
{
	m_commandHistory->clear();
}

void
KarbonPart::repaintAllViews( bool repaint )
{
	QPtrListIterator<KoView> itr( views() );

	for( ; itr.current() ; ++itr )
		static_cast<KarbonView*>( itr.current() )->canvasWidget()->repaintAll( repaint );
}

void
KarbonPart::repaintAllViews( const KoRect &rect )
{
	QPtrListIterator<KoView> itr( views() );

	for( ; itr.current() ; ++itr )
		static_cast<KarbonView*>( itr.current() )->canvasWidget()->repaintAll( rect );
}

void
KarbonPart::paintContent( QPainter& painter, const QRect& rect,
						  bool /*transparent*/, double zoomX, double /*zoomY*/ )
{
	kdDebug() << "**** part->paintContent()" << endl;
	painter.eraseRect( rect );
	VPainterFactory *painterFactory = new VPainterFactory;
	//QPaintDeviceMetrics metrics( painter.device() );
	painterFactory->setPainter( painter.device(), rect.width(), rect.height() );
	VPainter *p = painterFactory->painter();
	//VPainter *p = new VKoPainter( painter.device() );
	p->begin();
	p->setZoomFactor( zoomX );
	//kdDebug() << "painter.worldMatrix().dx() : " << painter.worldMatrix().dx() << endl;
	//kdDebug() << "rect.x() : "<< rect.x() << endl;
	//kdDebug() << "rect.y() : "<< rect.y() << endl;
	//p->setWorldMatrix( painter.worldMatrix() );
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( painter.worldMatrix().dx(), ( painter.worldMatrix().dy() - rect.height() ) );
	p->setWorldMatrix( mat );

	m_doc.selection()->clear();
	QPtrListIterator<VLayer> itr( m_doc.layers() );

	for( ; itr.current(); ++itr )
	{
		KoRect r = KoRect::fromQRect( rect );
		itr.current()->draw( p, &r );
	}

	p->end();
	delete painterFactory;
}

void
KarbonPart::setShowStatusBar( bool b )
{
	m_bShowStatusBar = b;
}

void
KarbonPart::reorganizeGUI()
{
	QPtrListIterator<KoView> itr( views() );

	for( ; itr.current(); ++itr )
	{
		static_cast<KarbonView*>( itr.current() )->reorganizeGUI();
	}
}

void
KarbonPart::setUndoRedoLimit( int undos )
{
	m_commandHistory->setUndoLimit( undos );
	m_commandHistory->setRedoLimit( undos );
}

void
KarbonPart::initConfig()
{
	KConfig* config = KarbonPart::instance()->config();

	if( config->hasGroup( "Interface" ) )
	{
		config->setGroup( "Interface" );
		setAutoSave( config->readNumEntry( "AutoSave", defaultAutoSave() / 60 ) * 60 );
		m_maxRecentFiles = config->readNumEntry( "NbRecentFile", VGlobal::maxRecentFiles );
		setShowStatusBar( config->readBoolEntry( "ShowStatusBar" , true ) );
		setBackupFile( config->readNumEntry( "BackupFile", true ) );
	}
        int undos = 30;
	if( config->hasGroup( "Misc" ) )
	{
            config->setGroup( "Misc" );
            undos = config->readNumEntry( "UndoRedo", -1 );
	}
        if( undos != -1 )
            setUndoRedoLimit( undos );

}

void
KarbonPart::initUnit()
{
	//load unit config after we load file.
	//load it for new file or empty file
	KConfig* config = KarbonPart::instance()->config();

	if( config->hasGroup( "Misc" ) )
	{
		config->setGroup( "Misc" );
		m_unit = KoUnit::unit( config->readEntry( "Units", KoUnit::unitName( KoUnit::U_MM ) ) );
	}
}

void
KarbonPart::setUnit( KoUnit::Unit _unit )

{
	m_unit = _unit;
	QPtrListIterator<KoView> itr( views() );

	for( ; itr.current(); ++itr )
	{
		static_cast<KarbonView*>( itr.current() )->setUnit( _unit );
	}
}

#include "karbon_part.moc"

