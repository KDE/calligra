/* This file is part of the KDE project
 * Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2001-2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002-2006 Laurent Montel <montel@kde.org>
 * Copyright (C) 2002 Werner Trobin <trobin@kde.org>
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2002 Stephan Kulow <coolo@kde.org>
 * Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * Copyright (C) 2003 Thomas Nagy <tnagyemail-mail@yahoo.fr>
 * Copyright (C) 2003,2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>
 * Copyright (C) 2004-2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005 Sven Langkamp <sven.langkamp@gmail.com>
 * Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2005-2013 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2005 Johannes Schaub <johannes.schaub@kdemail.net>
 * Copyright (C) 2006 Gabor Lehel <illissius@gmail.com>
 * Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * Copyright (C) 2006 Jaison Lee <lee.jaison@gmail.com>
 * Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
 * Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 * Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
 * Copyright (C) 2012 Yue Liu <yue.liu@mail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KarbonDocument.h"
#include "KarbonPart.h"
#include "KarbonFactory.h"
#include "KarbonView.h"
#include <KarbonCanvas.h>
#include <KarbonDocument.h>

#include <KoApplication.h>
#include <KoImageCollection.h>
#include <KoDataCenterBase.h>
#include <KoText.h>
#include <KoStyleManager.h>
#include <KoTextSharedLoadingData.h>
#include <KoOdfStylesReader.h>
#include <KoOdfStyleManager.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoPageLayout.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOasisSettings.h>
#include <KoMainWindow.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoCanvasResourceManager.h>
#include <KoDocumentResourceManager.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoStoreDevice.h>
#include <KoStore.h>
#include <KoShapePainter.h>
#include <SvgShapeFactory.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ktemporaryfile.h>
#include <kdebug.h>
#include <klocale.h>
#include <kundo2stack.h>

#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QRectF>
#include <QPainter>

// Make sure an appropriate DTD is available in www/calligra/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

class KarbonDocument::Private
{
public:
    Private()
            : pageSize(0.0, 0.0),
              hasExternalDataCenterMap(false),
              showStatusBar(true),
              merge(false),
              maxRecentFiles(10)
    {}

    ~Private()
    {
        qDeleteAll(layers);
        layers.clear();
        qDeleteAll(objects);
        objects.clear();
        if (!hasExternalDataCenterMap)
            qDeleteAll(dataCenterMap);
    }

    qreal getAttribute(KoXmlElement &element, const char *attributeName, qreal defaultValue)
    {
        QString value = element.attribute(attributeName);
        if (! value.isEmpty())
            return value.toDouble();
        else
            return defaultValue;
    }

    int getAttribute(KoXmlElement &element, const char *attributeName, int defaultValue)
    {
        QString value = element.attribute(attributeName);
        if (! value.isEmpty())
            return value.toInt();
        else
            return defaultValue;
    }

    // KarbonDocument document;  ///< store non-visual doc info
    QSizeF pageSize; ///< the documents page size

    QList<KoShape*> objects;     ///< The list of all object of the document.
    QList<KoShapeLayer*> layers; ///< The layers in this document.

    QMap<QString, KoDataCenterBase*> dataCenterMap;
    bool hasExternalDataCenterMap;
    bool showStatusBar;       ///< enable/disable status bar in attached view(s)
    bool merge;
    uint maxRecentFiles;      ///< max. number of files shown in open recent menu item

    // For common styles (a.k.a. named styles)
    KoOdfStyleManager styleManager;
};


KarbonDocument::KarbonDocument(KarbonPart* part)
        : KoDocument(part)
        , d(new Private())
{
    resourceManager()->setUndoStack(undoStack());

    initConfig();

    SvgShapeFactory::addToRegistry();

    // set as default paper
    KoPageLayout pl = pageLayout();
    pl.format = KoPageFormat::defaultFormat();
    pl.orientation = KoPageFormat::Portrait;
    pl.width = MM_TO_POINT(KoPageFormat::width(pl.format, pl.orientation));
    pl.height = MM_TO_POINT(KoPageFormat::height(pl.format, pl.orientation));
    setPageLayout(pl);
}

KarbonDocument::~KarbonDocument()
{
    delete d;
}

void KarbonDocument::setPageLayout(const KoPageLayout& layout)
{
    KoDocument::setPageLayout(layout);
    setPageSize(QSizeF(layout.width, layout.height));
}

bool KarbonDocument::loadXML(const KoXmlDocument&, KoStore*)
{
    return false;
}

bool KarbonDocument::loadOdf(KoOdfReadStore & odfStore)
{
    kDebug(38000) << "Start loading OASIS document..." /*<< doc.toString()*/;

    KoXmlElement contents = odfStore.contentDoc().documentElement();
    kDebug(38000) << "Start loading OASIS document..." << contents.text();
    kDebug(38000) << "Start loading OASIS contents..." << contents.lastChild().localName();
    kDebug(38000) << "Start loading OASIS contents..." << contents.lastChild().namespaceURI();
    kDebug(38000) << "Start loading OASIS contents..." << contents.lastChild().isElement();
    KoXmlElement body(KoXml::namedItemNS(contents, KoXmlNS::office, "body"));
    if (body.isNull()) {
        kDebug(38000) << "No office:body found!";
        setErrorMessage(i18n("Invalid OASIS document. No office:body tag found."));
        return false;
    }

    body = KoXml::namedItemNS(body, KoXmlNS::office, "drawing");
    if (body.isNull()) {
        kDebug(38000) << "No office:drawing found!";
        setErrorMessage(i18n("Invalid OASIS document. No office:drawing tag found."));
        return false;
    }

    KoXmlElement page(KoXml::namedItemNS(body, KoXmlNS::draw, "page"));
    if (page.isNull()) {
        kDebug(38000) << "No office:drawing found!";
        setErrorMessage(i18n("Invalid OASIS document. No draw:page tag found."));
        return false;
    }

    KoXmlElement * master = 0;
    if (odfStore.styles().masterPages().contains("Standard"))
        master = odfStore.styles().masterPages().value("Standard");
    else if (odfStore.styles().masterPages().contains("Default"))
        master = odfStore.styles().masterPages().value("Default");
    else if (! odfStore.styles().masterPages().empty())
        master = odfStore.styles().masterPages().begin().value();

    if (master) {
        const QString pageStyleName = master->attributeNS(KoXmlNS::style, "page-layout-name", QString());
        const KoXmlElement *style = odfStore.styles().findStyle(pageStyleName);
        if (style) {
            KoPageLayout layout;
            layout.loadOdf(*style);
            setPageLayout(layout);
        }
    } else {
        kWarning() << "No master page found!";
        return false;
    }

    KoOdfLoadingContext context(odfStore.styles(), odfStore.store());
    KoShapeLoadingContext shapeContext(context, resourceManager());

    loadOasis(page, shapeContext);

    if (pageSize().isEmpty()) {
        QSizeF pageSize = contentRect().united(QRectF(0, 0, 1, 1)).size();
        setPageSize(pageSize);
    }

    loadOasisSettings(odfStore.settingsDoc());

    return true;
}

