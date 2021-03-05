/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "TableShape.h"

#include "TablePageManager.h"
#include "DocBase.h"

#include <QPainter>

#include <KGuiItem>
#include <KMessageBox>
#include <KAboutData>

#include <KoShapeContainer.h>
#include <KoXmlNS.h>
#include <KoStore.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoEmbeddedDocumentSaver.h>
#include <calligraversion.h>
#include <KoComponentData.h>
#include <KoUnit.h>
#include <KoDpi.h>
#include <KoDocumentResourceManager.h>
#include <KoPart.h>
#include <KoMainWindow.h>
#include <KoDocumentBase.h>

#include <SheetsDebug.h>
#include <CellView.h>
#include <Damages.h>
#include <Condition.h>
#include <Map.h>
#include <PrintSettings.h>
#include <Region.h>
#include <RowColumnFormat.h>
#include <RowFormatStorage.h>
#include <Sheet.h>
#include <SheetView.h>
#include <Value.h>
#include <odf/SheetsOdf.h>
#include <FunctionModuleRegistry.h>
#include <Doc.h>
#include <Part.h>

// Define the protocol used here for embedded documents' URL
// This used to "store" but KUrl didn't like it,
// so let's simply make it "tar" !
#define STORE_PROTOCOL "tar"
#define INTERNAL_PROTOCOL "intern"

namespace Calligra {
namespace Sheets {

class TableShapePart : public KoPart
{
public:
    TableShapePart() : KoPart(KoComponentData(KAboutData(QStringLiteral("spreadsheet"), QStringLiteral("Spreadsheet"),
                               QStringLiteral(CALLIGRA_VERSION_STRING))), nullptr)
    {}

    KoMainWindow *createMainWindow() override {
        return nullptr;//new KoMainWindow("application/vnd.oasis.opendocument.spreadsheet", componentData());
    }
protected:
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override {
        Q_UNUSED(document)
        Q_UNUSED(parent)
        return nullptr;
    }
};

class TableShapeDoc : public DocBase
{
public:
    TableShapeDoc() : DocBase(new TableShapePart()) {
        FunctionModuleRegistry::instance()->loadFunctionModules();
    }
    void initConfig() override {}

};
}
}

using namespace Calligra::Sheets;

class TableShape::Private
{
public:
    Private(TableShape *parent) : q(parent), paintingDisabled(false) {}
    TableShape *q;
    Part part;
    KoDocumentResourceManager *resourceManager;
    KoDocumentBase *parentDoc;
    QPointF topLeftOffset;
    SheetView*  sheetView;
    bool        isMaster;
    TablePageManager* pageManager;
    Sheet* currentSheet;
    QUrl url; // the docs url
    bool paintingDisabled;

public:
    QRect visibleCells() const;
};

QRect TableShape::Private::visibleCells() const
{
    const Sheet *sheet = sheetView->sheet();
    QRectF rect(topLeftOffset, q->size());
    qreal tmp;
    QRect visibleCells;
    visibleCells.setLeft(sheet->leftColumn(rect.left(), tmp));
    visibleCells.setRight(sheet->rightColumn(rect.right())+1);
    visibleCells.setTop(sheet->topRow(rect.top(), tmp));
    visibleCells.setBottom(sheet->bottomRow(rect.bottom())+1);
    return visibleCells;
}

TableShape::TableShape(KoDocumentResourceManager *resourceManager, KoDocumentBase *parentDoc, int firstColumn, int firstRow, int columns, int rows)
    : KoFrameShape(KoXmlNS::draw, "object")
    , d(new Private(this))
{
    Q_UNUSED(firstColumn)
    Q_UNUSED(firstRow)
    Q_UNUSED(columns)
    Q_UNUSED(rows)
    setObjectName(QLatin1String("TableShape"));
    debugSheetsTableShape<<this;
    d->resourceManager = resourceManager;
    d->parentDoc = parentDoc;
    d->sheetView = nullptr;
    d->isMaster = false;
    d->pageManager = nullptr;
    d->currentSheet = nullptr;
    Doc *doc = new Doc(&d->part);
    d->part.setDocument(doc);
    if (parentDoc) {
        parentDoc->setEmbeddedDocument(doc);
    }
    setMap();
}

TableShape::~TableShape()
{
    delete d->pageManager;
    delete d->sheetView;
    delete d;
}

KoDocumentResourceManager *TableShape::resourceManager() const
{
    return d->resourceManager;
}

Doc *TableShape::document() const
{
    return dynamic_cast<Doc*>(d->part.document());
}

void TableShape::paint(QPainter& painter, const KoViewConverter& converter, KoShapePaintingContext &)
{
    if (d->paintingDisabled) {
        return;
    }
#ifndef NDEBUG
    if (KoShape::parent()) {
        debugSheetsTableShape << KoShape::parent()->name() <<  KoShape::parent()->shapeId() << KoShape::parent()->boundingRect();
    }
#endif

    applyConversion(painter, converter);
    const QRectF clipRect = QRectF(QPointF(), size());
    painter.setClipRect(clipRect, Qt::IntersectClip);

    // painting cell contents
    d->sheetView->setViewConverter(&converter);
    paintCells(painter);
}

