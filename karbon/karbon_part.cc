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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <qdom.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <q3paintdevicemetrics.h>
#include <QRectF>
#include <Q3ValueList>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <ktempfile.h>
#include <KoStoreDevice.h>
#include <KoOasisStyles.h>
#include <KoOasisLoadingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoOasisSettings.h>
#include <KoMainWindow.h>

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
#include "KoApplication.h"
#include "vtool.h"
#include "commands/vtransformcmd.h"

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
						QObject* parent, const char* name, bool singleViewMode )
		: KoDocument( parentWidget, parent, singleViewMode )
{
	setInstance( KarbonFactory::instance(), false );
	setTemplateType( "karbon_template" );
	m_bShowStatusBar = true;
	dcop = 0L;

	m_commandHistory = new VCommandHistory( this );
	connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
	connect( m_commandHistory, SIGNAL( commandExecuted( VCommand * ) ), this, SLOT( slotCommandExecuted( VCommand * ) ) );

	initConfig();

	m_merge = false;

	m_maxRecentFiles = 10;

	//if( name )
        dcopObject();

	// set as default paper
	m_pageLayout.format = KoPageFormat::defaultFormat();
	m_pageLayout.orientation = PG_PORTRAIT;
	m_pageLayout.ptWidth = MM_TO_POINT( KoPageFormat::width( m_pageLayout.format, m_pageLayout.orientation ) );
	m_pageLayout.ptHeight = MM_TO_POINT( KoPageFormat::height( m_pageLayout.format, m_pageLayout.orientation ) );
	m_doc.setWidth( m_pageLayout.ptWidth );
	m_doc.setHeight( m_pageLayout.ptHeight );
	// enable selection drawing
	m_doc.selection()->showHandle();
	m_doc.selection()->setSelectObjects();
	m_doc.selection()->setState( VObject::selected );
	m_doc.selection()->selectNodes();
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

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
	KarbonView *result = new KarbonView( this, parent, name );
	return result;
}

void
KarbonPart::removeView( KoView *view )
{
	kDebug(38000) << "KarbonPart::removeView" << endl;
	KoDocument::removeView( view );
}

double getAttribute(QDomElement &element, const char *attributeName, double defaultValue)
{
	QString value;
	if ( ( value = element.attribute( attributeName ) ) != QString::null )
		return value.toDouble();
	else
		return defaultValue;
}

int getAttribute(QDomElement &element, const char *attributeName, int defaultValue)
{
	QString value;
	if ( ( value = element.attribute( attributeName ) ) != QString::null )
		return value.toInt();
	else
		return defaultValue;
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

	//m_pageLayout = KoPageLayout::standardLayout();

	// <PAPER>
	QDomElement paper = doc.namedItem( "PAPER" ).toElement();
	if ( !paper.isNull() )
	{
		m_pageLayout.format = static_cast<KoFormat>( getAttribute( paper, "format", 0 ) );
		m_pageLayout.orientation = static_cast<KoOrientation>( getAttribute( paper, "orientation", 0 ) );

		if( m_pageLayout.format == PG_CUSTOM )
		{
			m_pageLayout.ptWidth	= m_doc.width();
			m_pageLayout.ptHeight	= m_doc.height();
		}
		else
		{
			m_pageLayout.ptWidth = getAttribute( paper, "width", 0.0 );
			m_pageLayout.ptHeight = getAttribute( paper, "height", 0.0 );
		}
	}
	else
	{
		m_pageLayout.ptWidth = getAttribute( doc, "width", 595.277);
		m_pageLayout.ptHeight = getAttribute( doc, "height", 841.891 );
	}

	kDebug() << " ptWidth=" << m_pageLayout.ptWidth << endl;
	kDebug() << " ptHeight=" << m_pageLayout.ptHeight << endl;
        QDomElement borders = paper.namedItem( "PAPERBORDERS" ).toElement();
        if( !borders.isNull() )
       	{
            if( borders.hasAttribute( "ptLeft" ) )
                m_pageLayout.ptLeft = borders.attribute( "ptLeft" ).toDouble();
            if( borders.hasAttribute( "ptTop" ) )
                m_pageLayout.ptTop = borders.attribute( "ptTop" ).toDouble();
            if( borders.hasAttribute( "ptRight" ) )
                m_pageLayout.ptRight = borders.attribute( "ptRight" ).toDouble();
            if( borders.hasAttribute( "ptBottom" ) )
                m_pageLayout.ptBottom = borders.attribute( "ptBottom" ).toDouble();
	}

	setUnit( m_doc.unit() );

	return success;
}