bool KarbonDocument::completeLoading(KoStore* store)
{
    bool ok = true;
    foreach(KoDataCenterBase *dataCenter, dataCenterMap()) {
        ok = ok && dataCenter->completeLoading(store);
    }
    return ok;
}

void KarbonDocument::loadOasisSettings(const KoXmlDocument & settingsDoc)
{
    if (settingsDoc.isNull())
        return ; // not an error if some file doesn't have settings.xml
    KoOasisSettings settings(settingsDoc);
    KoOasisSettings::Items viewSettings = settings.itemSet("view-settings");
    if (!viewSettings.isNull()) {
        setUnit(KoUnit::fromSymbol(viewSettings.parseConfigItemString("unit")));
        // FIXME: add other config here.
    }
    guidesData().loadOdfSettings(settingsDoc);
    gridData().loadOdfSettings(settingsDoc);
}

void KarbonDocument::saveOasisSettings(KoStore * store)
{
    KoStoreDevice settingsDev(store);
    KoXmlWriter * settingsWriter = KoOdfWriteStore::createOasisXmlWriter(&settingsDev, "office:document-settings");

    settingsWriter->startElement("office:settings");
    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "view-settings");

    saveUnitOdf(settingsWriter);

    settingsWriter->endElement(); // config:config-item-set

    settingsWriter->startElement("config:config-item-set");
    settingsWriter->addAttribute("config:name", "ooo:view-settings");
    settingsWriter->startElement("config:config-item-map-indexed");
    settingsWriter->addAttribute("config:name", "Views");
    settingsWriter->startElement("config:config-item-map-entry");

    guidesData().saveOdfSettings(*settingsWriter);
    gridData().saveOdfSettings(*settingsWriter);

    settingsWriter->endElement(); // config:config-item-map-entry
    settingsWriter->endElement(); // config:config-item-map-indexed
    settingsWriter->endElement(); // config:config-item-set

    settingsWriter->endElement(); // office:settings
    settingsWriter->endElement(); // office:document-settings

    settingsWriter->endDocument();

    delete settingsWriter;
}