void TableShape::paintCells(QPainter& painter)
{
    const Sheet *sheet = d->sheetView->sheet();
    const QRectF paintRect = QRectF(QPointF(), size());
    const QRect cells = visibleCells();
    const QPointF topLeft(sheet->columnPosition(cells.left()), sheet->rowPosition(cells.top()));
    painter.translate(-d->topLeftOffset);
    d->sheetView->paintCells(painter, paintRect, topLeft, nullptr, cells);
}

bool TableShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    debugSheetsTableShape << "loadOdf:"<<element.localName();
    loadOdfAttributes(element, context, OdfAllAttributes);
    bool r = loadOdfFrame(element, context);
    setMap();
    return r;
}

bool TableShape::loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    debugSheetsTableShape << "loadOdfFrameElement:"<<element.tagName();
    if (element.tagName() == "object") {
        return loadEmbeddedDocument(context.odfLoadingContext().store(),
                                    element,
                                    context);
    }
    warnSheetsTableShape << "Unknown frame element <" << element.tagName() << ">";
    return false;
}

bool TableShape::loadEmbeddedDocument(KoStore *store, const KoXmlElement &objectElement, KoShapeLoadingContext &context)
{
    debugSheetsTableShape<<"loadEmbeddedDocument:"<<objectElement.localName();
    if (!objectElement.hasAttributeNS(KoXmlNS::xlink, "href")) {
        errorSheetsTableShape << "Object element has no valid xlink:href attribute";
        return false;
    }
    QString href = objectElement.attributeNS(KoXmlNS::xlink, "href");
    debugSheetsTableShape<<"loadEmbeddedDocument:"<<"href="<<href;
    // It can happen that the url is empty e.g. when it is a
    // presentation:placeholder.
    if (href.isEmpty()) {
        return true;
    }
    if (href[0] == '#') {
        href.remove(0, 1);
    }
    // href can be:
    // 1. relative inside this store, e.g: ./Object1
    // 2. releative path to the outside filesystem, e.g: ../extern.ods
    //    The first .. is just to get out of this store so in this case
    //    extern.ods is in the same directory as store
    // 3. absolute path to the filesystem
    // 4. relative path to internet, e.g: TODO
    // 5. absolute path to the internet: https://here.we/are/extern.ods
    d->url = QUrl(href);
    if (d->url.isRelative()) {
        if (d->url.path().startsWith(QLatin1String("./"))) {
            d->url.setPath(d->url.path().remove(0, 2));
            d->url.setScheme(QStringLiteral(INTERNAL_PROTOCOL));
        } else if (href.startsWith("..")) {
            document()->setUrl(d->url);
            // relative urls are relative this package, so the firts ../ must be removed
            d->url.setPath(d->url.path().remove(0, 3));
            // get the url from the document we are embedded in and prepend out relative  one
            KoDocumentBase *doc = context.documentResourceManager()->odfDocument();
            d->url = doc->url().adjusted(QUrl::RemoveFilename).resolved(QUrl(d->url));
        } else {
            // hmmm, taken from ChartShape, which case is this?
            d->url.setScheme(QStringLiteral(INTERNAL_PROTOCOL));
        }
    } else {
        // absolute url
        document()->setUrl(d->url);
    }

    debugSheetsTableShape << "url=" << d->url;

    bool res = true;
    // hmmm, what about the STORE_PROTOCOL (tar), when does that happen? (ChartShape asserts)
    if (d->url.scheme() == QStringLiteral(INTERNAL_PROTOCOL) || d->url.scheme() == QStringLiteral(STORE_PROTOCOL)) {
        document()->setStoreInternal(true);
        store->pushDirectory();
        store->enterDirectory(d->url.path());
        debugSheetsTableShape<<"loadEmbeddedDocument: load from store"<<store->currentPath();
        res = document()->loadOasisFromStore(store);
        debugSheetsTableShape<<"loadEmbeddedDocument: loaded"<<document()->map();
        store->popDirectory();
    } else {
        // Reference to an external document.
        document()->setStoreInternal(false);
        if (d->url.isLocalFile()) {
            debugSheetsTableShape<<"loadEmbeddedDocument: load local file"<<d->url;
            res = document()->importDocument(d->url);
        } else {
            res = false;
            // For security reasons we need to ask confirmation if the url is remote.
            int result = KMessageBox::warningYesNoCancel(
                0, i18n("This document contains an external link to a remote document\n%1", d->url.url()),
                i18n("Confirmation Required"), KGuiItem(i18n("Download")), KGuiItem(i18n("Skip")));

            if (result == KMessageBox::Cancel) {
                //d->m_parent->setErrorMessage("USER_CANCELED");
                return false;
            }
            if (result == KMessageBox::Yes) {
                res = document()->openUrl(d->url);
            }
        }
        document()->setReadWrite(false);
    }
    if (!res) {
        QString errorMessage = document()->errorMessage();
        warnSheetsTableShape<<"loadEmbeddedDocument: failed"<<errorMessage;
        return false;
    }
    debugSheetsTableShape<<"loadEmbeddedDocument: loaded:"<<document()->url()<<document()->isStoredExtern();
    return res;
}

