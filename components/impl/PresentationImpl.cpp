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

#include "PresentationImpl.h"
#include "PresentationKoPAView.h"

#include <QGraphicsWidget>
#include <QMimeDatabase>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>
#include <QDebug>

#include <stage/part/KPrPart.h>
#include <stage/part/KPrDocument.h>
#include <libs/textlayout/KoTextShapeData.h>
#include <KoFindText.h>
#include <KoPACanvasItem.h>
#include <KoPAPageBase.h>
#include <KoShape.h>
#include <KoZoomController.h>

using namespace Calligra::Components;

class PresentationImpl::Private
{
public:
    Private() : part{nullptr}, document{nullptr}
    { }

    KPrPart* part;
    KPrDocument* document;

    PresentationKoPAView* koPaView;

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

        if(!koPaView || !koPaView->activePage())
            return;

        foreach(const KoShape* shape, koPaView->activePage()->shapes()) {
            if(!shape->hyperLink().isEmpty()) {
                QRectF rect = shape->boundingRect();
                for (KoShapeContainer* parent = shape->parent();
                     parent; parent = parent->parent()) {
                    rect.translate(parent->position());
                }
                links.append(QPair<QRectF, QUrl>(rect, QUrl(shape->hyperLink())));
            }
        }

        QList<QTextDocument*> texts;
        KoFindText::findTextInShapes(koPaView->activePage()->shapes(), texts);
        QList<KoShape*> allShapes = deepShapeFind(koPaView->activePage()->shapes());
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
                                    for (KoShapeContainer* parent = shape->parent();
                                         parent; parent = parent->parent()) {
                                        rect.translate(parent->position());
                                    }
                                    break;
                                }
                            }
                            links.append(QPair<QRectF, QUrl>(koPaView->kopaCanvas()->viewConverter()->documentToView(rect), QUrl(format.anchorHref())));
                        }
                    }
                }
            }
        }
        qDebug() << "Discovered the following links in the slide:" << links;
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

const float Calligra::Components::PresentationImpl::Private::wiggleFactor{ 4.f };

PresentationImpl::PresentationImpl(QObject* parent)
    : DocumentImpl{parent}, d{new Private}
{
    setDocumentType(DocumentType::Presentation);
}

PresentationImpl::~PresentationImpl()
{
    delete d;
}

bool PresentationImpl::load(const QUrl& url)
{
    delete d->part;
    delete d->document;

    d->part = new KPrPart{this};
    d->document = new KPrDocument{d->part};
    setKoDocument(d->document);
    d->part->setDocument(d->document);

    bool retval = false;
    if (url.scheme() == QStringLiteral("template")) {
        bool ok = d->document->loadNativeFormat(url.toString().mid(11));
        d->document->setModified(false);
        d->document->undoStack()->clear();

        if (ok) {
            QString mimeType = QMimeDatabase().mimeTypeForUrl(url).name();
            // in case this is a open document template remove the -template from the end
            mimeType.remove( QRegExp( "-template$" ) );
            d->document->setMimeTypeAfterLoading(mimeType);
            d->document->resetURL();
            d->document->setEmpty();
        } else {
            // some kind of error reporting thing here... failed to load template, tell the user
            // why their canvas is so terribly empty.
            d->document->initEmpty();
        }
        d->document->setModified(true);
        retval = true;
    } else {
        retval = d->document->openUrl(url);
    }

    auto canvas = static_cast<KoPACanvasItem*>(d->part->canvasItem(d->document));

    createAndSetCanvasController(canvas);

    d->koPaView = new PresentationKoPAView(canvasController(), canvas, d->document);
    canvas->setView(d->koPaView);

    createAndSetZoomController(canvas);
    d->koPaView->setZoomController(zoomController());
    d->koPaView->connectToZoomController();

    KoPAPageBase* page = d->document->pageByIndex(0, false);
    if(page) {
        d->koPaView->doUpdateActivePage(page);
    }
    d->updateLinkTargets();

    setCanvas(canvas);

    return retval;
}

int PresentationImpl::currentIndex()
{
    if (d->document && d->koPaView && d->koPaView->activePage()) {
        return d->document->pageIndex(d->koPaView->activePage());
    } else {
        return -1;
    }
}

void PresentationImpl::setCurrentIndex(int newValue)
{
    if(newValue != currentIndex()) {
        d->koPaView->doUpdateActivePage(d->document->pageByIndex(newValue, false));
        d->updateLinkTargets();
        emit requestViewUpdate();
        emit currentIndexChanged();
    }
}

int PresentationImpl::indexCount() const
{
    return d->document->pageCount();
}

QUrl PresentationImpl::urlAtPoint(QPoint point)
{
    for( const QPair< QRectF, QUrl >& link : d->links )
    {
        QRectF hitTarget{
            link.first.x() - Private::wiggleFactor,
            link.first.y() - Private::wiggleFactor,
            link.first.width() + Private::wiggleFactor * 2,
            link.first.height() + Private::wiggleFactor * 2
        };

        if( hitTarget.contains( point ) )
        {
            return link.second;
        }
    }
    return QUrl();
}

QObject* PresentationImpl::part() const
{
    return d->part;
}
