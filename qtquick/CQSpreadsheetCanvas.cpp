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
#include <KDebug>
#include <KActionCollection>
#include <QGraphicsWidget>

void CQSpreadsheetCanvas::setSource(const QString& source)
{
    m_source = source;
    openFile(source);
    emit sourceChanged();
}

QString CQSpreadsheetCanvas::source() const
{
    return m_source;
}

bool CQSpreadsheetCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("sheetspart");
    if(service.isNull()) {
        qWarning("Unable to load Sheets plugin, aborting!");
        return false;
    }

    KoPart* part = service->createInstance<KoPart>();
    Calligra::Sheets::Doc * document = static_cast<Calligra::Sheets::Doc*> (part->document());
    document->setAutoSave(0);
    document->setCheckAutoSaveFile(false);
    document->openUrl (KUrl (uri));

    m_canvasBase = dynamic_cast<KoCanvasBase*> (part->canvasItem());
    createAndSetCanvasControllerOn(m_canvasBase);
    createAndSetZoomController(m_canvasBase);

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    return true;
}

void CQSpreadsheetCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    m_canvasController = controller;
    connect (controller, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQSpreadsheetCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    m_zoomController = new KoZoomController(m_canvasController,
                                                            zoomHandler,
                                                            new KActionCollection(this));

    Calligra::Sheets::CanvasItem *canvasItem = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas);
    // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
    connect (m_canvasController->proxyObject,
                SIGNAL(moveDocumentOffset(QPoint)), canvasItem, SLOT(setDocumentOffset(QPoint)));
    // whenever the size of the document viewed in the canvas changes, inform the zoom controller
    connect (canvasItem, SIGNAL(documentSizeChanged(QSize)), this, SLOT(tellZoomControllerToSetDocumentSize(QSize)));
    canvasItem->update();
}

void CQSpreadsheetCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(m_canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

#include "CQSpreadsheetCanvas.moc"
