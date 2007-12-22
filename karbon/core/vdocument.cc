/* This file is part of the KDE project
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002-2007 Rob Buis <buis@kde.org>
   Copyright (C) 2002,2005 David Faure <faure@kde.org>
   Copyright (C) 2002 Laurent Montel <montel@kde.org>
   Copyright (C) 2004 Waldo Bastian <bastian@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Gabor Lehel <illissius@gmail.com>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "vdocument.h"

#include <KoStore.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>
#include <KoOasisLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoStoreDevice.h>
#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeStyleWriter.h>
#include <KoImageCollection.h>

#include <ktemporaryfile.h>
#include <kdebug.h>

#include <qdom.h>
#include <QRectF>


class VDocument::Private
{
public:
    Private()
    : pageSize(0.0, 0.0)
    , unit( KoUnit::Millimeter )
    , saveAsPath(true)
    {}

    ~Private()
    {
        qDeleteAll( layers );
        layers.clear();
        qDeleteAll( objects );
        objects.clear();
    }

    QSizeF pageSize; ///< the documents page size

    QList<KoShape*> objects;   ///< The list of all object of the document.
    VLayerList layers;         ///< The layers in this document.

    KoUnit unit; ///< The unit.
    KoImageCollection imageCollection; ///< the image collection

    // TODO this flag is used nowhere, can we remove it?
    bool saveAsPath;
};

VDocument::VDocument()
: d( new Private )
{
    // create a layer. we need at least one:
    insertLayer( new KoShapeLayer() );
}

VDocument::VDocument( const VDocument& document )
: d( new Private )
{
    d->layers = document.layers();
// TODO
}

VDocument::~VDocument()
{
    delete d;
}

void VDocument::insertLayer( KoShapeLayer* layer )
{
    d->layers.append( layer );
}

void VDocument::removeLayer( KoShapeLayer* layer )
{
    d->layers.removeAt( d->layers.indexOf( layer ) );
    if ( d->layers.count() == 0 )
        d->layers.append( new KoShapeLayer() );
}

bool VDocument::canRaiseLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    return (pos != int( d->layers.count() ) - 1 && pos >= 0 );
}

bool VDocument::canLowerLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    return (pos>0);
}

void VDocument::raiseLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    if( pos != int( d->layers.count() ) - 1 && pos >= 0 )
        d->layers.move( pos, pos + 1 );
}

void VDocument::lowerLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    if ( pos > 0 )
        d->layers.move( pos, pos - 1 );
}

int VDocument::layerPos( KoShapeLayer* layer )
{
    return d->layers.indexOf( layer );
}

void VDocument::add( KoShape* shape )
{
    if( ! d->objects.contains( shape ) )
        d->objects.append( shape );
}

void VDocument::remove( KoShape* shape )
{
    d->objects.removeAt( d->objects.indexOf( shape ) );
}

QDomDocument
VDocument::saveXML() const
{
    QDomDocument doc;
    QDomElement me = doc.createElement( "DOC" );
    doc.appendChild( me );
    save( me );
    return doc;
 }

void VDocument::saveOasis( KoShapeSavingContext & context ) const
{
    context.xmlWriter().startElement( "draw:page" );
    context.xmlWriter().addAttribute( "draw:name", "" );
    context.xmlWriter().addAttribute( "draw:id", "page1");
    context.xmlWriter().addAttribute( "draw:master-page-name", "Default");

    foreach( KoShapeLayer * layer, d->layers )
        layer->saveOdf( context );

    context.xmlWriter().endElement(); // draw:page
}

void
VDocument::save( QDomElement& me ) const
{
    me.setAttribute( "mime", "application/x-karbon" ),
    me.setAttribute( "version", "0.1" );
    me.setAttribute( "editor", "Karbon14" );
    me.setAttribute( "syntaxVersion", "0.1" );
    if( d->pageSize.width() > 0.0 )
        me.setAttribute( "width", d->pageSize.width() );
    if( d->pageSize.height() > 0. )
        me.setAttribute( "height", d->pageSize.height() );
    me.setAttribute( "unit", KoUnit::unitName( d->unit ) );

    // save objects:
    /* TODO: porting to flake
    VLayerListIterator itr( m_layers );

    for ( ; itr.current(); ++itr )
            itr.current()->save( me );
    */
}

VDocument*
VDocument::clone() const
{
    return new VDocument( *this );
}

void
VDocument::load( const KoXmlElement& doc )
{
    loadXML( doc );
}

bool VDocument::loadXML( const KoXmlElement& doc )
{
    if( doc.attribute( "mime" ) != "application/x-karbon" ||
        doc.attribute( "syntaxVersion" ) != "0.1" )
    {
        return false;
    }

    qDeleteAll( d->layers );
    d->layers.clear();
    qDeleteAll( d->objects );
    d->objects.clear();

    d->pageSize.setWidth( doc.attribute( "width", "800.0" ).toDouble() );
    d->pageSize.setHeight( doc.attribute( "height", "550.0" ).toDouble() );

    d->unit = KoUnit::unit( doc.attribute( "unit", KoUnit::unitName( d->unit ) ) );

    loadDocumentContent( doc );

    if( d->layers.isEmpty() )
        insertLayer( new KoShapeLayer() );

    return true;
}

void
VDocument::loadDocumentContent( const KoXmlElement& doc )
{
    KoXmlElement e;
    forEachElement(e, doc)
    {
        if( e.tagName() == "LAYER" )
        {
            KoShapeLayer* layer = new KoShapeLayer();
            // TODO implement layer loading
            //layer->load( e );
            insertLayer( layer );
        }
    }
}

