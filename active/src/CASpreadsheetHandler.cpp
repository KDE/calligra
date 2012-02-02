/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CASpreadsheetHandler.h"
#include "CADocumentController.h"
#include "CACanvasController.h"

#include <tables/Sheet.h>
#include <tables/Map.h>
#include <tables/DocBase.h>
#include <tables/part/CanvasItem.h>
#include <tables/part/Doc.h>

#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>

#include <KMimeType>
#include <KMimeTypeTrader>

class CASpreadsheetHandler::Private
{
public:
    Calligra::Tables::Doc* document;
};

CASpreadsheetHandler::CASpreadsheetHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{
}

CASpreadsheetHandler::~CASpreadsheetHandler()
{
    delete d;
}

KoDocument* CASpreadsheetHandler::document()
{
    return d->document;
}

bool CASpreadsheetHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoDocument* doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument> (mimetype, 0, 0, QString(),
                      QVariantList(), &error);

    if (!doc) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = static_cast<Calligra::Tables::Doc*> (doc);
    d->document->openUrl (KUrl (uri));

    setCanvas (dynamic_cast<KoCanvasBase*> (doc->canvasItem()));
    KoToolManager::instance()->addController (documentController()->canvasController());
    Calligra::Tables::CanvasItem* canvasItem = dynamic_cast<Calligra::Tables::CanvasItem*> (canvas());

    if (!canvasItem) {
        kDebug() << "Failed to fetch a canvas item";
        return false;
    }

    KoZoomHandler* zoomHandler = new KoZoomHandler();
    documentController()->canvasController()->setZoomHandler (zoomHandler);
    KoZoomController* zoomController = new KoZoomController (dynamic_cast<KoCanvasController*> (documentController()->canvasController()),
            zoomHandler, d->document->actionCollection());
    documentController()->canvasController()->setZoomController (zoomController);
    zoomController->setZoom (KoZoomMode::ZOOM_CONSTANT, 1.0);

    documentController()->canvasController()->setCanvasMode (KoCanvasController::Spreadsheet);

    if (canvasItem) {
        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect (documentController()->canvasController()->canvasControllerProxyObject(),
                 SIGNAL (moveDocumentOffset (const QPoint&)), canvasItem, SLOT (setDocumentOffset (QPoint)));
        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect (canvasItem, SIGNAL (documentSizeChanged (QSize)), this, SLOT (tellZoomControllerToSetDocumentSize (QSize)));
        canvasItem->update();
    }

    connect (documentController()->canvasController(), SIGNAL (needsCanvasResize (QSizeF)), SLOT (resizeCanvas (QSizeF)));
    connect (documentController()->canvasController(), SIGNAL (needCanvasUpdate()), SLOT (updateCanvas()));

    updateCanvas();
    documentController()->canvasController()->zoomToFit();

    return true;
}

QStringList CASpreadsheetHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.spreadsheet" << "application/vnd.ms-excel";
    return supportedTypes;
}

void CASpreadsheetHandler::tellZoomControllerToSetDocumentSize (QSize size)
{
    documentController()->canvasController()->zoomController()->setDocumentSize (size);
}

void CASpreadsheetHandler::updateCanvas()
{
    dynamic_cast<Calligra::Tables::CanvasItem*> (canvas())->update();
    updateDocumentSizeForActiveSheet();
}

void CASpreadsheetHandler::updateDocumentSizeForActiveSheet()
{
    Calligra::Tables::Sheet* sheet = dynamic_cast<Calligra::Tables::CanvasItem*> (canvas())->activeSheet();
    //FIXME 1.5 is a hack to "fix" the wrong values below. Why is it wrong?
    documentController()->canvasController()->updateDocumentSize (
        sheet->cellCoordinatesToDocument (sheet->usedArea (false)).toRect().size()*1.5, false);
}

QString CASpreadsheetHandler::documentTypeName()
{
    return "spreadsheet";
}

void CASpreadsheetHandler::nextSheet()
{
    Calligra::Tables::CanvasItem* canvasItem = dynamic_cast<Calligra::Tables::CanvasItem*> (canvas());
    if (!canvasItem)
        return;
    Calligra::Tables::Sheet* sheet = canvasItem->activeSheet();
    if (!sheet)
        return;
    Calligra::Tables::DocBase* kspreadDoc = qobject_cast<Calligra::Tables::DocBase*> (document());
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->nextSheet (sheet);
    if (!sheet)
        return;
    canvasItem->setActiveSheet (sheet);
    documentController()->canvasController()->updateDocumentSize (sheet->cellCoordinatesToDocument (sheet->usedArea (false)).toRect().size(), false);
}

void CASpreadsheetHandler::previousSheet()
{
    Calligra::Tables::CanvasItem* canvasItem = dynamic_cast<Calligra::Tables::CanvasItem*> (canvas());
    if (!canvasItem)
        return;
    Calligra::Tables::Sheet* sheet = canvasItem->activeSheet();
    if (!sheet)
        return;
    Calligra::Tables::DocBase* kspreadDoc = dynamic_cast<Calligra::Tables::DocBase*> (document());
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->previousSheet (sheet);
    if (!sheet)
        return;
    canvasItem->setActiveSheet (sheet);
    documentController()->canvasController()->updateDocumentSize (sheet->cellCoordinatesToDocument (sheet->usedArea (false)).toRect().size(), false);
}

void CASpreadsheetHandler::resizeCanvas (const QSizeF& canvasSize)
{
    canvas()->canvasItem()->setGeometry (QRectF (QPointF (0, 0), canvasSize));
}

int CASpreadsheetHandler::sheetCount() const
{
    return dynamic_cast<Calligra::Tables::CanvasItem*> (canvas())->activeSheet()->map()->count();
}

QString CASpreadsheetHandler::leftToolbarSource() const
{
    return "SpreadsheetLeftToolbar.qml";
}

QString CASpreadsheetHandler::rightToolbarSource() const
{
    return "SpreadsheetRightToolbar.qml";
}

#include "CASpreadsheetHandler.moc"
