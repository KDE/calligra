/* This file is part of the KDE project
 * Copyright (C) 2008,2010 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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

#include "KarbonZoomController.h"

#include <KarbonCanvas.h>
#include <KoZoomAction.h>
#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoZoomHandler.h>
#include <KoCanvasResourceManager.h>
#include <KoShapeManager.h>
#include <KoSelection.h>

#include <kactioncollection.h>
#include <klocalizedstring.h>

#include <math.h>

class KarbonZoomController::Private
{
public:
    Private()
            : canvasController(0), canvas(0), zoomHandler(0), action(0)
            , fitMargin(25)
    {
    }

    ~Private()
    {
        delete action;
    }

    KoCanvasController * canvasController;
    KarbonCanvas * canvas;
    KoZoomHandler * zoomHandler;
    KoZoomAction * action;
    QSizeF pageSize;
    int fitMargin;
};

KarbonZoomController::KarbonZoomController(KoCanvasController *controller, KActionCollection *actionCollection, QObject *parent)
        : QObject(parent), d(new Private())
{
    d->canvasController = controller;

    d->action = new KoZoomAction(KoZoomMode::ZOOM_WIDTH | KoZoomMode::ZOOM_PAGE, i18n("Zoom"), this);
    d->action->setSpecialButtons(KoZoomAction::ZoomToSelection | KoZoomAction::ZoomToAll);
    connect(d->action, SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)),
            this, SLOT(setZoom(KoZoomMode::Mode,qreal)));
    connect(d->action, SIGNAL(zoomedToSelection()),
            this, SIGNAL(zoomedToSelection()));
    connect(d->action, SIGNAL(zoomedToAll()),
            this, SIGNAL(zoomedToAll()));

    actionCollection->addAction("view_zoom", d->action);
    actionCollection->addAction(KStandardAction::ZoomIn, "zoom_in", this, SLOT(zoomInRelative()));
    actionCollection->addAction(KStandardAction::ZoomOut, "zoom_out", this, SLOT(zoomOutRelative()));

    d->canvas = dynamic_cast<KarbonCanvas*>(d->canvasController->canvas());
    d->zoomHandler = dynamic_cast<KoZoomHandler*>(const_cast<KoViewConverter*>(d->canvas->viewConverter()));

    connect(d->canvasController->proxyObject, SIGNAL(sizeChanged(QSize)), this, SLOT(setAvailableSize()));
    connect(d->canvasController->proxyObject, SIGNAL(zoomRelative(qreal,QPointF)), this, SLOT(requestZoomRelative(qreal,QPointF)));
    connect(d->canvasController->proxyObject, SIGNAL(moveDocumentOffset(QPoint)),
            d->canvas, SLOT(setDocumentOffset(QPoint)));

    connect(d->canvas->resourceManager(), SIGNAL(canvasResourceChanged(int,QVariant)),
            this, SLOT(canvasResourceChanged(int,QVariant)));
}

KarbonZoomController::~KarbonZoomController()
{
    delete d;
}

KoZoomAction * KarbonZoomController::zoomAction() const
{
    return d->action;
}

void KarbonZoomController::setZoomMode(KoZoomMode::Mode mode)
{
    setZoom(mode, 1.0);
}

void KarbonZoomController::setZoom(KoZoomMode::Mode mode, qreal zoom)
{
    setZoom(mode, zoom, d->canvasController->preferredCenter());
}

void KarbonZoomController::setZoom(KoZoomMode::Mode mode, qreal zoom, const QPointF &stillPoint)
{
    if (d->zoomHandler->zoomMode() == mode && d->zoomHandler->zoom() == zoom)
        return; // no change

    qreal oldEffectiveZoom = d->action->effectiveZoom();

    d->zoomHandler->setZoomMode(mode);

    if (mode == KoZoomMode::ZOOM_CONSTANT) {
        if (zoom == 0.0) return;
        d->action->setZoom(zoom);
    } else if (mode == KoZoomMode::ZOOM_WIDTH) {
        zoom = (d->canvasController->viewportSize().width() - 2 * d->fitMargin)
               / (d->zoomHandler->resolutionX() * d->pageSize.width());
        d->action->setSelectedZoomMode(mode);
        d->action->setEffectiveZoom(zoom);
    } else if (mode == KoZoomMode::ZOOM_PAGE) {
        zoom = (d->canvasController->viewportSize().width() - 2 * d->fitMargin)
               / (d->zoomHandler->resolutionX() * d->pageSize.width());
        zoom = qMin(zoom, (d->canvasController->viewportSize().height() - 2 * d->fitMargin)
                    / (d->zoomHandler->resolutionY() * d->pageSize.height()));

        d->action->setSelectedZoomMode(mode);
        d->action->setEffectiveZoom(zoom);
    }

    d->zoomHandler->setZoom(zoom);

    // Tell the canvasController that the zoom has changed
    // Actually canvasController doesn't know about zoom, but the document in pixels
    // has change as a result of the zoom change
    QRectF documentRect = d->canvas->documentViewRect();
    QSizeF viewSize = d->zoomHandler->documentToView(documentRect).size();
    d->canvasController->updateDocumentSize(QSize(qRound(viewSize.width()), qRound(viewSize.height())), true);

    d->canvas->adjustOrigin();

    // Finally ask the canvasController to recenter
    QPointF documentCenter;
    if (mode == KoZoomMode::ZOOM_CONSTANT) {
        KoSelection * selection = d->canvas->shapeManager()->selection();
        if (selection->count()) {
            QPointF docCenter = selection->boundingRect().center();
            documentCenter = d->canvas->documentOrigin() + d->zoomHandler->documentToView(docCenter);
        } else {
            qreal zoomCoeff = d->action->effectiveZoom() / oldEffectiveZoom;
            QPointF oldCenter = d->canvasController->preferredCenter();
            documentCenter = stillPoint * zoomCoeff - (stillPoint - 1.0 / zoomCoeff * oldCenter);
        }
    } else {
        // center the page rect when change the zoom mode to ZOOM_PAGE or ZOOM_WIDTH
        QRectF pageRect(-documentRect.topLeft(), d->pageSize);
        documentCenter = d->zoomHandler->documentToView(pageRect.center());
    }
    d->canvasController->setPreferredCenter(documentCenter);
    d->canvas->update();
}

void KarbonZoomController::setAvailableSize()
{
    if (d->zoomHandler->zoomMode() == KoZoomMode::ZOOM_WIDTH)
        setZoom(KoZoomMode::ZOOM_WIDTH, -1);
    if (d->zoomHandler->zoomMode() == KoZoomMode::ZOOM_PAGE)
        setZoom(KoZoomMode::ZOOM_PAGE, -1);
}

void KarbonZoomController::requestZoomRelative(const qreal factor, const QPointF &stillPoint)
{
    qreal zoom = d->zoomHandler->zoom();
    setZoom(KoZoomMode::ZOOM_CONSTANT, factor*zoom, stillPoint);
}

void KarbonZoomController::requestZoomBy(const qreal factor)
{
    qreal zoom = d->zoomHandler->zoom();
    setZoom(KoZoomMode::ZOOM_CONSTANT, factor*zoom);
}

void KarbonZoomController::setPageSize(const QSizeF &pageSize)
{
    if (d->pageSize == pageSize)
        return;
    d->pageSize = pageSize;

    if (d->zoomHandler->zoomMode() == KoZoomMode::ZOOM_WIDTH)
        setZoom(KoZoomMode::ZOOM_WIDTH, -1);
    if (d->zoomHandler->zoomMode() == KoZoomMode::ZOOM_PAGE)
        setZoom(KoZoomMode::ZOOM_PAGE, -1);
}

void KarbonZoomController::canvasResourceChanged(int key, const QVariant &value)
{
    if (key == KoCanvasResourceManager::PageSize) {
        setPageSize(value.toSizeF());

        // Tell the canvasController that the document in pixels
        // has changed as a result of the page layout change
        QRectF documentRect = d->canvas->documentViewRect();
        QSizeF viewSize = d->zoomHandler->documentToView(documentRect).size();
        d->canvasController->updateDocumentSize(QSize(qRound(viewSize.width()), qRound(viewSize.height())), true);
        d->canvas->adjustOrigin();
        d->canvas->update();
    }
}

void KarbonZoomController::zoomInRelative()
{
    requestZoomBy(sqrt(2.0));

}

void KarbonZoomController::zoomOutRelative()
{
    requestZoomBy(sqrt(0.5));
}

