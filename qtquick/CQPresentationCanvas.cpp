/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
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

#include "CQPresentationCanvas.h"
#include "CQCanvasController.h"
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KoPADocument.h>
#include <KoPACanvasItem.h>
#include <sheets/part/Doc.h>
#include <sheets/part/CanvasItem.h>
#include <KMimeType>
#include <KMimeTypeTrader>
#include <KDebug>
#include <KActionCollection>
#include <QGraphicsWidget>

#include <stage/part/KPrDocument.h>

#include "CQPresentationView.h"

void CQPresentationCanvas::setSource(const QString& source)
{
    if(m_source != source) {
        m_source = source;
        openFile(source);
        emit sourceChanged();
    }
}

QString CQPresentationCanvas::source() const
{
    return m_source;
}

bool CQPresentationCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("stagepart");
    if(service.isNull()) {
        qWarning("Unable to load Stage plugin, aborting!");
        return false;
    }

    KoPart *part = service->createInstance<KoPart>();
    KPrDocument* document = dynamic_cast<KPrDocument*>(part->document());
    document->setAutoSave(0);
    document->setCheckAutoSaveFile(false);
    document->openUrl (KUrl (uri));

    m_canvasBase = dynamic_cast<KoCanvasBase*> (part->canvasItem());
    createAndSetCanvasControllerOn(m_canvasBase);

    KoPACanvasItem *paCanvasItem = static_cast<KoPACanvasItem*>(part->canvasItem());

    CQPresentationView *view = new CQPresentationView(m_canvasController, static_cast<KoPACanvasBase*>(m_canvasBase), dynamic_cast<KPrDocument*>(document));
    paCanvasItem->setView(view);

    createAndSetZoomController(m_canvasBase);
    view->setZoomController(m_zoomController);
    //view->connectToZoomController();

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    view->doUpdateActivePage(document->pageByIndex(0, false));

    return true;
}

void CQPresentationCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    m_canvasController = controller;
    connect (controller, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQPresentationCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    m_zoomController = new KoZoomController(m_canvasController,
                                            zoomHandler,
                                            new KActionCollection(this));

    KoPACanvasItem* canvasItem = static_cast<KoPACanvasItem*>(canvas);

    // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
    connect (m_canvasController->proxyObject,
                SIGNAL(moveDocumentOffset(QPoint)), canvasItem, SLOT(slotSetDocumentOffset(QPoint)));
    // whenever the size of the document viewed in the canvas changes, inform the zoom controller
    //connect (canvasItem, SIGNAL(documentSize(QSize)), this, SLOT(updateDocumentSize(QSize)));
    canvasItem->update();
}

void CQPresentationCanvas::updateDocumentSize(const QSize& size)
{
    //m_zoomController->setDocumentSize(size);
}

void CQPresentationCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

#include "CQPresentationCanvas.moc"
