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

#include "KarbonPart.h"
#include "KarbonFactory.h"
#include "KarbonView.h"
#include <KarbonCanvas.h>

#include <KoApplication.h>
#include <KoDataCenterBase.h>
#include <KoOdfStylesReader.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOasisSettings.h>
#include <KoMainWindow.h>
#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>
#include <KoResourceManager.h>
#include <KoStoreDevice.h>
#include <KoShapePainter.h>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <klocale.h>
#include <KUndoStack>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QFileInfo>
#include <QtCore/QRectF>
#include <QtGui/QPainter>

// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
// static const char * CURRENT_DTD_VERSION = "1.2";

class KarbonPart::Private
{
public:
    Private()
            : showStatusBar(true), merge(false), maxRecentFiles(10) {}

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

    void applyCanvasConfiguration(KarbonCanvas * canvas, KarbonPart * part)
    {
        KSharedConfigPtr config = part->componentData().config();

        uint grabSensitivity = 3;
        uint handleRadius = 3;
        if (config->hasGroup("Misc")) {
            KConfigGroup miscGroup = config->group("Misc");
            grabSensitivity = miscGroup.readEntry("GrabSensitivity", grabSensitivity);
            handleRadius = miscGroup.readEntry("HandleRadius", handleRadius);
        }
        canvas->resourceManager()->setHandleRadius(handleRadius);
        canvas->resourceManager()->setGrabSensitivity(grabSensitivity);

        QColor color(Qt::white);
        if (config->hasGroup("Interface")) {
            color = config->group("Interface").readEntry("CanvasColor", color);
        }
        canvas->setBackgroundColor(color);
    }

    KarbonDocument document;  ///< store non-visual doc info
    bool showStatusBar;       ///< enable/disable status bar in attached view(s)
    bool merge;
    uint maxRecentFiles;      ///< max. number of files shown in open recent menu item
};


KarbonPart::KarbonPart(QWidget* parentWidget, const char* widgetName, QObject* parent, const char* name, bool singleViewMode)
        : KoDocument(parentWidget, parent, singleViewMode), d(new Private())
{
    Q_UNUSED(widgetName);
    d->document.setResourceManager(resourceManager());

    setObjectName(name);
    setComponentData(KarbonFactory::componentData(), false);
    setTemplateType("karbon_template");
    resourceManager()->setUndoStack(undoStack());

    initConfig();

    // set as default paper
    KoPageLayout pl = pageLayout();
    pl.format = KoPageFormat::defaultFormat();
    pl.orientation = KoPageFormat::Portrait;
    pl.width = MM_TO_POINT(KoPageFormat::width(pl.format, pl.orientation));
    pl.height = MM_TO_POINT(KoPageFormat::height(pl.format, pl.orientation));
    setPageLayout(pl);
}

KarbonPart::~KarbonPart()
{
    delete d;
}

void KarbonPart::setPageLayout(const KoPageLayout& layout)
{
    KoDocument::setPageLayout(layout);
    setPageSize(QSizeF(layout.width, layout.height));
}

KoView* KarbonPart::createViewInstance(QWidget* parent)
{
    KarbonView *result = new KarbonView(this, parent);

    KoResourceManager * provider = result->canvasWidget()->resourceManager();
    provider->setResource(KoCanvasResource::PageSize, d->document.pageSize());

    d->applyCanvasConfiguration(result->canvasWidget(), this);

    return result;
}

void KarbonPart::removeView(KoView *view)
{
    kDebug(38000) << "KarbonPart::removeView";
    KoDocument::removeView(view);
}

void KarbonPart::openTemplate(const KUrl& url)
{
    KoDocument::openTemplate(url);

    // explicitly set the output mimetype to our native mimetype
    // so that autosaving works for not yet saved templates as well
    if (outputMimeType().isEmpty())
        setOutputMimeType("application/vnd.oasis.opendocument.graphics");
}

bool KarbonPart::loadXML(const KoXmlDocument&, KoStore*)
{
    return false;
}

bool KarbonPart::loadOdf(KoOdfReadStore & odfStore)
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

    d->document.loadOasis(page, shapeContext);

    if (d->document.pageSize().isEmpty()) {
        QSizeF pageSize = d->document.contentRect().united(QRectF(0, 0, 1, 1)).size();
        setPageSize(pageSize);
    }

    loadOasisSettings(odfStore.settingsDoc());

    return true;
}

bool KarbonPart::completeLoading(KoStore* store)
{
    bool ok = true;
    foreach(KoDataCenterBase *dataCenter, dataCenterMap()) {
        ok = ok && dataCenter->completeLoading(store);
    }
    return ok;
}

