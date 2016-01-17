/*
 * This file is part of the KDE project
 *
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

#include "TextDocumentImpl.h"

#include <QtWidgets/QGraphicsWidget>

#include <kactioncollection.h>

#include <KWPart.h>
#include <KWDocument.h>
#include <KWCanvasItem.h>
#include <KoFindText.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextLayout>

#include "ComponentsKoCanvasController.h"
#include <libs/textlayout/KoTextShapeData.h>

using namespace Calligra::Components;

class TextDocumentImpl::Private
{
public:
    Private() : part{nullptr}, document{nullptr}
    { }

    KWPart* part;
    KWDocument* document;
    KWCanvasItem* canvas;
    QTimer indexChangedDelay;

    QList< QPair< QRectF, QUrl > > links;

    QList<KoShape*> deepShapeFind(QList<KoShape*> shapes)
    {
        QList<KoShape*> allShapes;
        foreach(KoShape* shape, shapes) {
            allShapes.append(shape);
            KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
            if(container) {
                allShapes.append(deepShapeFind(container->shapes()));
            }
        }
        return allShapes;
    }

    void updateLinkTargets()
    {
        links.clear();

        if(!canvas)
            return;

        foreach(const KoShape* shape, canvas->shapeManager()->shapes()) {
            if(!shape->hyperLink().isEmpty()) {
                QRectF rect = shape->boundingRect();
                while(KoShapeContainer* parent = shape->parent()) {
                    rect.translate(parent->position());
                }
                links.append(QPair<QRectF, QUrl>(rect, QUrl(shape->hyperLink())));
            }
        }

        QList<QTextDocument*> texts;
        KoFindText::findTextInShapes(canvas->shapeManager()->shapes(), texts);
        QList<KoShape*> allShapes = deepShapeFind(canvas->shapeManager()->shapes());
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
                            QRectF rect = getFragmentPosition(block, fragment);
                            foreach(KoShape* shape, allShapes) {
                                KoTextShapeData *shapeData = dynamic_cast<KoTextShapeData*>(shape->userData());
                                if (!shapeData)
                                    continue;
                                if(shapeData->document() == text)
                                {
                                    rect.translate(shape->position());
                                    while(KoShapeContainer* parent = shape->parent()) {
                                        rect.translate(parent->position());
                                    }
                                    break;
                                }
                            }
                            KWPage page = document->pageManager()->page(rect.top());
                            //rect.translate(page.rightMargin(), page.topMargin());
                            //rect = canvas->viewConverter()->documentToView(rect);
                            //rect.translate(0, page.pageNumber() * (page.topMargin() + page.bottomMargin()) + 20);
                            rect.translate(0, (page.pageNumber() - 1) * (page.topMargin() + 20));
                            links.append(QPair<QRectF, QUrl>(canvas->viewConverter()->documentToView(rect), QUrl(format.anchorHref())));
                        }
                    }
                }
            }
        }
        qDebug() << links;
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
        qreal top = line.position().y() + (line.height() / 2);
        qreal bottom = top + line.height();
        qreal left = line.cursorToX(fragment.position() - block.position());
        qreal right = line.cursorToX((fragment.position() - block.position()) + fragment.length());
        QRectF fragmentPosition(QPointF(left, top), QPointF(right, bottom));
        return fragmentPosition.adjusted(layout->position().x(), layout->position().y(), 0, 0);
    }

    static const float wiggleFactor;
};

const float Calligra::Components::TextDocumentImpl::Private::wiggleFactor{ 4.f };

TextDocumentImpl::TextDocumentImpl(QObject* parent)
    : DocumentImpl{parent}, d{new Private}
{
    setDocumentType(DocumentType::TextDocument);
    d->indexChangedDelay.setInterval(0);
    connect(&d->indexChangedDelay, SIGNAL(timeout()), this, SIGNAL(currentIndexChanged()));
}

TextDocumentImpl::~TextDocumentImpl()
{
    delete d;
}

bool TextDocumentImpl::load(const QUrl& url)
{
    if(d->part) {
        delete d->part;
        delete d->document;
    }

    d->part = new KWPart{this};
    d->document = new KWDocument{d->part};
    setKoDocument(d->document);
    d->part->setDocument(d->document);

    d->document->setAutoSave(0);
    d->document->setCheckAutoSaveFile(false);

    bool retval = d->document->openUrl(url);

    d->canvas = static_cast<KWCanvasItem*>(d->part->canvasItem(d->document));

    createAndSetCanvasController(d->canvas);
    createAndSetZoomController(d->canvas);
    zoomController()->setPageSize(d->document->pageManager()->begin().rect().size());
    connect(d->canvas, SIGNAL(documentSize(QSizeF)), zoomController(), SLOT(setDocumentSize(QSizeF)));

    d->canvas->updateSize();

    setCanvas(d->canvas);
    connect(canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), &d->indexChangedDelay, SLOT(start()));

    d->updateLinkTargets();

    return retval;
}

int TextDocumentImpl::currentIndex()
{
    QPointF newPoint = d->canvas->viewConverter()->viewToDocument(canvasController()->documentOffset());
    KWPage page = d->document->pageManager()->page(newPoint.y());
    return page.pageNumber();
}

void TextDocumentImpl::setCurrentIndex(int newValue)
{
    KWPage newPage = d->document->pageManager()->page(newValue + 1);
    QRectF newRect = d->canvas->viewConverter()->documentToView(newPage.rect());
    canvasController()->setScrollBarValue(newRect.topLeft().toPoint());
    emit requestViewUpdate();
    emit currentIndexChanged();
}

int TextDocumentImpl::indexCount() const
{
    return d->document->pageCount();
}

QUrl TextDocumentImpl::urlAtPoint(QPoint point)
{
    qDebug() << Q_FUNC_INFO << point + (d->canvas->documentOffset() / zoomController()->zoomAction()->effectiveZoom());
    for( const QPair< QRectF, QUrl >& link : d->links )
    {
        QRectF hitTarget{
            link.first.x() - Private::wiggleFactor,
            link.first.y() - Private::wiggleFactor,
            link.first.width() + Private::wiggleFactor * 2,
            link.first.height() + Private::wiggleFactor * 2
        };

        if( hitTarget.contains( point + (d->canvas->documentOffset() / zoomController()->zoomAction()->effectiveZoom()) ) )
            return link.second;
    }
    return QUrl();
}

QObject* TextDocumentImpl::part() const
{
    return d->part;
}
