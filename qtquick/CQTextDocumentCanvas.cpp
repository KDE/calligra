/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
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

#include "CQTextDocumentCanvas.h"
#include "CQCanvasController.h"
#include "CQTextDocumentModel.h"

#include <KoDocument.h>
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWCanvasItem.h>
#include <KService>
#include <KActionCollection>
#include <QGraphicsWidget>
#include <QTextDocument>
#include <KDebug>

class CQTextDocumentCanvas::Private
{
public:
    Private()
        : canvasBase(0),
          canvasController(0),
          zoomController(0),
          zoomMode(ZOOM_CONSTANT),
          findText(0),
          documentModel(0)
    {}
    QString source;
    KoCanvasBase *canvasBase;
    KoCanvasController *canvasController;
    KoZoomController *zoomController;
    ZoomMode zoomMode;
    QString searchTerm;
    KoFindText *findText;
    CQTextDocumentModel *documentModel;
    QSize documentSize;
};

CQTextDocumentCanvas::CQTextDocumentCanvas(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private)
{
    d->findText = new KoFindText(this);

    connect (d->findText, SIGNAL(updateCanvas()), SLOT(updateCanvas()));
    connect (d->findText, SIGNAL(matchFound(KoFindMatch)), SLOT(findMatchFound(KoFindMatch)));
    connect (d->findText, SIGNAL(noMatchFound()), SLOT(findNoMatchFound()));
}

CQTextDocumentCanvas::~CQTextDocumentCanvas()
{
    delete d;
}

bool CQTextDocumentCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("wordspart");
    if(service.isNull()) {
        qWarning("Unable to load Words plugin, aborting!");
        return false;
    }

    KoPart* part = service->createInstance<KoPart>();
    KoDocument* document = part->document();
    document->setAutoSave(0);
    document->setCheckAutoSaveFile(false);
    document->openUrl(KUrl(uri));

    d->canvasBase = dynamic_cast<KoCanvasBase*> (part->canvasItem());
    createAndSetCanvasControllerOn(d->canvasBase);
    createAndSetZoomController(d->canvasBase);
    updateZoomControllerAccordingToDocument(document);
    updateControllerWithZoomMode();

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    KWCanvasItem *kwCanvasItem = dynamic_cast<KWCanvasItem*>(d->canvasBase);
    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(kwCanvasItem ->shapeManager()->shapes(), texts);
    d->findText->setDocuments(texts);

    KWDocument *kwDocument = static_cast<KWDocument*>(document);
    d->documentModel = new CQTextDocumentModel(this, kwDocument, kwCanvasItem->shapeManager());
    emit documentModelChanged();

    return true;
}

void CQTextDocumentCanvas::setSource(const QString& source)
{
    if(source != d->source) {
        d->source = source;
        openFile(source);
        emit sourceChanged();
    }
}

QString CQTextDocumentCanvas::source() const
{
    return d->source;
}

void CQTextDocumentCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
            updateControllerWithZoomMode();
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CQTextDocumentCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    d->canvasController = controller;
    connect (controller, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQTextDocumentCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    d->zoomController = new KoZoomController(d->canvasController,
                                                            zoomHandler,
                                                            new KActionCollection(this));
    KWCanvasItem *kwCanvasItem = static_cast<KWCanvasItem*>(canvas);
    connect (kwCanvasItem, SIGNAL(documentSize(QSizeF)), d->zoomController, SLOT(setDocumentSize(QSizeF)));
}

void CQTextDocumentCanvas::setZoomMode(ZoomMode zoomMode)
{
    d->zoomMode = zoomMode;
    updateControllerWithZoomMode();
    emit zoomModeChanged();
}

CQTextDocumentCanvas::ZoomMode CQTextDocumentCanvas::zoomMode() const
{
    return d->zoomMode;
}

void CQTextDocumentCanvas::updateZoomControllerAccordingToDocument(const KoDocument* document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    d->zoomController->setPageSize (kwDoc->pageManager()->begin().rect().size());
}

void CQTextDocumentCanvas::updateControllerWithZoomMode()
{
    KoZoomMode::Mode zoomMode;
    switch (d->zoomMode) {
        case ZOOM_CONSTANT: zoomMode = KoZoomMode::ZOOM_CONSTANT; break;
        case ZOOM_PAGE: zoomMode = KoZoomMode::ZOOM_PAGE; break;
        case ZOOM_WIDTH: zoomMode = KoZoomMode::ZOOM_WIDTH; break;
    }
    d->zoomController->setZoom(zoomMode, 1.0);
}

QString CQTextDocumentCanvas::searchTerm() const
{
    return d->searchTerm;
}

void CQTextDocumentCanvas::setSearchTerm(const QString& term)
{
    d->searchTerm = term;
    if (!term.isEmpty()) {
        d->findText->find(term);
    }
    emit searchTermChanged();
}

void CQTextDocumentCanvas::findMatchFound(const KoFindMatch &match)
{
    QTextCursor cursor = match.location().value<QTextCursor>();
    d->canvasBase->canvasItem()->update();

    d->canvasBase->resourceManager()->setResource (KoText::CurrentTextAnchor, cursor.anchor());
    d->canvasBase->resourceManager()->setResource (KoText::CurrentTextPosition, cursor.position());
}

void CQTextDocumentCanvas::findNoMatchFound()
{
    kDebug() << "Match for " << d->searchTerm << " not found";
}

void CQTextDocumentCanvas::updateCanvas()
{
    KWCanvasItem* kwCanvasItem = dynamic_cast<KWCanvasItem*> (d->canvasBase);
    kwCanvasItem->update();
}

void CQTextDocumentCanvas::findNext()
{
    d->findText->findNext();
}

void CQTextDocumentCanvas::findPrevious()
{
    d->findText->findPrevious();
}

QObject* CQTextDocumentCanvas::documentModel() const
{
    return d->documentModel;
}

QSize CQTextDocumentCanvas::documentSize() const
{
    return d->documentSize;
}

void CQTextDocumentCanvas::updateDocumentSize(const QSize& size)
{
    d->documentSize = size;
    emit documentSizeChanged();
}

#include "CQTextDocumentCanvas.moc"