bool KarbonDocument::saveOdf(SavingContext &documentContext)
{
    KoStore * store = documentContext.odfStore.store();
    KoXmlWriter* contentWriter = documentContext.odfStore.contentWriter();
    if (!contentWriter)
        return false;

    KoGenStyles mainStyles;
    KoXmlWriter * bodyWriter = documentContext.odfStore.bodyWriter();

    KoShapeSavingContext shapeContext(*bodyWriter, mainStyles, documentContext.embeddedSaver);

    // save text styles
    saveOdfStyles(shapeContext);

    // save page
    QString layoutName = mainStyles.insert(pageLayout().saveOdf(), "PL");
    KoGenStyle masterPage(KoGenStyle::MasterPageStyle);
    masterPage.addAttribute("style:page-layout-name", layoutName);
    mainStyles.insert(masterPage, "Default", KoGenStyles::DontAddNumberToName);

    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:drawing");

    saveOasis(shapeContext);   // Save contents

    bodyWriter->endElement(); // office:drawing
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);

    documentContext.odfStore.closeContentWriter();

    //add manifest line for content.xml
    documentContext.odfStore.manifestWriter()->addManifestEntry("content.xml", "text/xml");

    if (!shapeContext.saveDataCenter(store, documentContext.odfStore.manifestWriter())) {
        return false;
    }

    if (! mainStyles.saveOdfStylesDotXml(store, documentContext.odfStore.manifestWriter()))
        return false;

    if (! store->open("settings.xml"))
        return false;

    saveOasisSettings(store);

    if (! store->close())
        return false;

    documentContext.odfStore.manifestWriter()->addManifestEntry("settings.xml", "text/xml");

    setModified(false);

    return true;
}

void KarbonDocument::slotDocumentRestored()
{
    setModified(false);
}

void KarbonDocument::paintContent(QPainter &painter, const QRect& rect)
{
    KoShapePainter shapePainter;
    shapePainter.setShapes(shapes());
    shapePainter.paint(painter, rect, QRectF(QPointF(), pageSize()));
}

bool KarbonDocument::showStatusBar() const
{
    return d->showStatusBar;
}

void KarbonDocument::setShowStatusBar(bool b)
{
    d->showStatusBar = b;
}

uint KarbonDocument::maxRecentFiles() const
{
    return d->maxRecentFiles;
}

void KarbonDocument::reorganizeGUI()
{
    foreach(KoView* view, documentPart()->views()) {
        KarbonView * kv = qobject_cast<KarbonView*>(view);
        if (kv) {
            kv->reorganizeGUI();
            emit applyCanvasConfiguration(kv->canvasWidget());
        }
    }
}

