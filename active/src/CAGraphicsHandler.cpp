/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2012  Yue Liu <yue.liu@mail.com>
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

#include "CAGraphicsHandler.h"
#include "CAPAView.h"
#include "CADocumentController.h"
#include "CACanvasController.h"

#include <flow/part/FlowDocument.h>

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

#include <QSize>
#include <QSizeF>

class CAGraphicsHandler::Private
{
public:
    Private()
    {
        currentPageNum = -1;
    }

    FlowDocument* document;
    CAPAView* paView;
    int currentPageNum;
};

CAGraphicsHandler::CAGraphicsHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController), d (new Private())
{
}

CAGraphicsHandler::~CAGraphicsHandler()
{
    delete d;
}

KoDocument* CAGraphicsHandler::document()
{
    return d->document;
}

bool CAGraphicsHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoDocument* doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument> (mimetype, 0, 0, QString(),
                      QVariantList(), &error);

    if (!doc) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = static_cast<FlowDocument*> (doc);
    d->document->openUrl (KUrl (uri));

    KoCanvasBase* paCanvas = dynamic_cast<KoCanvasBase*> (d->document->canvasItem());
    KoPACanvasItem* paCanvasItem = dynamic_cast<KoPACanvasItem*> (paCanvas);
    if (!paCanvasItem) {
        kDebug() << "Failed to fetch a canvas item";
        return false;
    }

    if (paCanvasItem) {
        d->paView = new CAPAView (documentController()->canvasController(), dynamic_cast<KoPACanvasBase*> (paCanvas),
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

    connect(documentController()->canvasController(), SIGNAL(needsCanvasResize(QSizeF)), SLOT(resizeCanvas(QSizeF)));
    connect (documentController()->canvasController(), SIGNAL (needCanvasUpdate()), SLOT (updateCanvas()));

    d->document;

    nextPage();

    return true;
}

QStringList CAGraphicsHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.graphics" << "application/vnd.visio";
    return supportedTypes;
}

void CAGraphicsHandler::nextPage()
{
    d->currentPageNum++;
    //emit currentPageNumChanged();

    if (d->currentPageNum >= d->document->pageCount())
        d->currentPageNum = d->document->pageCount() - 1;
    //emit currentPageNumChanged();
    d->paView->doUpdateActivePage (d->document->pageByIndex (d->currentPageNum, false));
    zoomToFit();
}

void CAGraphicsHandler::previousPage()
{
    if (d->currentPageNum > 0)
    {
        d->currentPageNum--;
        //emit currentPageNumChanged();
    }

    d->paView->doUpdateActivePage (d->document->pageByIndex (d->currentPageNum, false));
    zoomToFit();
}

void CAGraphicsHandler::zoomToFit()
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

    updateCanvas();
}

void CAGraphicsHandler::tellZoomControllerToSetDocumentSize (const QSize& size)
{
    documentController()->canvasController()->zoomController()->setDocumentSize (size);
}

void CAGraphicsHandler::updateCanvas()
{
    dynamic_cast<KoPACanvasItem*> (canvas())->update();
}

QString CAGraphicsHandler::documentTypeName()
{
    return "graphics";
}

void CAGraphicsHandler::resizeCanvas (const QSizeF& canvasSize)
{
    QSizeF pageSize = d->paView->activePage()->boundingRect().size();
    QGraphicsWidget* canvasItem = canvas()->canvasItem();
    QSizeF newSize (pageSize);
    newSize.scale (canvasSize, Qt::KeepAspectRatio);

    if (canvasSize.width() < canvasSize.height()) {
        canvasItem->setGeometry (0, (canvasSize.height() - newSize.height()) / 2,
                                 newSize.width(), newSize.height());
        documentController()->canvasController()->zoomHandler()->setZoom (canvasSize.width() / pageSize.width() * 0.75);
    } else {
        canvasItem->setGeometry ( (canvasSize.width() - newSize.width()) / 2, 0,
                                  newSize.width(), newSize.height());
        documentController()->canvasController()->zoomHandler()->setZoom (canvasSize.height() / pageSize.height() * 0.75);
    }
}

QString CAGraphicsHandler::topToolbarSource() const
{
    return "FlowTopToolbar.qml";
}

QString CAGraphicsHandler::leftToolbarSource() const
{
    return "FlowDockToolbar.qml";
}

QString CAGraphicsHandler::rightToolbarSource() const
{
    return "FlowDockToolbar.qml";
}

int CAGraphicsHandler::currentPageNumber() const
{
    return d->currentPageNum + 1;
}

int CAGraphicsHandler::totalNumberOfPages() const
{
    return d->document->pageCount();
}

