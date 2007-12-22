/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2007 Rob Buis <buis@kde.org>
   Copyright (C) 2002-2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002-2006 David Faure <faure@kde.org>
   Copyright (C) 2002 Stephan Kulow <coolo@kde.org>
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2003 Thomas Nagy <tnagyemail-mail@yahoo.fr>
   Copyright (C) 2003,2006 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>
   Copyright (C) 2004-2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005 Sven Langkamp <sven.langkamp@gmail.com>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2005 Johannes Schaub <johannes.schaub@kdemail.net>
   Copyright (C) 2006 Gabor Lehel <illissius@gmail.com>
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright (C) 2006 Jaison Lee <lee.jaison@gmail.com>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
   Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#include "karbon_part.h"
#include "karbon_factory.h"
#include "karbon_view.h"
#include "vglobal.h"
#include "KarbonCanvas.h"
#include "vdocumentdocker.h"

#include <KoApplication.h>
#include <KoOdfStylesReader.h>
#include <KoOasisLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoOasisSettings.h>
#include <KoMainWindow.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoImageCollection.h>
#include <KoCanvasResourceProvider.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <klocale.h>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QFileInfo>
#include <QtCore/QRectF>
#include <QtGui/QPainter>


// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName, QObject* parent, const char* name, bool singleViewMode )
: KoDocument( parentWidget, parent, singleViewMode )
{
    Q_UNUSED(widgetName)

    setObjectName(name);
    setComponentData( KarbonFactory::componentData(), false );
    setTemplateType( "karbon_template" );
    m_bShowStatusBar = true;

    initConfig();

    m_merge = false;

    m_maxRecentFiles = 10;

    // set as default paper
    m_pageLayout.format = KoPageFormat::defaultFormat();
    m_pageLayout.orientation = KoPageFormat::Portrait;
    m_pageLayout.width = MM_TO_POINT( KoPageFormat::width( m_pageLayout.format, m_pageLayout.orientation ) );
    m_pageLayout.height = MM_TO_POINT( KoPageFormat::height( m_pageLayout.format, m_pageLayout.orientation ) );
    setPageSize( QSizeF( m_pageLayout.width, m_pageLayout.height ) );
}

KarbonPart::~KarbonPart()
{
}

void
KarbonPart::setPageLayout( KoPageLayout& layout, KoUnit _unit )
{
    m_pageLayout = layout;
    m_doc.setUnit( _unit );
    setPageSize( QSizeF( m_pageLayout.width, m_pageLayout.height ) );
}

KoView* KarbonPart::createViewInstance( QWidget* parent )
{
    KarbonView *result = new KarbonView( this, parent );
    result->canvasWidget()->resourceProvider()->setResource( KoCanvasResource::PageSize, m_doc.pageSize() );
    return result;
}

void
KarbonPart::removeView( KoView *view )
{
    kDebug(38000) <<"KarbonPart::removeView";
    KoDocument::removeView( view );
}

static double getAttribute(KoXmlElement &element, const char *attributeName, double defaultValue)
{
    QString value = element.attribute( attributeName );
    if( ! value.isEmpty() )
        return value.toDouble();
    else
        return defaultValue;
}

static int getAttribute(KoXmlElement &element, const char *attributeName, int defaultValue)
{
    QString value = element.attribute( attributeName );
    if( ! value.isEmpty() )
        return value.toInt();
    else
        return defaultValue;
}

bool
KarbonPart::loadXML( QIODevice*, const KoXmlDocument& document )
{
    bool success = false;

    KoXmlElement doc = document.documentElement();

    if( m_merge )
    {
        m_doc.loadDocumentContent( doc );
        return true;
    }

    success = m_doc.loadXML( doc );

    //m_pageLayout = KoPageLayout::standardLayout();

    // <PAPER>
    KoXmlElement paper = doc.namedItem( "PAPER" ).toElement();
    if ( !paper.isNull() )
    {
        m_pageLayout.format = static_cast<KoPageFormat::Format>( getAttribute( paper, "format", 0 ) );
        m_pageLayout.orientation = static_cast<KoPageFormat::Orientation>( getAttribute( paper, "orientation", 0 ) );

        if( m_pageLayout.format == KoPageFormat::CustomSize )
        {
            m_pageLayout.width    = m_doc.pageSize().width();
            m_pageLayout.height    = m_doc.pageSize().height();
        }
        else
        {
            m_pageLayout.width = getAttribute( paper, "width", 0.0 );
            m_pageLayout.height = getAttribute( paper, "height", 0.0 );
        }
    }
    else
    {
        m_pageLayout.width = getAttribute( doc, "width", 595.277);
        m_pageLayout.height = getAttribute( doc, "height", 841.891 );
    }

    kDebug() <<" width=" << m_pageLayout.width;
    kDebug() <<" height=" << m_pageLayout.height;
        KoXmlElement borders = paper.namedItem( "PAPERBORDERS" ).toElement();
        if( !borders.isNull() )
    {
        if( borders.hasAttribute( "left" ) )
            m_pageLayout.left = borders.attribute( "left" ).toDouble();
        if( borders.hasAttribute( "top" ) )
            m_pageLayout.top = borders.attribute( "top" ).toDouble();
        if( borders.hasAttribute( "right" ) )
            m_pageLayout.right = borders.attribute( "right" ).toDouble();
        if( borders.hasAttribute( "bottom" ) )
            m_pageLayout.bottom = borders.attribute( "bottom" ).toDouble();
    }

    setUnit( m_doc.unit() );
    setPageSize( m_doc.pageSize() );

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
    paper.setAttribute( "width", m_pageLayout.width );
    paper.setAttribute( "height", m_pageLayout.height );
    paper.setAttribute( "orientation", static_cast<int>( m_pageLayout.orientation ) );

    QDomElement paperBorders = doc.createElement( "PAPERBORDERS" );
    paperBorders.setAttribute( "left", m_pageLayout.left );
    paperBorders.setAttribute( "top", m_pageLayout.top );
    paperBorders.setAttribute( "right", m_pageLayout.right );
    paperBorders.setAttribute( "bottom", m_pageLayout.bottom );
    paper.appendChild(paperBorders);

    return doc;
}

