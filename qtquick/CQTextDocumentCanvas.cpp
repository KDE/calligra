/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include <QStyleOptionGraphicsItem>

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
#include <KActionCollection>
#include <QGraphicsWidget>
#include <QTextDocument>
#include <KDebug>

class CQTextDocumentCanvas::Private
{
public:
    Private()
        : canvasBase(0),
          findText(0),
          documentModel(0)
    {}

    KoCanvasBase *canvasBase;
    QString searchTerm;
    KoFindText *findText;
    CQTextDocumentModel *documentModel;
    QSize documentSize;
    int pageNumber;
    QPoint currentPoint;
};

CQTextDocumentCanvas::CQTextDocumentCanvas(QDeclarativeItem* parent)
    : CQCanvasBase(parent), d(new Private)
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

void CQTextDocumentCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("wordspart");
    if(service.isNull()) {
        qWarning("Unable to load Words plugin, aborting!");
        return;
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

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    if(d->pageNumber >= 1) {
      gotoPage(d->pageNumber, document);
    }

    KWCanvasItem *kwCanvasItem = dynamic_cast<KWCanvasItem*>(d->canvasBase);
    QList<QTextDocument*> texts;
    KoFindText::findTextInShapes(kwCanvasItem ->shapeManager()->shapes(), texts);
    d->findText->setDocuments(texts);

    KWDocument *kwDocument = static_cast<KWDocument*>(document);
    d->documentModel = new CQTextDocumentModel(this, kwDocument, kwCanvasItem->shapeManager());
    emit documentModelChanged();
}

void CQTextDocumentCanvas::gotoPage(int pageNumber, KoDocument *document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    KWPage currentTextDocPage = kwDoc->pageManager()->page(pageNumber);

    QRectF rect = d->canvasBase->viewConverter()->documentToView(currentTextDocPage.rect());
    alignTopWith(rect.top());
    updateCanvas();
}

int CQTextDocumentCanvas::cameraY() const
{
    return d->currentPoint.y();
}

void CQTextDocumentCanvas::setCameraY(int cameraY)
{
    d->currentPoint.setY (cameraY);
    emit cameraYChanged();
}

void CQTextDocumentCanvas::alignTopWith(int y)
{
    d->currentPoint.setY(y);
    emit cameraYChanged();
}

int CQTextDocumentCanvas::currentPageNumber() const
{
    return d->pageNumber;
}

void CQTextDocumentCanvas::setCurrentPageNumber(const int& currentPageNumber)
{
    d->pageNumber = currentPageNumber;
    emit currentPageNumberChanged();
}

void CQTextDocumentCanvas::render(QPainter* painter, const QRectF& target)
{
    QStyleOptionGraphicsItem option;
    option.exposedRect = target;
    option.rect = target.toAlignedRect();
    d->canvasBase->canvasItem()->paint(painter, &option);
}

void CQTextDocumentCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void CQTextDocumentCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    setCanvasController(controller);
    connect (controller, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQTextDocumentCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    setZoomController(new KoZoomController(canvasController(), zoomHandler, new KActionCollection(this)));

    KWCanvasItem *kwCanvasItem = static_cast<KWCanvasItem*>(canvas);
    connect (kwCanvasItem, SIGNAL(documentSize(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));
    connect (canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), kwCanvasItem, SLOT(setDocumentOffset(QPoint)));
    kwCanvasItem->updateSize();
}

void CQTextDocumentCanvas::updateZoomControllerAccordingToDocument(const KoDocument* document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    zoomController()->setPageSize (kwDoc->pageManager()->begin().rect().size());
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