bool VDocument::loadOasis( const KoXmlElement &element, KoShapeLoadingContext &context )
{
    qDeleteAll( d->layers );
    d->layers.clear();
    qDeleteAll( d->objects );
    d->objects.clear();

    context.setImageCollection( &d->imageCollection );

    KoXmlElement layerElement;
    forEachElement( layerElement, context.koLoadingContext().stylesReader().layerSet() )
    {
        KoShapeLayer * l = new KoShapeLayer();
        if( l->loadOdf( layerElement, context ) )
            insertLayer( l );
    }

    KoShapeLayer * defaultLayer = 0;

    // check if we have to insert a default layer
    if( d->layers.count() == 0 )
        defaultLayer = new KoShapeLayer();

    KoXmlElement child;
    forEachElement( child, element )
    {
        kDebug(38000) <<"loading shape" << child.localName();

        KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf( child, context );
        if( shape )
            d->objects.append( shape );
    }

    // add all toplevel shapes to the default layer
    foreach( KoShape * shape, d->objects )
    {
        if( ! shape->parent() )
        {
            if( ! defaultLayer )
                defaultLayer = new KoShapeLayer();

            defaultLayer->addChild( shape );
        }
    }

    if( defaultLayer )
        insertLayer( defaultLayer );

    return true;
}

QRectF VDocument::boundingRect() const
{
    return contentRect().united( QRectF( QPointF(0,0), d->pageSize ) );
}

QRectF VDocument::contentRect() const
{
    QRectF bb;
    foreach( KoShape* layer, d->layers )
    {
        if( bb.isNull() )
            bb = layer->boundingRect();
        else
            bb = bb.united(  layer->boundingRect() );
    }

    return bb;
}

QSizeF VDocument::pageSize() const
{
    return d->pageSize;
}

void VDocument::setPageSize( QSizeF pageSize )
{
    d->pageSize = pageSize;
}

const QList<KoShape*> VDocument::shapes() const
{
    return d->objects;
}

bool VDocument::saveAsPath() const
{
    return d->saveAsPath;
}

void VDocument::saveAsPath( bool b )
{
    d->saveAsPath = b;
}

KoUnit VDocument::unit() const
{
    return d->unit;
}

void VDocument::setUnit( KoUnit unit )
{
    d->unit = unit;
}

const VLayerList& VDocument::layers() const
{
    return d->layers;
}

KoImageCollection * VDocument::imageCollection()
{
    return &d->imageCollection;
}

//#############################################################################
// ODF saving
//#############################################################################

bool VDocument::saveOdf( KoDocument::SavingContext &documentContext )
{
    KoStore * store = documentContext.odfStore.store();
    KoXmlWriter * manifestWriter = documentContext.odfStore.manifestWriter();

    if( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice storeDev( store );
    KoXmlWriter * docWriter = KoOdfWriteStore::createOasisXmlWriter( &storeDev, "office:document-content" );

    // for office:master-styles
    KTemporaryFile masterStyles;
    masterStyles.open();
    KoXmlWriter masterStylesTmpWriter( &masterStyles, 1 );

    KoPageLayout page;
    page.format = KoPageFormat::defaultFormat();
    page.orientation = KoPageFormat::Portrait;
    page.width = pageSize().width();
    page.height = pageSize().height();

    KoGenStyles mainStyles;
    KoGenStyle pageLayout = page.saveOasis();
    QString layoutName = mainStyles.lookup( pageLayout, "PL" );
    KoGenStyle masterPage( KoGenStyle::StyleMaster );
    masterPage.addAttribute( "style:page-layout-name", layoutName );
    mainStyles.lookup( masterPage, "Default", KoGenStyles::DontForceNumbering );

    KTemporaryFile contentTmpFile;
    contentTmpFile.open();
    KoXmlWriter contentTmpWriter( &contentTmpFile, 1 );

    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:drawing" );

    KoShapeSavingContext shapeContext( contentTmpWriter, mainStyles, documentContext.embeddedSaver );
    saveOasis( shapeContext ); // Save contents

    contentTmpWriter.endElement(); // office:drawing
    contentTmpWriter.endElement(); // office:body

    mainStyles.saveOdfAutomaticStyles( docWriter, false );

    // And now we can copy over the contents from the tempfile to the real one
    contentTmpFile.seek(0);
    docWriter->addCompleteElement( &contentTmpFile );

    docWriter->endElement(); // Root element
    docWriter->endDocument();
    delete docWriter;

    if( !store->close() )
        return false;

    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    if ( ! mainStyles.saveOdfStylesDotXml( store, manifestWriter ) ) {
        return false;
    }

    if(!store->open("settings.xml"))
        return false;

    saveOasisSettings( store );

    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    if( ! shapeContext.saveImages( store, manifestWriter ) )
        return false;

    return true;
}

void VDocument::saveOasisSettings( KoStore * store )
{
    KoStoreDevice settingsDev( store );
    KoXmlWriter * settingsWriter = KoOdfWriteStore::createOasisXmlWriter( &settingsDev, "office:document-settings");

    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    KoUnit::saveOasis( settingsWriter, unit() );

    settingsWriter->endElement(); // config:config-item-set
    settingsWriter->endElement(); // office:settings
    settingsWriter->endElement(); // office:document-settings
    settingsWriter->endDocument();

    delete settingsWriter;
}