void KarbonDocument::initConfig()
{
    KSharedConfigPtr config = KarbonFactory::componentData().config();

    // disable grid by default
    gridData().setShowGrid(false);

    if (config->hasGroup("Interface")) {
        KConfigGroup interfaceGroup = config->group("Interface");
        setAutoSave(interfaceGroup.readEntry("AutoSave", defaultAutoSave() / 60) * 60);
        d->maxRecentFiles = interfaceGroup.readEntry("NbRecentFile", 10);
        setShowStatusBar(interfaceGroup.readEntry("ShowStatusBar" , true));
        setBackupFile(interfaceGroup.readEntry("BackupFile", true));
    }
    int undos = 30;

    QString defaultUnitSymbol =
        QLatin1String((KGlobal::locale()->measureSystem() == KLocale::Imperial)?"in":"cm");

    if (config->hasGroup("Misc")) {
        KConfigGroup miscGroup = config->group("Misc");
        undos = miscGroup.readEntry("UndoRedo", -1);
        defaultUnitSymbol = miscGroup.readEntry("Units", defaultUnitSymbol);
    }
    undoStack()->setUndoLimit(undos);
    setUnit(KoUnit::fromSymbol(defaultUnitSymbol));

    if (config->hasGroup("Grid")) {
        KoGridData defGrid;
        KConfigGroup gridGroup = config->group("Grid");
        qreal spacingX = gridGroup.readEntry<qreal>("SpacingX", defGrid.gridX());
        qreal spacingY = gridGroup.readEntry<qreal>("SpacingY", defGrid.gridY());
        gridData().setGrid(spacingX, spacingY);
        QColor color = gridGroup.readEntry("Color", defGrid.gridColor());
        gridData().setGridColor(color);
    }
}

bool KarbonDocument::mergeNativeFormat(const QString &file)
{
    d->merge = true;
    bool result = loadNativeFormat(file);
    if (!result)
        documentPart()->showLoadingErrorDialog();
    d->merge = false;
    return result;
}

void KarbonDocument::addShape(KoShape* shape)
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();

    KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
    if (layer) {
        insertLayer(layer);
        if (canvasController) {
            KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
            selection->setActiveLayer(layer);
        }
    } else {
        // only add shape to active layer if it has no parent yet
        if (! shape->parent()) {
            kDebug(38000) << "shape has no parent, adding to the active layer!";
            KoShapeLayer *activeLayer = 0;
            if (canvasController)
                activeLayer = canvasController->canvas()->shapeManager()->selection()->activeLayer();
            else if (layers().count())
                activeLayer = layers().first();

            if (activeLayer)
                activeLayer->addShape(shape);
        }

        add(shape);

        foreach(KoView *view, documentPart()->views()) {
            KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
            canvas->shapeManager()->addShape(shape);
        }
    }

    setModified(true);
    emit shapeCountChanged();
}

void KarbonDocument::removeShape(KoShape* shape)
{
    KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
    if (layer) {
        removeLayer(layer);
    } else {
        remove(shape);
        foreach(KoView *view, documentPart()->views()) {
            KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
            canvas->shapeManager()->remove(shape);
        }
    }
    setModified(true);
    emit shapeCountChanged();
}

void KarbonDocument::setPageSize(const QSizeF &pageSize)
{
    d->pageSize = pageSize;
    foreach(KoView *view, documentPart()->views()) {
        KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
        canvas->resourceManager()->setResource(KoCanvasResourceManager::PageSize, pageSize);
    }
}

QSizeF KarbonDocument::pageSize() const
{
    return d->pageSize;
}

void KarbonDocument::insertLayer(KoShapeLayer* layer)
{
    if (!d->layers.contains(layer)) {
        if (d->layers.count()) {
            layer->setZIndex(d->layers.last()->zIndex() + 1);
        } else {
            layer->setZIndex(d->layers.count());
        }
        d->layers.append(layer);
    }
}

