/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2006-2009 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "CQPresentationView.h"
#include "CQCanvasController.h"

#include <KoZoomController.h>
#include <KoPADocument.h>
#include <KoPACanvasBase.h>
#include <KoPAPageBase.h>
#include <KoPAViewModeNormal.h>
#include <stage/part/KPrDocument.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoSelection.h>
#include <KoPAPage.h>
#include <KoPAMasterPage.h>
#include <KoCanvasController.h>
#include <KoPACanvasItem.h>
#include <KoZoomHandler.h>
#include <KoPart.h>

#include <QGraphicsItem>

//TODO: Cleanup code that is not called
CQPresentationView::CQPresentationView (KoCanvasController* canvasController, KoPACanvasBase* canvas, KPrDocument* prDocument)
    : m_canvasController (canvasController), m_zoomController(0), m_paCanvas (canvas), m_prDocument (prDocument), m_page (0)
{
    KoPAViewModeNormal* mode = new KoPAViewModeNormal (this, m_paCanvas);
    setViewMode (mode);
}

CQPresentationView::~CQPresentationView()
{

}

void CQPresentationView::setShowRulers (bool show)
{
    Q_UNUSED(show);
}

void CQPresentationView::editPaste()
{

}

void CQPresentationView::pagePaste()
{

}

void CQPresentationView::insertPage()
{

}

void CQPresentationView::updatePageNavigationActions()
{

}

void CQPresentationView::setActionEnabled (int actions, bool enable)
{
    Q_UNUSED(actions);
    Q_UNUSED(enable);

}

void CQPresentationView::navigatePage (KoPageApp::PageNavigation pageNavigation)
{
    Q_UNUSED(pageNavigation);
}

KoPAPageBase* CQPresentationView::activePage() const
{
    return m_page;
}

void CQPresentationView::setActivePage (KoPAPageBase* page)
{
    KoShapeManager* shapeManager = m_paCanvas->shapeManager();
    KoShapeManager* masterShapeManager = m_paCanvas->masterShapeManager();
    shapeManager->removeAdditional (m_page);
    m_page = page;
    shapeManager->addAdditional (page);
    QList<KoShape*> shapes = page->shapes();
    shapeManager->setShapes (shapes, KoShapeManager::AddWithoutRepaint);
    //Make the top most layer active
    if (!shapes.isEmpty()) {
        KoShapeLayer* layer = dynamic_cast<KoShapeLayer*> (shapes.last());
        shapeManager->selection()->setActiveLayer (layer);
    }

    // if the page is not a master page itself set shapes of the master page
    KoPAPage* paPage = dynamic_cast<KoPAPage*> (page);
    if (paPage) {
        KoPAMasterPage* masterPage = paPage->masterPage();
        QList<KoShape*> masterShapes = masterPage->shapes();
        masterShapeManager->setShapes (masterShapes, KoShapeManager::AddWithoutRepaint);
        //Make the top most layer active
        if (!masterShapes.isEmpty()) {
            KoShapeLayer* layer = dynamic_cast<KoShapeLayer*> (masterShapes.last());
            masterShapeManager->selection()->setActiveLayer (layer);
        }
    } else {
        // if the page is a master page no shapes are in the masterShapeManager
        masterShapeManager->setShapes (QList<KoShape*>());
    }

    // Set the current page number in the canvas resource provider
    m_paCanvas->resourceManager()->setResource (KoCanvasResourceManager::CurrentPage, m_prDocument->pageIndex (page) + 1);
}

void CQPresentationView::doUpdateActivePage (KoPAPageBase* page)
{
    // save the old offset into the page so we can use it also on the new page
    setActivePage (page);

    m_paCanvas->updateSize();
    KoPageLayout& layout = page->pageLayout();

    QSizeF pageSize (layout.width, layout.height);
    //m_paCanvas->setDocumentOrigin(QPointF(layout.width, layout.height));
    m_paCanvas->setDocumentOrigin (QPointF (0, 0));
    // the page is in the center of the canvas
    m_paCanvas->resourceManager()->setResource (KoCanvasResourceManager::PageSize, pageSize);
    zoomController()->setPageSize(pageSize);
    zoomController()->setDocumentSize(pageSize);
    //zoomController()->setZoom(KoZoomMode::ZOOM_PAGE, 1);

    m_paCanvas->updateSize();
    QGraphicsItem* item = dynamic_cast<QGraphicsItem*> (m_paCanvas);
    item->update();

    proxyObject->emitActivePageChanged();
}

KoZoomController* CQPresentationView::zoomController() const
{
    return m_zoomController;
}

KoPADocument* CQPresentationView::kopaDocument() const
{
    return m_prDocument;
}

KoPACanvasBase* CQPresentationView::kopaCanvas() const
{
    return m_paCanvas;
}

void CQPresentationView::connectToZoomController()
{
    connect (zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)), SLOT(slotZoomChanged(KoZoomMode::Mode,qreal)));
}

void CQPresentationView::slotZoomChanged (KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED (zoom);
    if (m_page) {
        if (mode == KoZoomMode::ZOOM_PAGE) {
            KoPageLayout& layout = m_page->pageLayout();
            QRectF pageRect (0, 0, layout.width, layout.height);
            m_canvasController->ensureVisible (m_paCanvas->viewConverter()->documentToView (pageRect));
        } else if (mode == KoZoomMode::ZOOM_WIDTH) {
            // horizontally center the page
            KoPageLayout& layout = m_page->pageLayout();
            QRectF pageRect (0, 0, layout.width, layout.height);
            QRect viewRect = m_paCanvas->viewConverter()->documentToView (pageRect).toRect();
            viewRect.translate (m_paCanvas->documentOrigin());
            QRect currentVisible (qMax (0, -m_canvasController->canvasOffsetX()),
                                  qMax (0, -m_canvasController->canvasOffsetY()),
                                  m_canvasController->visibleWidth(),
                                  m_canvasController->visibleHeight());
            int horizontalMove = viewRect.center().x() - currentVisible.center().x();
            m_canvasController->pan (QPoint (horizontalMove, 0));
        }
        dynamic_cast<KoPACanvasItem*> (m_paCanvas)->update();
    }
}

void CQPresentationView::setZoomController(KoZoomController* controller)
{
    m_zoomController = controller;
}
