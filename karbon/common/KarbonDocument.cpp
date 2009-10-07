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

#include "KarbonDocument.h"

#include <KoStore.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoStoreDevice.h>
#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoImageCollection.h>
#include <KoDataCenter.h>
#include <KoStyleManager.h>
#include <KoTextSharedLoadingData.h>

#include <ktemporaryfile.h>
#include <kdebug.h>

#include <qdom.h>
#include <QRectF>

class KarbonDocument::Private
{
public:
    Private()
    : pageSize(0.0, 0.0)
    , unit( KoUnit::Millimeter )
    , hasExternalDataCenterMap(false)
    {
        // Ask every shapefactory to populate the dataCenterMap
        foreach( const QString & id, KoShapeRegistry::instance()->keys())
        {
            KoShapeFactory *shapeFactory = KoShapeRegistry::instance()->value(id);
            shapeFactory->populateDataCenterMap( dataCenterMap );
        }
    }

    ~Private()
    {
        qDeleteAll( layers );
        layers.clear();
        qDeleteAll( objects );
        objects.clear();
        if (!hasExternalDataCenterMap)
            qDeleteAll( dataCenterMap );
    }

    QSizeF pageSize; ///< the documents page size

    QList<KoShape*> objects;     ///< The list of all object of the document.
    QList<KoShapeLayer*> layers; ///< The layers in this document.

    KoUnit unit; ///< The unit.
    QMap<QString, KoDataCenter*> dataCenterMap;
    bool hasExternalDataCenterMap;
};

KarbonDocument::KarbonDocument()
: d( new Private )
{
    // create a layer. we need at least one:
    insertLayer( new KoShapeLayer() );
}

KarbonDocument::KarbonDocument( const KarbonDocument& document )
: d( new Private )
{
    d->layers = document.layers();
// TODO
}

KarbonDocument::~KarbonDocument()
{
    delete d;
}

void KarbonDocument::insertLayer( KoShapeLayer* layer )
{
    if (!d->layers.contains(layer)) {
        if (d->layers.count()) {
            layer->setZIndex(d->layers.last()->zIndex()+1);
        } else {
            layer->setZIndex(d->layers.count());
        }
        d->layers.append( layer );
    }
}

void KarbonDocument::removeLayer( KoShapeLayer* layer )
{
    d->layers.removeAt( d->layers.indexOf( layer ) );
    if ( d->layers.count() == 0 )
        d->layers.append( new KoShapeLayer() );
}

bool KarbonDocument::canRaiseLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    return (pos != int( d->layers.count() ) - 1 && pos >= 0 );
}

bool KarbonDocument::canLowerLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    return (pos>0);
}

void KarbonDocument::raiseLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    if( pos != int( d->layers.count() ) - 1 && pos >= 0 ) {
        KoShapeLayer * layerAbove = d->layers.at(pos+1);
        int lowerZIndex = layer->zIndex();
        int upperZIndex = layerAbove->zIndex();
        layer->setZIndex(upperZIndex);
        layerAbove->setZIndex(lowerZIndex);
        d->layers.move( pos, pos + 1 );
    }
}

void KarbonDocument::lowerLayer( KoShapeLayer* layer )
{
    int pos = d->layers.indexOf( layer );
    if ( pos > 0 ) {
        KoShapeLayer * layerBelow = d->layers.at(pos-1);
        int upperZIndex = layer->zIndex();
        int lowerZIndex = layerBelow->zIndex();
        layer->setZIndex(lowerZIndex);
        layerBelow->setZIndex(upperZIndex);
        d->layers.move( pos, pos - 1 );
    }
}

int KarbonDocument::layerPos( KoShapeLayer* layer )
{
    return d->layers.indexOf( layer );
}

void KarbonDocument::add( KoShape* shape )
{
    if( ! d->objects.contains( shape ) )
        d->objects.append( shape );
}

void KarbonDocument::remove( KoShape* shape )
{
    d->objects.removeAt( d->objects.indexOf( shape ) );
}

void KarbonDocument::saveOasis( KoShapeSavingContext & context ) const
{
    context.xmlWriter().startElement( "draw:page" );
    context.xmlWriter().addAttribute( "draw:name", "" );
    context.xmlWriter().addAttribute( "draw:id", "page1");
    context.xmlWriter().addAttribute( "draw:master-page-name", "Default");

    foreach( KoShapeLayer * layer, d->layers )
        layer->saveOdf( context );

    context.saveLayerSet( context.xmlWriter() );
    context.xmlWriter().endElement(); // draw:page
}