void KarbonDocument::removeLayer(KoShapeLayer* layer)
{
    d->layers.removeAt(d->layers.indexOf(layer));
    if (d->layers.count() == 0)
        d->layers.append(new KoShapeLayer());
}

bool KarbonDocument::canRaiseLayer(KoShapeLayer* layer)
{
    int pos = d->layers.indexOf(layer);
    return (pos != int(d->layers.count()) - 1 && pos >= 0);
}

bool KarbonDocument::canLowerLayer(KoShapeLayer* layer)
{
    int pos = d->layers.indexOf(layer);
    return (pos > 0);
}

void KarbonDocument::raiseLayer(KoShapeLayer* layer)
{
    int pos = d->layers.indexOf(layer);
    if (pos != int(d->layers.count()) - 1 && pos >= 0) {
        KoShapeLayer * layerAbove = d->layers.at(pos + 1);
        int lowerZIndex = layer->zIndex();
        int upperZIndex = layerAbove->zIndex();
        layer->setZIndex(upperZIndex);
        layerAbove->setZIndex(lowerZIndex);
        d->layers.move(pos, pos + 1);
    }
}

void KarbonDocument::lowerLayer(KoShapeLayer* layer)
{
    int pos = d->layers.indexOf(layer);
    if (pos > 0) {
        KoShapeLayer * layerBelow = d->layers.at(pos - 1);
        int upperZIndex = layer->zIndex();
        int lowerZIndex = layerBelow->zIndex();
        layer->setZIndex(lowerZIndex);
        layerBelow->setZIndex(upperZIndex);
        d->layers.move(pos, pos - 1);
    }
}

int KarbonDocument::layerPos(KoShapeLayer* layer)
{
    return d->layers.indexOf(layer);
}

void KarbonDocument::add(KoShape* shape)
{
    if (! d->objects.contains(shape))
        d->objects.append(shape);
}

void KarbonDocument::remove(KoShape* shape)
{
    d->objects.removeAt(d->objects.indexOf(shape));
}

void KarbonDocument::saveOasis(KoShapeSavingContext & context) const
{
    context.xmlWriter().startElement("draw:page");
    context.xmlWriter().addAttribute("draw:name", "");
    context.xmlWriter().addAttribute("draw:id", "page1");
    context.xmlWriter().addAttribute("xml:id", "page1");
    context.xmlWriter().addAttribute("draw:master-page-name", "Default");

    foreach(KoShapeLayer * layer, d->layers) {
        context.addLayerForSaving(layer);
    }
    context.saveLayerSet(context.xmlWriter());

    foreach(KoShapeLayer * layer, d->layers) {
        layer->saveOdf(context);
    }

    context.xmlWriter().endElement(); // draw:page
}