void KarbonPart::loadOasisSettings(const KoXmlDocument & settingsDoc)
{
    if (settingsDoc.isNull())
        return ; // not an error if some file doesn't have settings.xml
    KoOasisSettings settings(settingsDoc);
    KoOasisSettings::Items viewSettings = settings.itemSet("view-settings");
    if (!viewSettings.isNull()) {
        setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
        // FIXME: add other config here.
    }
    guidesData().loadOdfSettings(settingsDoc);
    gridData().loadOdfSettings(settingsDoc);
}

void KarbonPart::saveOasisSettings(KoStore * store)
{
    KoStoreDevice settingsDev(store);
    KoXmlWriter * settingsWriter = KoOdfWriteStore::createOasisXmlWriter(&settingsDev, "office:document-settings");

    // add this so that OOo reads guides lines and grid data from ooo:view-settings
    settingsWriter->addAttribute("xmlns:ooo", "http://openoffice.org/2004/office");

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

bool KarbonPart::saveOdf(SavingContext &documentContext)
{
    if (! d->document.saveOdf(documentContext, pageLayout()))
        return false;

    KoStore * store = documentContext.odfStore.store();
    if (! store->open("settings.xml"))
        return false;

    saveOasisSettings(store);

    if (! store->close())
        return false;

    documentContext.odfStore.manifestWriter()->addManifestEntry("settings.xml", "text/xml");

    setModified(false);

    return true;
}

void KarbonPart::slotDocumentRestored()
{
    setModified(false);
}

void KarbonPart::paintContent(QPainter &painter, const QRect& rect)
{
    KoShapePainter shapePainter;
    shapePainter.setShapes(d->document.shapes());
    shapePainter.paint(painter, rect, QRectF(QPointF(), d->document.pageSize()));
}

KarbonDocument& KarbonPart::document()
{
    return d->document;
}

bool KarbonPart::showStatusBar() const
{
    return d->showStatusBar;
}

void KarbonPart::setShowStatusBar(bool b)
{
    d->showStatusBar = b;
}

uint KarbonPart::maxRecentFiles() const
{
    return d->maxRecentFiles;
}

void KarbonPart::reorganizeGUI()
{
    foreach(KoView* view, views()) {
        KarbonView * kv = qobject_cast<KarbonView*>(view);
        if (kv) {
            kv->reorganizeGUI();
            d->applyCanvasConfiguration(kv->canvasWidget(), this);
        }
    }
}

void KarbonPart::initConfig()
{
    KSharedConfigPtr config = KarbonPart::componentData().config();

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
    QString defaultUnit = "cm";
    if (KGlobal::locale()->measureSystem() == KLocale::Imperial)
        defaultUnit = "in";

    if (config->hasGroup("Misc")) {
        KConfigGroup miscGroup = config->group("Misc");
        undos = miscGroup.readEntry("UndoRedo", -1);
        defaultUnit = miscGroup.readEntry("Units", defaultUnit);
    }
    undoStack()->setUndoLimit(undos);
    setUnit(KoUnit::unit(defaultUnit));

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

bool KarbonPart::mergeNativeFormat(const QString &file)
{
    d->merge = true;
    bool result = loadNativeFormat(file);
    if (!result)
        showLoadingErrorDialog();
    d->merge = false;
    return result;
}

void KarbonPart::addShape(KoShape* shape)
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();

    KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
    if (layer) {
        d->document.insertLayer(layer);
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
            else if (d->document.layers().count())
                activeLayer = d->document.layers().first();

            if (activeLayer)
                activeLayer->addShape(shape);
        }

        d->document.add(shape);

        foreach(KoView *view, views()) {
            KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
            canvas->shapeManager()->addShape(shape);
        }
    }

    setModified(true);
}

void KarbonPart::removeShape(KoShape* shape)
{
    KoShapeLayer *layer = dynamic_cast<KoShapeLayer*>(shape);
    if (layer) {
        d->document.removeLayer(layer);
    } else {
        d->document.remove(shape);
        foreach(KoView *view, views()) {
            KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
            canvas->shapeManager()->remove(shape);
        }
    }
    setModified(true);
}

QMap<QString, KoDataCenterBase*> KarbonPart::dataCenterMap() const
{
    return d->document.dataCenterMap();
}

void KarbonPart::setPageSize(const QSizeF &pageSize)
{
    d->document.setPageSize(pageSize);
    foreach(KoView *view, views()) {
        KarbonCanvas *canvas = ((KarbonView*)view)->canvasWidget();
        canvas->resourceManager()->setResource(KoCanvasResource::PageSize, pageSize);
    }
}

#include "KarbonPart.moc"

