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

#include "CAPresentationHandler.h"
#include "PAView.h"
#include "CADocumentController.h"
#include "CanvasController.h"

#include <stage/part/KPrDocument.h>

#include <KoPACanvasItem.h>
#include <KoPAPageBase.h>

#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>

#include <KDebug>
#include <KMimeType>
#include <KMimeTypeTrader>

#include <QtCore/QSize>

class CAPresentationHandler::Private
{
public:
    KPrDocument* document;
    PAView* paView;
    int currentSlideNum;
};

CAPresentationHandler::CAPresentationHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{

}

CAPresentationHandler::~CAPresentationHandler()
{
    delete d;
}

KoDocument* CAPresentationHandler::document()
{
    return d->document;
}

bool CAPresentationHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoDocument* doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument> (mimetype, 0, 0, QString(),
                      QVariantList(), &error);

    if (!doc) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = static_cast<KPrDocument*> (doc);
    d->document->openUrl (KUrl (uri));

    KoCanvasBase* paCanvas = dynamic_cast<KoCanvasBase*> (d->document->canvasItem());
    KoPACanvasItem* paCanvasItem = dynamic_cast<KoPACanvasItem*> (paCanvas);
    if (!paCanvasItem) {
        kDebug() << "Failed to fetch a canvas item";
        return false;
    }

    if (paCanvasItem) {
        d->paView = new PAView (documentController()->canvasController(), dynamic_cast<KoPACanvasBase*> (paCanvas),
                                d->document);
        paCanvasItem->setView (d->paView);

        documentController()->canvasController()->setZoomController (d->paView->zoomController());
        documentController()->canvasController()->setZoomHandler (static_cast<KoZoomHandler*> (paCanvasItem->viewConverter()));

        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect (documentController()->canvasController()->canvasControllerProxyObject(),
                 SIGNAL (moveDocumentOffset (const QPoint&)), paCanvasItem, SLOT (slotSetDocumentOffset (QPoint)));
        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect (paCanvasItem, SIGNAL (documentSize (QSize)), this, SLOT (tellZoomControllerToSetDocumentSize (QSize)));
        connect (paCanvasItem, SIGNAL (documentSize (QSize)),
                 documentController()->canvasController()->canvasControllerProxyObject(),
                 SLOT (updateDocumentSize (QSize)));

        paCanvasItem->update();
    }

    setCanvas (paCanvas);
    KoToolManager::instance()->addController (documentController()->canvasController());

    connect (documentController()->canvasController(), SIGNAL (needCanvasUpdate()), SLOT (updateCanvas()));

    d->currentSlideNum = -1;
    nextSlide();

    return true;
}

QStringList CAPresentationHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.presentation" << "application/vnd.ms-powerpoint";
    return supportedTypes;
}

void CAPresentationHandler::nextSlide()
{
    d->currentSlideNum++;

    if (d->currentSlideNum >= d->document->pageCount())
        d->currentSlideNum = d->document->pageCount() - 1;
    d->paView->doUpdateActivePage (d->document->pageByIndex (d->currentSlideNum, false));
    zoomToFit();
}

void CAPresentationHandler::previousSlide()
{
    d->currentSlideNum--;

    if (d->currentSlideNum < 0)
        d->currentSlideNum = 0;
    d->paView->doUpdateActivePage (d->document->pageByIndex (d->currentSlideNum, false));
    zoomToFit();
}

void CAPresentationHandler::zoomToFit()
{
    QSizeF canvasSize (documentController()->canvasController()->width(),
                       documentController()->canvasController()->height());

    QSizeF pageSize = d->paView->activePage()->boundingRect().size();
    QGraphicsWidget* canvasItem = canvas()->canvasItem();
    QSizeF newSize (pageSize);
    newSize.scale (canvasSize, Qt::KeepAspectRatio);

    KoZoomHandler* zoomHandler = documentController()->canvasController()->zoomHandler();

    if (canvasSize.width() < canvasSize.height()) {
        canvasItem->setGeometry (0, (canvasSize.height() - newSize.height()) / 2,
                                 newSize.width(), newSize.height());
        zoomHandler->setZoom (canvasSize.width() / pageSize.width() * 0.75);
    } else {
        canvasItem->setGeometry ( (canvasSize.width() - newSize.width()) / 2, 0,
                                  newSize.width(), newSize.height());
        zoomHandler->setZoom (canvasSize.height() / pageSize.height() * 0.75);
    }
}

void CAPresentationHandler::tellZoomControllerToSetDocumentSize (const QSize& size)
{
    documentController()->canvasController()->zoomController()->setDocumentSize (size);
}

void CAPresentationHandler::updateCanvas()
{
    dynamic_cast<KoPACanvasItem*> (canvas())->update();
}

QString CAPresentationHandler::documentTypeName()
{
    return "presentation";
}

#include "CAPresentationHandler.moc"
