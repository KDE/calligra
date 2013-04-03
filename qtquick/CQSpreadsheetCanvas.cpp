/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 *
 */

#include "CQSpreadsheetCanvas.h"
#include "CQCanvasController.h"
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <sheets/part/Doc.h>
#include <sheets/part/CanvasItem.h>
#include <sheets/Map.h>
#include <sheets/Sheet.h>
#include <KDebug>
#include <KActionCollection>
#include <QGraphicsWidget>

class CQSpreadsheetCanvas::Private
{
public:
    Private() : canvas(0), document(0), currentSheet(0) { }

    Calligra::Sheets::CanvasItem* canvas;
    Calligra::Sheets::Doc * document;

    int currentSheet;
};

CQSpreadsheetCanvas::CQSpreadsheetCanvas(QDeclarativeItem* parent)
    : CQCanvasBase(parent), d(new Private)
{

}

CQSpreadsheetCanvas::~CQSpreadsheetCanvas()
{
    delete d;
}

int CQSpreadsheetCanvas::currentSheet() const
{
    return d->currentSheet;
}

Calligra::Sheets::Map* CQSpreadsheetCanvas::documentMap() const
{
    return d->document->map();
}

void CQSpreadsheetCanvas::setCurrentSheet(int sheet)
{
    sheet = qBound(0, sheet, d->document->map()->count() - 1);
    if(sheet != d->currentSheet) {
        d->currentSheet = sheet;
        d->canvas->setActiveSheet(d->document->map()->sheet(d->currentSheet));
        emit currentSheetChanged();
    }
}

void CQSpreadsheetCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("sheetspart");
    if(service.isNull()) {
        qWarning("Unable to load Sheets plugin, aborting!");
        return;
    }

    KoPart* part = service->createInstance<KoPart>();
    d->document = static_cast<Calligra::Sheets::Doc*> (part->document());
    d->document->setAutoSave(0);
    d->document->setCheckAutoSaveFile(false);
    d->document->openUrl (KUrl (uri));

    d->canvas = dynamic_cast<Calligra::Sheets::CanvasItem*> (part->canvasItem());
    createAndSetCanvasControllerOn(d->canvas);
    createAndSetZoomController(d->canvas);

    d->canvas->setParentItem(this);
    d->canvas->installEventFilter(this);
    d->canvas->setVisible(true);
    d->canvas->setGeometry(x(), y(), width(), height());

    Calligra::Sheets::Sheet *sheet = d->document->map()->sheet(0);
    if(sheet) {
        updateDocumentSize(sheet->documentSize().toSize());
    }
}

void CQSpreadsheetCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    setCanvasController(controller);
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQSpreadsheetCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    setZoomController(new KoZoomController(canvasController(),
                                           zoomHandler,
                                           new KActionCollection(this)));

    Calligra::Sheets::CanvasItem *canvasItem = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas);
    // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
    connect (canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), canvasItem, SLOT(setDocumentOffset(QPoint)));
    // whenever the size of the document viewed in the canvas changes, inform the zoom controller
    connect(canvasItem, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    canvasItem->update();
}

void CQSpreadsheetCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvas) {
        d->canvas->setGeometry(newGeometry);
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CQSpreadsheetCanvas::updateDocumentSize(const QSize& size)
{
    zoomController()->setDocumentSize(size, false);
}
