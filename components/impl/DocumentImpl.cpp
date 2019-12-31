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

#include "DocumentImpl.h"

#include <kactioncollection.h>

#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>

#include "ComponentsKoCanvasController.h"

using namespace Calligra::Components;

class DocumentImpl::Private
{
public:
    Private()
        : type{DocumentType::Unknown}
        , canvas{nullptr}
        , finder{nullptr}
        , canvasController{nullptr}
        , zoomController{nullptr}
        , document{nullptr}
        , readOnly{false}
    { }

    DocumentType::Type type;
    QGraphicsWidget* canvas;
    KoFindBase* finder;
    KoCanvasController* canvasController;
    KoZoomController* zoomController;
    QSize documentSize;
    KoDocument* document;
    bool readOnly;
};

DocumentImpl::DocumentImpl(QObject* parent)
    : QObject{parent}, d{new Private}
{

}

DocumentImpl::~DocumentImpl()
{

}

DocumentType::Type DocumentImpl::documentType() const
{
    return d->type;
}

QGraphicsWidget* DocumentImpl::canvas() const
{
    return d->canvas;
}

KoFindBase* DocumentImpl::finder() const
{
    return d->finder;
}

KoCanvasController* DocumentImpl::canvasController() const
{
    return d->canvasController;
}

KoZoomController* DocumentImpl::zoomController() const
{
    return d->zoomController;
}

QSize DocumentImpl::documentSize() const
{
    return d->documentSize;
}

KoDocument* DocumentImpl::koDocument() const
{
    return d->document;
}

void DocumentImpl::setDocumentType(DocumentType::Type type)
{
    d->type = type;
}

void DocumentImpl::setKoDocument(KoDocument* document)
{
    d->document = document;
}

void DocumentImpl::setCanvas(QGraphicsWidget* newCanvas)
{
    d->canvas = newCanvas;
}

void DocumentImpl::setFinder(KoFindBase* newFinder)
{
    d->finder = newFinder;
}

void DocumentImpl::setReadOnly(bool readOnly)
{
    d->readOnly = readOnly;
}

void DocumentImpl::createAndSetCanvasController(KoCanvasBase* canvas)
{
    auto controller = new ComponentsKoCanvasController{new KActionCollection{this}};
    d->canvasController = controller;
    controller->setCanvas(canvas);
    if (!d->readOnly) {
        KoToolManager::instance()->addController(controller);
    }
    connect(controller, &ComponentsKoCanvasController::documentSizeChanged, this, &DocumentImpl::setDocumentSize);
}

void DocumentImpl::createAndSetZoomController(KoCanvasBase* canvas)
{
    auto zoomHandler = static_cast<KoZoomHandler*>(canvas->viewConverter());
    d->zoomController = new KoZoomController{d->canvasController, zoomHandler, new KActionCollection(this)};

    auto canvasQObject = dynamic_cast<QObject*>(canvas);
    connect(d->canvasController->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), canvasQObject, SLOT(setDocumentOffset(QPoint)));
    connect(canvasQObject, SIGNAL(canvasUpdated()), this, SIGNAL(requestViewUpdate()));
}

void DocumentImpl::setDocumentSize(const QSize& size)
{
    if(size != d->documentSize) {
        d->documentSize = size;
        emit documentSizeChanged();
    }
}