bool KarbonPart::loadOdf( KoOdfReadStore & odfStore )
{
    kDebug(38000) <<"Start loading OASIS document..." /*<< doc.toString()*/;

    KoXmlElement contents = odfStore.contentDoc().documentElement();
    kDebug(38000) <<"Start loading OASIS document..." << contents.text();
    kDebug(38000) <<"Start loading OASIS contents..." << contents.lastChild().localName();
    kDebug(38000) <<"Start loading OASIS contents..." << contents.lastChild().namespaceURI();
    kDebug(38000) <<"Start loading OASIS contents..." << contents.lastChild().isElement();
    KoXmlElement body( KoDom::namedItemNS( contents, KoXmlNS::office, "body" ) );
    kDebug(38000) <<"Start loading OASIS document..." << body.text();
    if( body.isNull() )
    {
        kDebug(38000) <<"No office:body found!";
        setErrorMessage( i18n( "Invalid OASIS document. No office:body tag found." ) );
        return false;
    }

    body = KoDom::namedItemNS( body, KoXmlNS::office, "drawing");
    if(body.isNull())
    {
        kDebug(38000) <<"No office:drawing found!";
        setErrorMessage( i18n( "Invalid OASIS document. No office:drawing tag found." ) );
        return false;
    }

    KoXmlElement page( KoDom::namedItemNS( body, KoXmlNS::draw, "page" ) );
    if(page.isNull())
    {
        kDebug(38000) <<"No office:drawing found!";
        setErrorMessage( i18n( "Invalid OASIS document. No draw:page tag found." ) );
        return false;
    }

    QString masterPageName = "Standard"; // use default layout as fallback
    KoXmlElement *master = odfStore.styles().masterPages()[ masterPageName ];
    if ( !master ) //last test...
        master = odfStore.styles().masterPages()[ "Default" ];
    Q_ASSERT( master );

    if( master )
    {
        const KoXmlElement *style = odfStore.styles().findStyle(
            master->attributeNS( KoXmlNS::style, "page-layout-name", QString() ) );
        m_pageLayout.loadOasis( *style );
        setPageSize( QSizeF( m_pageLayout.width, m_pageLayout.height ) );
    }
    else
        return false;

    KoOasisLoadingContext context( this, odfStore.styles(), odfStore.store() );
    KoShapeLoadingContext shapeContext( context );

    m_doc.loadOasis( page, shapeContext );

    shapeContext.transferShapesToDocument( this );

    if( m_doc.pageSize().isEmpty() )
    {
        QSizeF pageSize = m_doc.contentRect().united( QRectF(0,0,1,1) ).size();
        setPageSize( pageSize );
    }

    loadOasisSettings( odfStore.settingsDoc() );

    return true;
}

bool KarbonPart::completeLoading( KoStore* store )
{
    return m_doc.imageCollection()->loadFromStore( store );
}

void
KarbonPart::loadOasisSettings( const KoXmlDocument&settingsDoc )
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


bool KarbonPart::saveOdf( SavingContext &documentContext )
{
    KoGenStyles mainStyles;

    /*
    KoGenStyle pageLayout = m_pageLayout.saveOasis();
    QString layoutName = mainStyles.lookup( pageLayout, "PL" );
    KoGenStyle masterPage( KoGenStyle::StyleMaster );
    masterPage.addAttribute( "style:page-layout-name", layoutName );
    mainStyles.lookup( masterPage, "Default", KoGenStyles::DontForceNumbering );
    */

    if( ! m_doc.saveOasis( documentContext.odfStore.store(), documentContext.odfStore.manifestWriter(), mainStyles ) )
        return false;

    setModified( false );

    return true;
}