bool KarbonDocument::loadOasis(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // load text styles used by text shapes
    loadOdfStyles(context);

    qDeleteAll(d->layers);
    d->layers.clear();
    qDeleteAll(d->objects);
    d->objects.clear();

    const KoXmlElement & pageLayerSet = KoXml::namedItemNS(element, KoXmlNS::draw, "layer-set");
    const KoXmlElement & usedPageLayerSet = pageLayerSet.isNull() ? context.odfLoadingContext().stylesReader().layerSet() : pageLayerSet;

    KoXmlElement layerElement;
    forEachElement(layerElement, usedPageLayerSet) {
        KoShapeLayer * l = new KoShapeLayer();
        if (l->loadOdf(layerElement, context))
            insertLayer(l);
    }

    KoShapeLayer * defaultLayer = 0;

    // check if we have to insert a default layer
    if (d->layers.count() == 0)
        defaultLayer = new KoShapeLayer();

    KoXmlElement child;
    forEachElement(child, element) {
        kDebug(38000) << "loading shape" << child.localName();

        KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf(child, context);
        if (shape)
            d->objects.append(shape);
    }

    // add all toplevel shapes to the default layer
    foreach(KoShape * shape, d->objects) {
        if (! shape->parent()) {
            if (! defaultLayer)
                defaultLayer = new KoShapeLayer();

            defaultLayer->addShape(shape);
        }
    }

    if (defaultLayer)
        insertLayer(defaultLayer);

    KoOdfStylesReader & styleReader = context.odfLoadingContext().stylesReader();
    QHash<QString, KoXmlElement*> masterPages = styleReader.masterPages();

    KoXmlElement * master = 0;
    if( masterPages.contains( "Standard" ) )
        master = masterPages.value( "Standard" );
    else if( masterPages.contains( "Default" ) )
        master = masterPages.value( "Default" );
    else if( ! masterPages.empty() )
        master = masterPages.begin().value();

    if (master) {
        context.odfLoadingContext().setUseStylesAutoStyles( true );

        QList<KoShape*> masterPageShapes;
        KoXmlElement child;
        forEachElement(child, (*master)) {
            kDebug(38000) <<"loading master page shape" << child.localName();
            KoShape * shape = KoShapeRegistry::instance()->createShapeFromOdf( child, context );
            if( shape )
                masterPageShapes.append( shape );
        }

        KoShapeLayer * masterPageLayer = 0;
        // add all toplevel shapes to the master page layer
        foreach(KoShape * shape, masterPageShapes) {
            d->objects.append( shape );
            if(!shape->parent()) {
                if( ! masterPageLayer ) {
                    masterPageLayer = new KoShapeLayer();
                    masterPageLayer->setName(i18n("Master Page"));
                }

                masterPageLayer->addShape(shape);
            }
        }

        if( masterPageLayer )
            insertLayer( masterPageLayer );

        context.odfLoadingContext().setUseStylesAutoStyles( false );
    }

    return true;
}

QRectF KarbonDocument::boundingRect() const
{
    return contentRect().united(QRectF(QPointF(0, 0), d->pageSize));
}

QRectF KarbonDocument::contentRect() const
{
    QRectF bb;
    foreach(KoShape* layer, d->layers) {
        if (bb.isNull())
            bb = layer->boundingRect();
        else
            bb = bb.united(layer->boundingRect());
    }

    return bb;
}

const QList<KoShape*> KarbonDocument::shapes() const
{
    return d->objects;
}

const QList<KoShapeLayer*> KarbonDocument::layers() const
{
    return d->layers;
}

KoImageCollection * KarbonDocument::imageCollection()
{
    return resourceManager()->imageCollection();
}

QMap<QString, KoDataCenterBase*> KarbonDocument::dataCenterMap() const
{
    return d->dataCenterMap;
}

void KarbonDocument::useExternalDataCenterMap(QMap<QString, KoDataCenterBase*> dataCenters)
{
    qDeleteAll(d->dataCenterMap);
    d->dataCenterMap = dataCenters;
    d->hasExternalDataCenterMap = true;
}

void KarbonDocument::loadOdfStyles(KoShapeLoadingContext & context)
{
    // Common styles (named styles) in general
    KoOdfLoadingContext &odfLoadingContext = context.odfLoadingContext();
    d->styleManager.loadStyles(odfLoadingContext.store());

    // Only text styles (old style system).
    KoStyleManager *styleManager = resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();

    if (! styleManager)
        return;

    KoTextSharedLoadingData * sharedData = new KoTextSharedLoadingData();
    if (! sharedData)
        return;

    sharedData->loadOdfStyles(context, styleManager);
    context.addSharedData(KOTEXT_SHARED_LOADING_ID, sharedData);
}

void KarbonDocument::saveOdfStyles(KoShapeSavingContext & context)
{
    KoStyleManager * styleManager = dynamic_cast<KoStyleManager*>(dataCenterMap()["StyleManager"]);
    if (! styleManager)
        return;

    styleManager->saveOdf(context);
}

void KarbonDocument::addToDataCenterMap(const QString &key, KoDataCenterBase* dataCenter)
{
    d->dataCenterMap.insert(key, dataCenter);
}