void TableShape::saveOdf(KoShapeSavingContext & context) const
{
    if (!sheet()) {
        warnSheetsTableShape<<"No sheet";
        return;
    }

    KoXmlWriter&  bodyWriter = context.xmlWriter();

    // Check if we're saving to a spreadsheet document. If not, embed a
    // spreadsheet document.
    //
    // FIXME: From ChartShape:
    //        The check isEmpty() fixes a crash that happened when a
    //        chart shape was saved from Words.  There are two
    //        problems with this fix:
    //        1. Checking the tag hierarchy is hardly the right way to do this
    //        2. The position doesn't seem to be saved yet.
    //
    //        Also, I have to check with the other apps, e.g. Calligra Sheets,
    //        if it works there too.
    //
    QList<const char*>  tagHierarchy = bodyWriter.tagHierarchy();
    if (tagHierarchy.isEmpty() || QString(tagHierarchy.last()) != "office:spreadsheet") {
        bodyWriter.startElement("draw:frame");
        // See also loadOdf() in loadOdfAttributes.
        saveOdfAttributes(context, OdfAllAttributes);

        bodyWriter.startElement("draw:object");
        document()->setUrl(d->url); // restore, somebody resets
        context.embeddedSaver().embedDocument(bodyWriter, document());
        bodyWriter.endElement(); // draw:object

        bodyWriter.endElement(); // draw:frame

        // TODO This should go into embeddedSaver
        debugSheetsTableShape<<"saving document: external:"<<document()->isStoredExtern()<<document()->url()<<document()->isModified();
        // Note that internal docs are save by embeddedSaver()
        if (document()->isStoredExtern() && document()->isModified()) {
            document()->save();
        }
        return;
    }
}

void TableShape::setMap()
{
    auto map = this->map();
    Sheet* sheet = map->sheetList().value(0);
    if (!sheet) {
        sheet = map->addNewSheet();
    }
    d->currentSheet = sheet;
    d->sheetView = new SheetView(sheet);
    d->isMaster = true;
    updateVisibleCellRange();

    connect(map, &Map::damagesFlushed, this, &TableShape::handleDamages);
}

void TableShape::setSize(const QSizeF& newSize)
{
    KoShape::setSize(newSize);
    if (sheet()) {
        map()->addDamage(new SheetDamage(sheet(), SheetDamage::ContentChanged));
    }
}

Map* TableShape::map() const
{
    return document()->map();
}

void TableShape::clear()
{
    delete d->pageManager;
    d->pageManager = nullptr;
    d->currentSheet = nullptr;
    delete d->sheetView;
    d->sheetView = nullptr;
    auto m = map();
    while (m->count() > 0) {
        m->removeSheet(m->sheet(0));
    }
}

Sheet* TableShape::sheet() const
{
    return d->currentSheet;
}

SheetView* TableShape::sheetView() const
{
    return d->sheetView;
}

void TableShape::setSheet(const QString& sheetName)
{
    Sheet* const sheet = map()->findSheet(sheetName);
    if (! sheet) {
        return;
    }
    d->currentSheet = sheet;
    delete d->sheetView;
    d->sheetView = new SheetView(sheet);
    updateVisibleCellRange();
    update();
}

void TableShape::setTopLeftOffset(const QPointF &point)
{
    d->topLeftOffset = point;
}

QPointF TableShape::topLeftOffset() const
{
    return d->topLeftOffset;
}

QRect TableShape::visibleCells() const
{
    return d->visibleCells();
}

void TableShape::updateVisibleCellRange()
{
    if (!sheet()) {
        return;
    }
    if (!d->sheetView) {
        d->sheetView = new SheetView(sheet());
    }
    d->sheetView->setPaintCellRange(visibleCells());
}

void TableShape::shapeChanged(ChangeType type, KoShape *shape)
{
    Q_UNUSED(shape);
    if (!sheet()) {
        return;
    }
    // If this is a master table shape, the parent changed and we have no parent yet...
    if (d->isMaster && type == ParentChanged && !d->pageManager) {
        d->pageManager = new TablePageManager(this);
        return;
    }
    // Not the master table shape? Not embedded into a container?
    if (!d->isMaster || !KoShape::parent()) {
        return;
    }
    // Not the changes, we want to react on?
    if (type != SizeChanged) {
        return;
    }
    d->pageManager->layoutPages();
}

void TableShape::handleDamages(const QList<Damage*>& damages)
{
    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;
        if (!damage) continue;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            const Region region = cellDamage->region();

            if (cellDamage->changes() & CellDamage::Appearance)
                d->sheetView->invalidateRegion(region);
            continue;
        }

        if (damage->type() == Damage::Sheet) {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>(damage);

            if (sheetDamage->changes() & SheetDamage::PropertiesChanged)
                d->sheetView->invalidate();
            continue;
        }
    }

    update();
}

void TableShape::setPaintingDisabled(bool disable)
{
    d->paintingDisabled = disable;
}
