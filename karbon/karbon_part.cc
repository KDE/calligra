/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
#include <ktempfile.h>
#include <koTemplateChooseDia.h>
#include <koStoreDevice.h>
#include <koOasisStyles.h>
#include <kooasiscontext.h>
#include <koxmlwriter.h>
#include <koOasisSettings.h>
#include "kovariable.h"

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
#include "vlayer.h"
#include "vdocumentdocker.h"
#include "vtoolcontroller.h"
#include "koApplication.h"

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
						QObject* parent, const char* name, bool singleViewMode )
		: KarbonPartBase( parentWidget, widgetName, parent, name, singleViewMode )
{
	m_toolController = new VToolController( const_cast<KarbonPart *>( this ) );
	m_toolController->init();
	setInstance( KarbonFactory::instance(), false );
	m_bShowStatusBar = true;
	dcop = 0L;

	m_commandHistory = new VCommandHistory( this );
	connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
	connect( m_commandHistory, SIGNAL( commandExecuted( VCommand * ) ), this, SLOT( slotCommandExecuted( VCommand * ) ) );

	initConfig();

	m_merge = false;

	m_maxRecentFiles = 10;

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

void
KarbonPart::setPageLayout( KoPageLayout& layout, KoUnit::Unit _unit )
{
	m_pageLayout = layout;
	m_doc.setUnit( _unit );
	m_doc.setWidth( m_pageLayout.ptWidth );
	m_doc.setHeight( m_pageLayout.ptHeight );
}

bool
KarbonPart::initDoc()
{
	QString file;
	KoTemplateChooseDia::ReturnType result;

        KoTemplateChooseDia::DialogType dlgtype;
        //if (initDocFlags() != KoDocument::InitDocFileNew)
            dlgtype = KoTemplateChooseDia::Everything;
      //  else
        //    dlgtype = KoTemplateChooseDia::OnlyTemplates;

	result = KoTemplateChooseDia::choose( KarbonFactory::instance(), file, "application/x-karbon",
										  "*.karbon", i18n( "Karbon14" ), dlgtype, "karbon_template" );
	// set as default A4 paper
	m_pageLayout.ptWidth	= KoUnit::fromUserValue( PG_A4_WIDTH, KoUnit::U_MM );
	m_pageLayout.ptHeight	= KoUnit::fromUserValue( PG_A4_HEIGHT, KoUnit::U_MM );
	m_pageLayout.format		= PG_DIN_A4;
	m_doc.setWidth( m_pageLayout.ptWidth );
	m_doc.setHeight( m_pageLayout.ptHeight );

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
		KURL url( file );
		return openURL( url );
	}

	return false;
}

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
	KarbonView *result = new KarbonView( this, parent, name );
	toolController()->setActiveView( result );
	return result;
}

void
KarbonPart::removeView( KoView *view )
{
	kdDebug(38000) << "KarbonPart::removeView" << endl;
	KoDocument::removeView( view );
}

bool
KarbonPart::loadXML( QIODevice*, const QDomDocument& document )
{
	bool success = false;

	QDomElement doc = document.documentElement();

	if( m_merge )
	{
		m_doc.loadDocumentContent( doc );
		return true;
	}

	success = m_doc.loadXML( doc );

	m_pageLayout.ptWidth	= m_doc.width();
	m_pageLayout.ptHeight	= m_doc.height();

	setUnit( m_doc.unit() );

	return success;
}

QDomDocument
KarbonPart::saveXML()
{
	return m_doc.saveXML();
}