QDomDocument
KarbonPart::saveXML()
{
	QDomDocument doc = m_doc.saveXML();
	QDomElement me = doc.documentElement();
	QDomElement paper = doc.createElement( "PAPER" );
	me.appendChild( paper );
	paper.setAttribute( "format", static_cast<int>( m_pageLayout.format ) );
	paper.setAttribute( "pages", pageCount() );
	paper.setAttribute( "width", m_pageLayout.ptWidth );
	paper.setAttribute( "height", m_pageLayout.ptHeight );
	paper.setAttribute( "orientation", static_cast<int>( m_pageLayout.orientation ) );

	QDomElement paperBorders = doc.createElement( "PAPERBORDERS" );
	paperBorders.setAttribute( "ptLeft", m_pageLayout.ptLeft );
	paperBorders.setAttribute( "ptTop", m_pageLayout.ptTop );
	paperBorders.setAttribute( "ptRight", m_pageLayout.ptRight );
	paperBorders.setAttribute( "ptBottom", m_pageLayout.ptBottom );
	paper.appendChild(paperBorders);

	return doc;
}

bool
KarbonPart::loadOasis( const QDomDocument &doc, KoOasisStyles &styles, const QDomDocument &settings, KoStore *store )
{
	kDebug(38000) << "Start loading OASIS document..." << doc.toString() << endl;

	QDomElement contents = doc.documentElement();
	kDebug(38000) << "Start loading OASIS document..." << contents.text() << endl;
	kDebug(38000) << "Start loading OASIS contents..." << contents.lastChild().localName() << endl;
	kDebug(38000) << "Start loading OASIS contents..." << contents.lastChild().namespaceURI() << endl;
	kDebug(38000) << "Start loading OASIS contents..." << contents.lastChild().isElement() << endl;
	QDomElement body( KoDom::namedItemNS( contents, KoXmlNS::office, "body" ) );
	kDebug(38000) << "Start loading OASIS document..." << body.text() << endl;
	if( body.isNull() )
	{
		kDebug(38000) << "No office:body found!" << endl;
		setErrorMessage( i18n( "Invalid OASIS document. No office:body tag found." ) );
		return false;
	}

	body = KoDom::namedItemNS( body, KoXmlNS::office, "drawing");
	if(body.isNull())
	{
		kDebug(38000) << "No office:drawing found!" << endl;
		setErrorMessage( i18n( "Invalid OASIS document. No office:drawing tag found." ) );
		return false;
	}

	QDomElement page( KoDom::namedItemNS( body, KoXmlNS::draw, "page" ) );
	if(page.isNull())
	{
		kDebug(38000) << "No office:drawing found!" << endl;
		setErrorMessage( i18n( "Invalid OASIS document. No draw:page tag found." ) );
		return false;
	}

	QString masterPageName = "Standard"; // use default layout as fallback
	QDomElement *master = styles.masterPages()[ masterPageName ];
	if ( !master ) //last test...
		master = styles.masterPages()[ "Default" ];
	Q_ASSERT( master );
	const QDomElement *style = master ? styles.findStyle( master->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
	if( style )
	{
		m_pageLayout.loadOasis( *style );
		m_doc.setWidth( m_pageLayout.ptWidth );
		m_doc.setHeight( m_pageLayout.ptHeight );
	}
	else
		return false;

	KoOasisLoadingContext context( this, styles, store );
	m_doc.loadOasis( page, context );
	// do y-mirroring here
	QMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0, -m_doc.height() );
	VTransformCmd trafo( 0L, mat );
	trafo.visit( m_doc );

	loadOasisSettings( settings );

	return true;
}

