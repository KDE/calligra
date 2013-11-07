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

#include <kservice.h>
#include <kactioncollection.h>

#include <KWPart.h>
#include <KWDocument.h>
#include <KWCanvasItem.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include "ComponentsKoCanvasController.h"

using namespace Calligra::Components;

class TextDocumentImpl::Private
{
public:
    Private() : part{nullptr}, document{nullptr}, currentPage{-1}
    { }

    KWPart* part;
    KWDocument* document;
    KWCanvasItem* canvas;
    int currentPage;
};

TextDocumentImpl::TextDocumentImpl(QObject* parent)
    : DocumentImpl{parent}, d{new Private}
{
    setDocumentType(DocumentType::TextDocument);
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

    return retval;
}

int TextDocumentImpl::currentIndex()
{
    return d->currentPage;
}

void TextDocumentImpl::setCurrentIndex(int newValue)
{
    KWPage newPage = d->document->pageManager()->page(newValue + 1);
    QRectF newRect = d->canvas->viewConverter()->documentToView(newPage.rect());
    canvasController()->setScrollBarValue(newRect.topLeft().toPoint());
    emit currentIndexChanged();
}

void TextDocumentImpl::indexCount() const
{
    return d->document->pageCount();
}