bool
KarbonPart::loadOasis( const QDomDocument &doc, KoOasisStyles &styles, const QDomDocument &settings, KoStore *store )
{
	kdDebug(38000) << "Start loading OASIS document..." << endl;

	QDomElement contents = doc.documentElement();
	QDomElement body( contents.namedItem( "office:body" ).toElement() );
	if( body.isNull() )
	{
		kdDebug(38000) << "No office:body found!" << endl;
		setErrorMessage( i18n( "Invalid OASIS document. No office:body tag found." ) );
		return false;
	}

	body = body.namedItem("office:drawing").toElement();
	if(body.isNull())
	{
		kdDebug(38000) << "No office:drawing found!" << endl;
		setErrorMessage( i18n( "Invalid OASIS document. No office:drawing tag found." ) );
		return false;
	}

	QDomElement page( body.namedItem( "draw:page" ).toElement() );
	if(page.isNull())
	{
		kdDebug(38000) << "No office:drawing found!" << endl;
		setErrorMessage( i18n( "Invalid OASIS document. No draw:page tag found." ) );
		return false;
	}

	QString masterPageName = "Standard"; // use default layout as fallback
	QDomElement *master = styles.masterPages()[ masterPageName ];
	Q_ASSERT( master );
	QDomElement *style =master ? styles.styles()[master->attribute( "style:page-layout-name" )] : 0;
	if( style )
	{
        	m_pageLayout.loadOasis( *style );
		m_doc.setWidth( m_pageLayout.ptWidth );
		m_doc.setHeight( m_pageLayout.ptHeight );
	}
	else
		return false;

	KoVariableFormatCollection varFormatColl;
	KoVariableCollection varColl( new KoVariableSettings(), &varFormatColl );
	KoOasisContext context( this, varColl, styles, store );
	m_doc.loadOasis( page, context );
        loadOasisSettings( settings );
	return true;
}

void
KarbonPart::loadOasisSettings( const QDomDocument&settingsDoc )
{
    if ( settingsDoc.isNull() )
        return ; //not a error some file doesn't have settings.xml
    KoOasisSettings settings( settingsDoc );
    bool tmp = settings.selectItemSet( "view-settings" );
    //kdDebug()<<" settings : view-settings :"<<tmp<<endl;

    if ( tmp )
    {
        tmp = settings.selectItemMap( "Views" );
        setUnit(KoUnit::unit(settings.parseConfigItemString("unit")));
        //todo add other config here.
    }
}


bool
KarbonPart::saveOasis( KoStore *store, KoXmlWriter *manifestWriter )
{
    if( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice storeDev( store );
    KoXmlWriter docWriter( &storeDev, "office:document-content" );
    KoGenStyles mainStyles;

    KoGenStyle pageLayout = m_pageLayout.saveOasis();
    QString layoutName = mainStyles.lookup( pageLayout, "PL" );
    KoGenStyle masterPage( KoGenStyle::STYLE_MASTER );
    masterPage.addAttribute( "style:page-layout-name", layoutName );
    mainStyles.lookup( masterPage, "Standard", false );

    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );

    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:drawing" );

    m_doc.saveOasis( store, &contentTmpWriter, mainStyles ); // Save contents

    contentTmpWriter.endElement(); // office:drawing
    contentTmpWriter.endElement(); // office:body

    docWriter.startElement( "office:automatic-styles" );

    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( VDocument::STYLE_GRAPHICAUTO );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for( ; it != styles.end() ; ++it )
        (*it).style->writeStyle( &docWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );

    docWriter.endElement(); // office:automatic-styles

    styles = mainStyles.styles( KoGenStyle::STYLE_MASTER );
    it = styles.begin();
    docWriter.startElement("office:master-styles");

    for( ; it != styles.end(); ++it)
        (*it).style->writeStyle( &docWriter, mainStyles, "style:master-page", (*it).name, "");

    docWriter.endElement(); // office:master-styles

    // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    docWriter.addCompleteElement( tmpFile );
    contentTmpFile.close();

    docWriter.endElement(); // Root element
    docWriter.endDocument();

    if( !store->close() )
        return false;

    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    if( !store->open( "styles.xml" ) )
        return false;

    KoXmlWriter styleWriter( &storeDev, "office:document-styles" );

    styleWriter.startElement( "office:styles" );

    styles = mainStyles.styles( VDocument::STYLE_LINEAR_GRADIENT );
    it = styles.begin();
    for( ; it != styles.end() ; ++it )
        (*it).style->writeStyle( &styleWriter, mainStyles, "svg:linearGradient", (*it).name, 0, true,  true /*add draw:name*/);

    styles = mainStyles.styles( VDocument::STYLE_RADIAL_GRADIENT );
    it = styles.begin();
    for( ; it != styles.end() ; ++it )
        (*it).style->writeStyle( &styleWriter, mainStyles, "svg:radialGradient", (*it).name, 0, true,  true /*add draw:name*/);

    styleWriter.endElement(); // office:styles

    styleWriter.startElement( "office:automatic-styles" );

    QValueList<KoGenStyles::NamedStyle> styleList = mainStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    it = styleList.begin();

    for( ; it != styleList.end(); ++it )
        (*it).style->writeStyle( &styleWriter, mainStyles, "style:page-layout", (*it).name, "style:page-layout-properties" );

    styleWriter.endElement(); // office:automatic-styles

    styleWriter.endElement(); // Root element
    styleWriter.endDocument();

    if( !store->close() )
        return false;

    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );


    if(!store->open("settings.xml"))
        return false;


    KoXmlWriter settingsWriter(&storeDev, "office:document-settings");
    settingsWriter.startElement("office:settings");
    settingsWriter.startElement("config:config-item-set");
    settingsWriter.addAttribute("config:name", "view-settings");


    //<config:config-item-map-indexed config:name="Views">
    settingsWriter.startElement("config:config-item-map-indexed" );
    settingsWriter.addAttribute("config:name", "Views" );
    settingsWriter.startElement("config:config-item-map-entry" );
    KoUnit::saveOasis(&settingsWriter, unit());
    saveOasisSettings( settingsWriter );
    settingsWriter.endElement();


    settingsWriter.endElement(); //config:config-item-map-indexed
    settingsWriter.endElement(); // config:config-item-set
    settingsWriter.endElement(); // office:settings
    settingsWriter.endElement(); // Root element
    settingsWriter.endDocument();



    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    setModified( false );
    return true;
}