bool KarbonDocument::loadOasis( const KoXmlElement &element, KoShapeLoadingContext &context )
{
    // load text styles used by text shapes
    loadOdfStyles( context );
    
    qDeleteAll( d->layers );
    d->layers.clear();
    qDeleteAll( d->objects );
    d->objects.clear();

    const KoXmlElement & pageLayerSet = KoXml::namedItemNS(  element, KoXmlNS::draw, "layer-set" );
    const KoXmlElement & usedPageLayerSet = pageLayerSet.isNull() ? context.odfLoadingContext().stylesReader().layerSet(): pageLayerSet;

    KoXmlElement layerElement;
    forEachElement( layerElement, usedPageLayerSet ) {
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

QRectF KarbonDocument::boundingRect() const
{
    return contentRect().united( QRectF( QPointF(0,0), d->pageSize ) );
}

QRectF KarbonDocument::contentRect() const
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

QSizeF KarbonDocument::pageSize() const
{
    return d->pageSize;
}

void KarbonDocument::setPageSize( QSizeF pageSize )
{
    d->pageSize = pageSize;
}

const QList<KoShape*> KarbonDocument::shapes() const
{
    return d->objects;
}

KoUnit KarbonDocument::unit() const
{
    return d->unit;
}

void KarbonDocument::setUnit( KoUnit unit )
{
    d->unit = unit;
}

const QList<KoShapeLayer*> KarbonDocument::layers() const
{
    return d->layers;
}

KoImageCollection * KarbonDocument::imageCollection()
{
    return dynamic_cast<KoImageCollection *>( d->dataCenterMap["ImageCollection"] );
}

QMap<QString, KoDataCenter*> KarbonDocument::dataCenterMap() const
{
    return d->dataCenterMap;
}

void KarbonDocument::useExternalDataCenterMap( QMap<QString, KoDataCenter*> dataCenters )
{
    qDeleteAll( d->dataCenterMap );
    d->dataCenterMap = dataCenters;
    d->hasExternalDataCenterMap = true;
}

//#############################################################################
// ODF saving
//#############################################################################

bool KarbonDocument::saveOdf( KoDocument::SavingContext &documentContext )
{
    KoStore * store = documentContext.odfStore.store();
    KoXmlWriter* contentWriter = documentContext.odfStore.contentWriter();
    if ( !contentWriter )
        return false;

    KoGenStyles mainStyles;
    KoXmlWriter * bodyWriter = documentContext.odfStore.bodyWriter();

    KoShapeSavingContext shapeContext( *bodyWriter, mainStyles, documentContext.embeddedSaver );

    // save text styles
    saveOdfStyles( shapeContext );
    
    // save page
    KoPageLayout page;
    page.format = KoPageFormat::defaultFormat();
    page.orientation = KoPageFormat::Portrait;
    page.width = pageSize().width();
    page.height = pageSize().height();

    KoGenStyle pageLayout = page.saveOdf();
    QString layoutName = mainStyles.lookup( pageLayout, "PL" );
    KoGenStyle masterPage( KoGenStyle::StyleMaster );
    masterPage.addAttribute( "style:page-layout-name", layoutName );
    mainStyles.lookup( masterPage, "Default", KoGenStyles::DontForceNumbering );

    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:drawing" );

    saveOasis( shapeContext ); // Save contents

    bodyWriter->endElement(); // office:drawing
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfAutomaticStyles( contentWriter, false );

    documentContext.odfStore.closeContentWriter();

    //add manifest line for content.xml
    documentContext.odfStore.manifestWriter()->addManifestEntry( "content.xml", "text/xml" );

    if ( !shapeContext.saveDataCenter( store, documentContext.odfStore.manifestWriter() ) ) {
        return false;
    }

    return mainStyles.saveOdfStylesDotXml( store, documentContext.odfStore.manifestWriter() );
}

void KarbonDocument::loadOdfStyles( KoShapeLoadingContext & context )
{
    KoStyleManager * styleManager = dynamic_cast<KoStyleManager*>( dataCenterMap()["StyleManager"] );
    if( ! styleManager )
        return;
    
    KoTextSharedLoadingData * sharedData = new KoTextSharedLoadingData();
    if( ! sharedData )
        return;
    
    sharedData->loadOdfStyles( context.odfLoadingContext(), styleManager );
    context.addSharedData( KOTEXT_SHARED_LOADING_ID, sharedData );
}

void KarbonDocument::saveOdfStyles( KoShapeSavingContext & context )
{
    KoStyleManager * styleManager = dynamic_cast<KoStyleManager*>( dataCenterMap()["StyleManager"] );
    if( ! styleManager )
        return;
    
    styleManager->saveOdf( context.mainStyles() );
}

void KarbonDocument::addToDataCenterMap(const QString &key, KoDataCenter* dataCenter)
{
    d->dataCenterMap.insert(key, dataCenter);
}
