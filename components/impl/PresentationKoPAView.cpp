/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "PresentationKoPAView.h"

#include <KoCanvasController.h>
#include <KoPACanvasBase.h>
#include <KoPACanvasItem.h>
#include <KoPADocument.h>
#include <KoPAMasterPage.h>
#include <KoPAPage.h>
#include <KoPAPageBase.h>
#include <KoPAViewModeNormal.h>
#include <KoPart.h>
#include <KoSelection.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <QGraphicsItem>
#include <stage/part/KPrDocument.h>

using namespace Calligra::Components;

class PresentationKoPAView::Private
{
public:
    Private()
        : canvasController{nullptr}
        , zoomController{nullptr}
        , paCanvas{nullptr}
        , prDocument{nullptr}
        , page{nullptr}
    {
    }

    KoCanvasController *canvasController;
    KoZoomController *zoomController;
    KoPACanvasBase *paCanvas;
    KPrDocument *prDocument;
    KoPAPageBase *page;
};

// TODO: Cleanup code that is not called
PresentationKoPAView::PresentationKoPAView(KoCanvasController *canvasController, KoPACanvasBase *canvas, KPrDocument *prDocument)
    : d{new Private}
{
    d->canvasController = canvasController;
    d->paCanvas = canvas;
    d->prDocument = prDocument;

    KoPAViewModeNormal *mode = new KoPAViewModeNormal(this, d->paCanvas);
    setViewMode(mode);
}

PresentationKoPAView::~PresentationKoPAView() = default;

void PresentationKoPAView::setShowRulers(bool show)
{
    Q_UNUSED(show);
}

void PresentationKoPAView::editPaste()
{
}

void PresentationKoPAView::pagePaste()
{
}

void PresentationKoPAView::insertPage()
{
}

void PresentationKoPAView::updatePageNavigationActions()
{
}

void PresentationKoPAView::setActionEnabled(int actions, bool enable)
{
    Q_UNUSED(actions);
    Q_UNUSED(enable);
}

void PresentationKoPAView::navigatePage(KoPageApp::PageNavigation pageNavigation)
{
    Q_UNUSED(pageNavigation);
}

KoPAPageBase *PresentationKoPAView::activePage() const
{
    return d->page;
}

void PresentationKoPAView::setActivePage(KoPAPageBase *page)
{
    auto shapeManager = d->paCanvas->shapeManager();
    auto masterShapeManager = d->paCanvas->masterShapeManager();
    shapeManager->removeAdditional(d->page);
    d->page = page;
    shapeManager->addAdditional(page);

    auto shapes = page->shapes();
    shapeManager->setShapes(shapes, KoShapeManager::AddWithoutRepaint);
    // Make the top most layer active
    if (!shapes.isEmpty()) {
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(shapes.last());
        shapeManager->selection()->setActiveLayer(layer);
    }

    // if the page is not a master page itself set shapes of the master page
    auto paPage = dynamic_cast<KoPAPage *>(page);
    if (paPage) {
        auto masterPage = paPage->masterPage();
        auto masterShapes = masterPage->shapes();
        masterShapeManager->setShapes(masterShapes, KoShapeManager::AddWithoutRepaint);
        // Make the top most layer active
        if (!masterShapes.isEmpty()) {
            auto layer = dynamic_cast<KoShapeLayer *>(masterShapes.last());
            masterShapeManager->selection()->setActiveLayer(layer);
        }
    } else {
        // if the page is a master page no shapes are in the masterShapeManager
        masterShapeManager->setShapes(QList<KoShape *>());
    }

    // Set the current page number in the canvas resource provider
    d->paCanvas->resourceManager()->setResource(KoCanvasResourceManager::CurrentPage, d->prDocument->pageIndex(page) + 1);
}

void PresentationKoPAView::doUpdateActivePage(KoPAPageBase *page)
{
    setActivePage(page);

    d->paCanvas->updateSize();
    auto layout = page->pageLayout();

    QSizeF pageSize{layout.width, layout.height};
    d->paCanvas->setDocumentOrigin(QPointF(0, 0));
    d->paCanvas->resourceManager()->setResource(KoCanvasResourceManager::PageSize, pageSize);
    zoomController()->setPageSize(pageSize);
    zoomController()->setDocumentSize(pageSize);

    d->paCanvas->updateSize();
    auto item = dynamic_cast<QGraphicsItem *>(d->paCanvas);
    item->update();

    proxyObject->emitActivePageChanged();
}

KoZoomController *PresentationKoPAView::zoomController() const
{
    return d->zoomController;
}

KoPADocument *PresentationKoPAView::kopaDocument() const
{
    return d->prDocument;
}

KoPACanvasBase *PresentationKoPAView::kopaCanvas() const
{
    return d->paCanvas;
}

void PresentationKoPAView::connectToZoomController()
{
    connect(zoomController(), &KoZoomController::zoomChanged, this, &PresentationKoPAView::slotZoomChanged);
}

void PresentationKoPAView::slotZoomChanged(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(zoom);
    if (d->page) {
        if (mode == KoZoomMode::ZOOM_PAGE) {
            auto layout = d->page->pageLayout();
            QRectF pageRect{0, 0, layout.width, layout.height};
            d->canvasController->ensureVisible(d->paCanvas->viewConverter()->documentToView(pageRect));
        } else if (mode == KoZoomMode::ZOOM_WIDTH) {
            // horizontally center the page
            auto layout = d->page->pageLayout();
            QRectF pageRect{0, 0, layout.width, layout.height};
            auto viewRect = d->paCanvas->viewConverter()->documentToView(pageRect).toRect();
            viewRect.translate(d->paCanvas->documentOrigin());
            QRect currentVisible{qMax(0, -d->canvasController->canvasOffsetX()),
                                 qMax(0, -d->canvasController->canvasOffsetY()),
                                 d->canvasController->visibleWidth(),
                                 d->canvasController->visibleHeight()};
            int horizontalMove = viewRect.center().x() - currentVisible.center().x();
            d->canvasController->pan(QPoint(horizontalMove, 0));
        }
        dynamic_cast<KoPACanvasItem *>(d->paCanvas)->update();
    }
}

void PresentationKoPAView::setZoomController(KoZoomController *controller)
{
    d->zoomController = controller;
}