void
KarbonPart::saveOasisSettings( KoXmlWriter &/*settingsWriter*/ )
{
    //todo
}

void
KarbonPart::insertObject( VObject* object )
{
	// don't repaint here explicitly. some commands might want to insert many
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
KarbonPart::slotCommandExecuted( VCommand *command )
{
	setModified( true );
	if( command && command->changesSelection() )
	{
		QPtrListIterator<KoView> itr( views() );
			for( ; itr.current() ; ++itr )
				static_cast<KarbonView*>( itr.current() )->selectionChanged();
	}
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
	kdDebug(38000) << "**** part->paintContent()" << endl;
	painter.eraseRect( rect );
	VPainterFactory *painterFactory = new VPainterFactory;
	//QPaintDeviceMetrics metrics( painter.device() );
	painterFactory->setPainter( painter.device(), rect.width(), rect.height() );
	VPainter *p = painterFactory->painter();
	//VPainter *p = new VKoPainter( painter.device() );
	p->begin();
	p->setZoomFactor( zoomX );
	kdDebug(38000) << "painter.worldMatrix().dx() : " << painter.worldMatrix().dx() << endl;
	kdDebug(38000) << "painter.worldMatrix().dy() : " << painter.worldMatrix().dy() << endl;
	kdDebug(38000) << "rect.x() : "<< rect.x() << endl;
	kdDebug(38000) << "rect.y() : "<< rect.y() << endl;
	kdDebug(38000) << "rect.width() : "<< rect.width() << endl;
	kdDebug(38000) << "rect.height() : "<< rect.height() << endl;
	//p->setWorldMatrix( painter.worldMatrix() );
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( painter.worldMatrix().dx(), ( -painter.worldMatrix().dy() - rect.height() ) );
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
		m_maxRecentFiles = config->readNumEntry( "NbRecentFile", 10 );
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


	if (KGlobal::locale()->measureSystem() == KLocale::Imperial) {
		setUnit( KoUnit::U_INCH );
	} else {
		setUnit( KoUnit::U_CM );
	}

	if(config->hasGroup("Misc") ) {
	        config->setGroup( "Misc" );
		if ( config->hasKey( "Units" ) ) {
		         setUnit( KoUnit::unit( config->readEntry("Units") ) );
		}
	}
}

void
KarbonPart::setUnit( KoUnit::Unit _unit )
{
	m_doc.setUnit( _unit );
	QPtrListIterator<KoView> itr( views() );

	for( ; itr.current(); ++itr )
	{
		static_cast<KarbonView*>( itr.current() )->setUnit( _unit );
	}
}

bool
KarbonPart::mergeNativeFormat( const QString &file )
{
	m_merge = true;
	bool result = loadNativeFormat( file );
	m_merge = false;
	return result;
}

#include "karbon_part.moc"

