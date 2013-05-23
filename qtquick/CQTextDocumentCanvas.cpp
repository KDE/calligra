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
#include <KoShape.h>
#include <KActionCollection>
#include <QGraphicsWidget>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>
#include <KDebug>

class CQTextDocumentCanvas::Private
{
public:
    Private()
        : canvasBase(0),
          findText(0),
          documentModel(0),
          document(0)
    {}

    KoCanvasBase *canvasBase;
    QString searchTerm;
    KoFindText *findText;
    CQTextDocumentModel *documentModel;
    KWDocument* document;
    QSize documentSize;
    int pageNumber;
    QPoint currentPoint;
    QObjectList linkTargets;

    void updateLinkTargets()
    {
        qDeleteAll(linkTargets);
        linkTargets.clear();

        KWCanvasItem *kwCanvasItem = dynamic_cast<KWCanvasItem*>(canvasBase);
        if(!kwCanvasItem)
            return;
        foreach(const KoShape* shape, kwCanvasItem->shapeManager()->shapes()) {
            if(!shape->hyperLink().isEmpty()) {
                QObject * obj = new QObject(documentModel);
                obj->setProperty("linkRect", shape->boundingRect());
                obj->setProperty("linkTarget", QUrl(shape->hyperLink()));
                linkTargets.append(obj);
            }
        }

        foreach(QTextDocument* text, findText->documents()) {
            QTextBlock block = text->rootFrame()->firstCursorPosition().block();
            for (; block.isValid(); block = block.next()) {
                block.begin();
                QTextBlock::iterator it;
                for (it = block.begin(); !(it.atEnd()); ++it) {
                    QTextFragment fragment = it.fragment();
                    if (fragment.isValid()) {
                        QTextCharFormat format = fragment.charFormat();
                        if(format.isAnchor()) {
                            // This is an anchor, store target and position...
                            QObject * obj = new QObject(documentModel);
                            QRectF rect = getFragmentPosition(block, fragment);
                            KWPage page = document->pageManager()->page(rect.left());
                            rect.translate(page.topMargin(), page.rightMargin());
                            rect = canvasBase->viewConverter()->documentToView(rect);
                            rect.moveLeft(rect.left() - (page.pageNumber() * 8) + 28);
                            obj->setProperty("linkRect", rect);
                            obj->setProperty("linkTarget", QUrl(format.anchorHref()));
                            linkTargets.append(obj);
                        }
                    }
                }
            }
        }
    }

    QRectF getFragmentPosition(QTextBlock block, QTextFragment fragment)
    {
        // TODO this only produces a position for the first part, if the link spans more than one line...
        // Need to sort that somehow, unfortunately probably by slapping this code into the above function.
        // For now leave it like this, more important things are needed.
        QTextLayout* layout = block.layout();
        QTextLine line = layout->lineForTextPosition(fragment.position() - block.position());
        if(!line.isValid())
        {
            // fragment has no valid position and consequently no line...
            return QRectF();
        }
        qreal top = line.position().y();
        qreal bottom = line.position().y() + line.height();
        qreal left = line.cursorToX(fragment.position() - block.position());
        qreal right = line.cursorToX((fragment.position() - block.position()) + fragment.length());
        QRectF fragmentPosition(QPointF(top, left), QPointF(bottom, right));
        return fragmentPosition.adjusted(layout->position().x(), layout->position().y(), 0, 0);
    }
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

    d->document = static_cast<KWDocument*>(document);
    d->documentModel = new CQTextDocumentModel(this, d->document, kwCanvasItem->shapeManager());
    emit documentModelChanged();

    d->updateLinkTargets();
    emit linkTargetsChanged();

}

void CQTextDocumentCanvas::gotoPage(int pageNumber, KoDocument *document)
{
    const KWDocument *kwDoc = static_cast<const KWDocument*>(document);
    KWPage currentTextDocPage = kwDoc->pageManager()->page(pageNumber);

    QRectF rect = d->canvasBase->viewConverter()->documentToView(currentTextDocPage.rect());
    canvasController()->pan(rect.topLeft().toPoint() - d->canvasBase->viewConverter()->documentToView(canvasController()->documentOffset()).toPoint());
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
    if(d->pageNumber != currentPageNumber)
    {
        d->pageNumber = currentPageNumber;
        gotoPage(d->pageNumber, d->document);
    }
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

KWDocument* CQTextDocumentCanvas::document() const
{
    return d->document;
}

QObjectList CQTextDocumentCanvas::linkTargets() const
{
    return d->linkTargets;
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