void
KarbonPart::slotDocumentRestored()
{
    setModified( false );
}

void
KarbonPart::repaintAllViews( bool /*repaint*/ )
{
// TODO: needs porting
/*
    foreach ( KoView* view, views() )
        static_cast<KarbonView*>( view )->canvasWidget()->repaintAll( repaint );*/
}

void KarbonPart::paintContent( QPainter& painter, const QRect& rect)
{
    Q_UNUSED( painter );
    Q_UNUSED( rect );
    kDebug(38000) <<"**** part->paintContent()";

    /*
    QRectF r = rect;
    double zoomFactorX = double( r.width() ) / double( document().pageSize().width() );
    double zoomFactorY = double( r.height() ) / double( document().pageSize().height() );
    double zoomFactor = qMin( zoomFactorX, zoomFactorY );
    */
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
KarbonPart::initConfig()
{
    KSharedConfigPtr config = KarbonPart::componentData().config();

    // disable grid by default
    gridData().setShowGrid( false );

    if( config->hasGroup( "Interface" ) )
    {
        KConfigGroup interfaceGroup = config->group( "Interface" );
        setAutoSave( interfaceGroup.readEntry( "AutoSave", defaultAutoSave() / 60 ) * 60 );
        m_maxRecentFiles = interfaceGroup.readEntry( "NbRecentFile", 10 );
        setShowStatusBar( interfaceGroup.readEntry( "ShowStatusBar" , true ) );
        setBackupFile( interfaceGroup.readEntry( "BackupFile", true ) );
        m_doc.saveAsPath( interfaceGroup.readEntry( "SaveAsPath", false ) );
    }
    int undos = 30;
    if( config->hasGroup( "Misc" ) )
    {
        KConfigGroup miscGroup = config->group( "Misc" );
        undos = miscGroup.readEntry( "UndoRedo", -1 );
        QString defaultUnit = "cm";

        if( KGlobal::locale()->measureSystem() == KLocale::Imperial )
            defaultUnit = "in";

        setUnit( KoUnit::unit( miscGroup.readEntry( "Units", defaultUnit ) ) );
        m_doc.setUnit( unit() );
    }
    if( config->hasGroup( "Grid" ) )
    {
        KoGridData defGrid;
        KConfigGroup gridGroup = config->group( "Grid" );
        double spacingX = gridGroup.readEntry<double>( "SpacingX", defGrid.gridX() );
        double spacingY = gridGroup.readEntry<double>( "SpacingY", defGrid.gridY() );
        gridData().setGrid( spacingX, spacingY );
        //double snapX = gridGroup.readEntry<double>( "SnapX", defGrid.snapX() );
        //double snapY = gridGroup.readEntry<double>( "SnapY", defGrid.snapY() );
        //m_doc.grid().setSnap( snapX, snapY );
        QColor color = gridGroup.readEntry( "Color", defGrid.gridColor() );
        gridData().setGridColor( color );
    }
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

void KarbonPart::addShape( KoShape* shape )
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();

    KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>( shape );
    if( layer )
    {
        m_doc.insertLayer( layer );
        if( canvasController )
        {
            KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
            selection->setActiveLayer( layer );
        }
    }
    else
    {
        // only add shape to active layer if it has no parent yet
        if( ! shape->parent() )
        {
            kDebug(38000) <<"shape has no parent, adding to the active layer!";
            KoShapeLayer *activeLayer = 0;
            if( canvasController )
                activeLayer = canvasController->canvas()->shapeManager()->selection()->activeLayer();
            else if( m_doc.layers().count() )
                activeLayer = m_doc.layers().first();

            if( activeLayer )
                activeLayer->addChild( shape );
        }

        m_doc.add( shape );

        foreach( KoView *view, views() ) {
            KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
            canvas->shapeManager()->add(shape);
        }
    }

    setModified( true );
}

void
KarbonPart::removeShape( KoShape* shape )
{
    KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>( shape );
    if( layer )
    {
        m_doc.removeLayer( layer );
    }
    else
    {
        m_doc.remove( shape );
        foreach( KoView *view, views() ) {
            KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
            canvas->shapeManager()->remove(shape);
        }
    }
    setModified( true );
}

void KarbonPart::updateDocumentSize()
{
    KoCanvasController * canvasController = KoToolManager::instance()->activeCanvasController();
    const KoViewConverter * viewConverter = canvasController->canvas()->viewConverter();
    QSize documentSize = viewConverter->documentToView( m_doc.boundingRect() ).size().toSize();
    canvasController->setDocumentSize( documentSize );
}

void KarbonPart::setPageSize( const QSizeF &pageSize )
{
    m_doc.setPageSize( pageSize );
    foreach( KoView *view, views() )
    {
        KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
        canvas->resourceProvider()->setResource( KoCanvasResource::PageSize, pageSize );
    }
}

#include "karbon_part.moc"

