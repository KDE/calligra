/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
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

#include "CQPresentationCanvas.h"

#include <QStyleOptionGraphicsItem>

#include "CQCanvasController.h"
#include <KoPart.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KoPADocument.h>
#include <KoPACanvasItem.h>
#include <KDebug>
#include <KActionCollection>
#include <QGraphicsWidget>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>

#include <KoPAPageBase.h>

#include <stage/part/KPrDocument.h>
#include <KoShapeManager.h>

#include "CQPresentationView.h"

class CQPresentationCanvas::Private
{
public:
    Private() : canvasBase(0), view(0), document(0), currentSlide(0) { }

    KoCanvasBase* canvasBase;
    CQPresentationView* view;
    KPrDocument* document;

    int currentSlide;
    QSizeF pageSize;
    QObjectList linkTargets;

    void updateLinkTargets()
    {
        qDeleteAll(linkTargets);
        linkTargets.clear();

        if(!view)
            return;
        foreach(const KoShape* shape, view->activePage()->shapes()) {
            if(!shape->hyperLink().isEmpty()) {
                QObject * obj = new QObject(view);
                obj->setProperty("linkRect", shape->boundingRect());
                obj->setProperty("linkTarget", QUrl(shape->hyperLink()));
                linkTargets.append(obj);
            }
        }

        QList<QTextDocument*> texts;
        KoFindText::findTextInShapes(view->activePage()->shapes(), texts);
        foreach(QTextDocument* text, texts) {
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
                            QObject * obj = new QObject(view);
                            QRectF rect = getFragmentPosition(block, fragment);
                            obj->setProperty("linkRect", canvasBase->viewConverter()->documentToView(rect));
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

CQPresentationCanvas::CQPresentationCanvas(QDeclarativeItem* parent)
    : CQCanvasBase(parent), d(new Private)
{

}

CQPresentationCanvas::~CQPresentationCanvas()
{
    delete d;
}

int CQPresentationCanvas::currentSlide() const
{
    return d->currentSlide;
}

QObjectList CQPresentationCanvas::linkTargets() const
{
    return d->linkTargets;
}

KPrDocument* CQPresentationCanvas::document() const
{
    return d->document;
}

QSizeF CQPresentationCanvas::pageSize() const
{
    return d->pageSize;
}

void CQPresentationCanvas::setCurrentSlide(int slide)
{
    slide = qBound(0, slide, d->document->pageCount() - 1);
    if(slide != d->currentSlide) {
        d->currentSlide = slide;
        d->view->doUpdateActivePage(d->document->pageByIndex(slide, false));
        d->pageSize = d->view->activePage()->size();
        emit currentSlideChanged();
        d->updateLinkTargets();
        emit linkTargetsChanged();
    }
}

void CQPresentationCanvas::render(QPainter* painter, const QRectF& target)
{
    QStyleOptionGraphicsItem option;
    option.exposedRect = target;
    option.rect = target.toAlignedRect();
    d->canvasBase->canvasItem()->paint(painter, &option);
}

void CQPresentationCanvas::openFile(const QString& uri)
{
    KService::Ptr service = KService::serviceByDesktopName("stagepart");
    if(service.isNull()) {
        qWarning("Unable to load Stage plugin, aborting!");
        return;
    }

    KoPart *part = service->createInstance<KoPart>();
    d->document = dynamic_cast<KPrDocument*>(part->document());
    d->document->setAutoSave(0);
    d->document->setCheckAutoSaveFile(false);
    d->document->openUrl (KUrl (uri));

    KoPACanvasItem *paCanvasItem = static_cast<KoPACanvasItem*>(part->canvasItem());
    d->canvasBase = paCanvasItem;
    createAndSetCanvasControllerOn(d->canvasBase);

    d->view = new CQPresentationView(canvasController(), static_cast<KoPACanvasBase*>(d->canvasBase), dynamic_cast<KPrDocument*>(d->document));
    paCanvasItem->setView(d->view);

    createAndSetZoomController(d->canvasBase);
    d->view->setZoomController(zoomController());
    d->view->connectToZoomController();

    QGraphicsWidget *graphicsWidget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
    graphicsWidget->setParentItem(this);
    graphicsWidget->installEventFilter(this);
    graphicsWidget->setVisible(true);
    graphicsWidget->setGeometry(x(), y(), width(), height());

    d->view->doUpdateActivePage(d->document->pageByIndex(0, false));
    d->pageSize = d->view->activePage()->size();
    emit currentSlideChanged();

    d->updateLinkTargets();
    emit linkTargetsChanged();
}

void CQPresentationCanvas::createAndSetCanvasControllerOn(KoCanvasBase* canvas)
{
    //TODO: pass a proper action collection
    CQCanvasController *controller = new CQCanvasController(new KActionCollection(this));
    setCanvasController(controller);
    controller->setCanvas(canvas);
    KoToolManager::instance()->addController (controller);
}

void CQPresentationCanvas::createAndSetZoomController(KoCanvasBase* canvas)
{
    KoZoomHandler* zoomHandler = static_cast<KoZoomHandler*> (canvas->viewConverter());
    setZoomController(new KoZoomController(canvasController(),
                                           zoomHandler,
                                           new KActionCollection(this)));

    KoPACanvasItem* canvasItem = static_cast<KoPACanvasItem*>(canvas);

    // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
    connect (canvasController()->proxyObject,
                SIGNAL(moveDocumentOffset(QPoint)), canvasItem, SLOT(slotSetDocumentOffset(QPoint)));
    // whenever the size of the document viewed in the canvas changes, inform the zoom controller
    connect (canvasItem, SIGNAL(documentSize(QSize)), this, SLOT(updateDocumentSize(QSize)));
    canvasItem->updateSize();
    canvasItem->update();
}

void CQPresentationCanvas::updateDocumentSize(const QSize& size)
{
    zoomController()->setDocumentSize(d->canvasBase->viewConverter()->viewToDocument(size), false);
}

void CQPresentationCanvas::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvasBase) {
        QGraphicsWidget *widget = dynamic_cast<QGraphicsWidget*>(d->canvasBase);
        if (widget) {
            widget->setGeometry(newGeometry);
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}