void
KarbonPart::loadOasisSettings( const QDomDocument&settingsDoc )
{
    if ( settingsDoc.isNull() )
        return ; // not an error if some file doesn't have settings.xml
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
    {
        setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
        // FIXME: add other config here.
    }
}


bool
KarbonPart::saveOasis( KoStore *store, KoXmlWriter *manifestWriter )
{
    if( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice storeDev( store );
    KoXmlWriter* docWriter = createOasisXmlWriter( &storeDev, "office:document-content" );
    KoGenStyles mainStyles;

    KoGenStyle pageLayout = m_pageLayout.saveOasis();
    QString layoutName = mainStyles.lookup( pageLayout, "PL" );
    KoGenStyle masterPage( KoGenStyle::STYLE_MASTER );
    masterPage.addAttribute( "style:page-layout-name", layoutName );
    mainStyles.lookup( masterPage, "Default", KoGenStyles::DontForceNumbering );

    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );

    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:drawing" );

    m_doc.saveOasis( store, &contentTmpWriter, mainStyles ); // Save contents

    contentTmpWriter.endElement(); // office:drawing
    contentTmpWriter.endElement(); // office:body

    docWriter->startElement( "office:automatic-styles" );

    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( VDocument::STYLE_GRAPHICAUTO );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for( ; it != styles.end() ; ++it )
        (*it).style->writeStyle( docWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );

    docWriter->endElement(); // office:automatic-styles

    // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    docWriter->addCompleteElement( tmpFile );
    contentTmpFile.close();

    docWriter->endElement(); // Root element
    docWriter->endDocument();
    delete docWriter;

    if( !store->close() )
        return false;

    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    if( !store->open( "styles.xml" ) )
        return false;

    KoXmlWriter* styleWriter = createOasisXmlWriter( &storeDev, "office:document-styles" );

    styleWriter->startElement( "office:styles" );

    styles = mainStyles.styles( VDocument::STYLE_LINEAR_GRADIENT );
    it = styles.begin();
    for( ; it != styles.end() ; ++it )
        (*it).style->writeStyle( styleWriter, mainStyles, "svg:linearGradient", (*it).name, 0, true,  true /*add draw:name*/);

    styles = mainStyles.styles( VDocument::STYLE_RADIAL_GRADIENT );
    it = styles.begin();
    for( ; it != styles.end() ; ++it )
        (*it).style->writeStyle( styleWriter, mainStyles, "svg:radialGradient", (*it).name, 0, true,  true /*add draw:name*/);

    styleWriter->endElement(); // office:styles

    styleWriter->startElement( "office:automatic-styles" );

    Q3ValueList<KoGenStyles::NamedStyle> styleList = mainStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    it = styleList.begin();

    for( ; it != styleList.end(); ++it )
        (*it).style->writeStyle( styleWriter, mainStyles, "style:page-layout", (*it).name, "style:page-layout-properties" );

    styleWriter->endElement(); // office:automatic-styles

    styles = mainStyles.styles( KoGenStyle::STYLE_MASTER );
    it = styles.begin();
    styleWriter->startElement("office:master-styles");

    for( ; it != styles.end(); ++it)
        (*it).style->writeStyle( styleWriter, mainStyles, "style:master-page", (*it).name, "");

    styleWriter->endElement(); // office:master-styles

    styleWriter->endElement(); // Root element
    styleWriter->endDocument();
    delete styleWriter;

    if( !store->close() )
        return false;

    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );


    if(!store->open("settings.xml"))
        return false;


    KoXmlWriter& settingsWriter = *createOasisXmlWriter(&storeDev, "office:document-settings");
    settingsWriter.startElement("office:settings");
    settingsWriter.startElement("config:config-item-set");
    settingsWriter.addAttribute("config:name", "view-settings");

    KoUnit::saveOasis(&settingsWriter, unit());
    saveOasisSettings( settingsWriter );

    settingsWriter.endElement(); // config:config-item-set
    settingsWriter.endElement(); // office:settings
    settingsWriter.endElement(); // Root element
    settingsWriter.endDocument();
    delete &settingsWriter;


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
		foreach ( KoView* view, views() )
			static_cast<KarbonView*>( view )->selectionChanged();
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
	foreach ( KoView* view, views() )
		static_cast<KarbonView*>( view )->canvasWidget()->repaintAll( repaint );
}

