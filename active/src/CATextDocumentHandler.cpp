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

#include "CATextDocumentHandler.h"
#include "CADocumentController.h"
#include "CanvasController.h"

#include <KWDocument.h>
#include <KWCanvasItem.h>
#include <KoCanvasController.h>

#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>

#include <KMimeType>
#include <KMimeTypeTrader>
#include <KDebug>

class CATextDocumentHandler::Private
{
public:
    Private()
    {
        document = 0;
    }
    KWDocument *document;
    KoZoomHandler *zoomHandler;
    KoZoomController *zoomController;
    KWPage currentTextDocPage;
};

CATextDocumentHandler::CATextDocumentHandler(CADocumentController* documentController)
    : CAAbstractDocumentHandler(documentController)
    , d(new Private())
{

}

CATextDocumentHandler::~CATextDocumentHandler()
{
    delete d;
}

QStringList CATextDocumentHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.text" << "application/msword";
    return supportedTypes;
}

bool CATextDocumentHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath(uri)->name();
    KoDocument *doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument>(mimetype, 0, 0, QString(),
                      QVariantList(), &error);

    if (!doc) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    kDebug() << "Trying to open the document";
    d->document = static_cast<KWDocument*>(doc);
    d->document->openUrl(KUrl(uri));

    setCanvas(dynamic_cast<KoCanvasBase*>(doc->canvasItem()));
    kDebug() << "CONTROLLER " << documentController()->canvasController();
    KoToolManager::instance()->addController(dynamic_cast<KoCanvasController*>(documentController()->canvasController()));
    KWCanvasItem *kwCanvasItem = dynamic_cast<KWCanvasItem*>(canvas());

    if (!kwCanvasItem) {
        kDebug() << "Failed to get KWCanvasItem";
        return false;
    }

    d->zoomHandler = static_cast<KoZoomHandler*>(kwCanvasItem->viewConverter());
    d->zoomController = new KoZoomController(dynamic_cast<KoCanvasController*>(documentController()->canvasController()),
                                             d->zoomHandler, doc->actionCollection());
    d->currentTextDocPage = d->document->pageManager()->begin();
    d->zoomController->setPageSize(d->currentTextDocPage.rect().size());
    d->zoomController->setZoom(KoZoomMode::ZOOM_CONSTANT, 1.0);

    if (kwCanvasItem) {
        kwCanvasItem->updateSize();

        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect(kwCanvasItem, SIGNAL(documentSize(QSizeF)), d->zoomController, SLOT(setDocumentSize(QSizeF)));
        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        //connect(proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvasItem, SLOT(setDocumentOffset(QPoint)));
        kwCanvasItem->updateSize();
    }

    return true;
}

KoDocument* CATextDocumentHandler::document()
{
    return d->document;
}

#include "CATextDocumentHandler.moc"

