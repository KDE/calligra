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
#include "CAPAView.h"
#include "CADocumentController.h"
#include "CACanvasController.h"

#include <stage/part/KPrDocument.h>

#include <KoPACanvasItem.h>
#include <KoPAPageBase.h>

#include <KoPart.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>

#include <KDebug>
#include <KMimeType>
#include <KMimeTypeTrader>

#include <QSize>
#include <QTimer>

class CAPresentationHandler::Private
{
public:
    Private()
    {
        currentSlideNum = -1;
    }

    KPrDocument* document;
    CAPAView* paView;
    int currentSlideNum;
    QTimer slideshowTimer;
    QList<KoPAPageBase*> slideShow;
};

CAPresentationHandler::CAPresentationHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{
    connect(&d->slideshowTimer, SIGNAL(timeout()), SLOT(advanceSlideshow()));
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
    KoPart *part = KMimeTypeTrader::createInstanceFromQuery<KoPart>(mimetype,
                      QLatin1String("CalligraPart"), 0, QString(), QVariantList(), &error);

    if (!part) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = qobject_cast<KPrDocument*> (part->document());
    d->document->openUrl (KUrl (uri));

    KoCanvasBase* paCanvas = dynamic_cast<KoCanvasBase*> (part->canvasItem());
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
    emit currentSlideNumChanged();

    if (d->currentSlideNum >= d->document->pageCount())
        d->currentSlideNum = d->document->pageCount() - 1;
    emit currentSlideNumChanged();
    d->paView->doUpdateActivePage (d->document->pageByIndex (d->currentSlideNum, false));
    zoomToFit();
}

void CAPresentationHandler::previousSlide()
{
    if (d->currentSlideNum > 0)
    {
        d->currentSlideNum--;
        emit currentSlideNumChanged();
    }

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

    updateCanvas();
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

void CAPresentationHandler::resizeCanvas (const QSizeF& canvasSize)
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

QString CAPresentationHandler::topToolbarSource() const
{
    return "PresentationTopToolbar.qml";
}

QString CAPresentationHandler::leftToolbarSource() const
{
    return "PresentationLeftToolbar.qml";
}

QString CAPresentationHandler::rightToolbarSource() const
{
    return "PresentationRightToolbar.qml";
}

void CAPresentationHandler::setSlideshowDelay(int delay)
{
    d->slideshowTimer.setInterval(delay*1000);
}

int CAPresentationHandler::slideshowDelay() const
{
    return d->slideshowTimer.interval()/1000;
}

void CAPresentationHandler::startSlideshow()
{
    d->slideShow = d->document->slideShow();
    d->currentSlideNum = 0;
    d->paView->doUpdateActivePage(d->slideShow.at(d->currentSlideNum));

    d->slideshowTimer.start();
    emit slideshowStarted();
}

void CAPresentationHandler::stopSlideshow()
{
    d->slideshowTimer.stop();
    emit slideshowStopped();
}

void CAPresentationHandler::advanceSlideshow()
{
    d->paView->doUpdateActivePage(d->slideShow.at(++(d->currentSlideNum)));

    if (d->currentSlideNum == d->slideShow.count() - 1) {
        stopSlideshow();
    }
}

int CAPresentationHandler::currentSlideNumber() const
{
    return d->currentSlideNum + 1;
}

int CAPresentationHandler::totalNumberOfSlides() const
{
    return d->document->pageCount();
}

#include "CAPresentationHandler.moc"