void
KarbonPart::repaintAllViews( const QRectF &rect )
{
	foreach ( KoView* view, views() )
		static_cast<KarbonView*>( view )->canvasWidget()->repaintAll( rect );
}

void
KarbonPart::paintContent( QPainter& painter, const QRect& rect,
						  bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
	kDebug(38000) << "**** part->paintContent()" << endl;

	QRectF r = rect;
	double zoomFactorX = double( r.width() ) / double( document().width() );
	double zoomFactorY = double( r.height() ) / double( document().height() );
	double zoomFactor = qMin( zoomFactorX, zoomFactorY );

	painter.eraseRect( rect );
	VPainterFactory *painterFactory = new VPainterFactory;
	//QPaintDeviceMetrics metrics( painter.device() );
	painterFactory->setPainter( painter.device(), rect.width(), rect.height() );
	VPainter *p = painterFactory->painter();
	//VPainter *p = new VQPainter( painter.device() );
	p->begin();
	p->setZoomFactor( zoomFactor );
	kDebug(38000) << "painter.worldMatrix().dx() : " << painter.worldMatrix().dx() << endl;
	kDebug(38000) << "painter.worldMatrix().dy() : " << painter.worldMatrix().dy() << endl;
	kDebug(38000) << "rect.x() : "<< rect.x() << endl;
	kDebug(38000) << "rect.y() : "<< rect.y() << endl;
	kDebug(38000) << "rect.width() : "<< rect.width() << endl;
	kDebug(38000) << "rect.height() : "<< rect.height() << endl;
	r = document().boundingBox();
	QMatrix mat = painter.worldMatrix();
	mat.scale( 1, -1 );
	mat.translate( 0, -r.height() * zoomFactor );
	p->setMatrix( mat );

	m_doc.selection()->clear();
	Q3PtrListIterator<VLayer> itr( m_doc.layers() );

	for( ; itr.current(); ++itr )
	{
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
	foreach ( KoView* view, views() )
		static_cast<KarbonView*>( view )->reorganizeGUI();
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
		m_doc.saveAsPath( config->readBoolEntry( "SaveAsPath", true ) );
	}
	int undos = 30;
	if( config->hasGroup( "Misc" ) )
	{
		config->setGroup( "Misc" );
		undos = config->readNumEntry( "UndoRedo", -1 );
		QString defaultUnit = "cm";

		if( KGlobal::locale()->measureSystem() == KLocale::Imperial )
			defaultUnit = "in";

		setUnit( KoUnit::unit( config->readEntry( "Units", defaultUnit ) ) );
		m_doc.setUnit( unit() );
	}
	if( undos != -1 )
		setUndoRedoLimit( undos );
}

bool
KarbonPart::mergeNativeFormat( const QString &file )
{
	m_merge = true;
	bool result = loadNativeFormat( file );
	if ( !result )
		showLoadingErrorDialog();
	m_merge = false;
	return result;
}

void
KarbonPart::addShell( KoMainWindow *shell )
{
	connect( shell, SIGNAL( documentSaved() ), m_commandHistory, SLOT( documentSaved() ) );
	KoDocument::addShell( shell );
}


void
KarbonPart::slotUnitChanged( KoUnit::Unit /*unit*/ )
{
#if 0
	// VDocument has its own storage of the unit...
	m_doc.setUnit( unit );
	if( m_toolController->activeTool() )
		m_toolController->activeTool()->refreshUnit();
#endif
}

#include "karbon_part.moc"

