/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "DocumentImpl.h"

#include <KActionCollection>

#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

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
    {
    }

    DocumentType::Type type;
    QGraphicsWidget *canvas;
    KoFindBase *finder;
    KoCanvasController *canvasController;
    KoZoomController *zoomController;
    QSize documentSize;
    KoDocument *document;
    bool readOnly;
};

DocumentImpl::DocumentImpl(QObject *parent)
    : QObject{parent}
    , d{new Private}
{
}

DocumentImpl::~DocumentImpl() = default;

DocumentType::Type DocumentImpl::documentType() const
{
    return d->type;
}

QGraphicsWidget *DocumentImpl::canvas() const
{
    return d->canvas;
}

KoFindBase *DocumentImpl::finder() const
{
    return d->finder;
}

KoCanvasController *DocumentImpl::canvasController() const
{
    return d->canvasController;
}

KoZoomController *DocumentImpl::zoomController() const
{
    return d->zoomController;
}

QSize DocumentImpl::documentSize() const
{
    return d->documentSize;
}

KoDocument *DocumentImpl::koDocument() const
{
    return d->document;
}

void DocumentImpl::setDocumentType(DocumentType::Type type)
{
    d->type = type;
}

void DocumentImpl::setKoDocument(KoDocument *document)
{
    d->document = document;
}

void DocumentImpl::setCanvas(QGraphicsWidget *newCanvas)
{
    d->canvas = newCanvas;
}

void DocumentImpl::setFinder(KoFindBase *newFinder)
{
    d->finder = newFinder;
}

void DocumentImpl::setReadOnly(bool readOnly)
{
    d->readOnly = readOnly;
}

void DocumentImpl::createAndSetCanvasController(KoCanvasBase *canvas)
{
    auto controller = new ComponentsKoCanvasController{new KActionCollection{this}};
    d->canvasController = controller;
    controller->setCanvas(canvas);
    if (!d->readOnly) {
        KoToolManager::instance()->addController(controller);
    }
    connect(controller, &ComponentsKoCanvasController::documentSizeChanged, this, &DocumentImpl::setDocumentSize);
}

void DocumentImpl::createAndSetZoomController(KoCanvasBase *canvas)
{
    auto zoomHandler = static_cast<KoZoomHandler *>(canvas->viewConverter());
    d->zoomController = new KoZoomController{d->canvasController, zoomHandler, new KActionCollection(this)};

    auto canvasQObject = dynamic_cast<QObject *>(canvas);
    connect(d->canvasController->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), canvasQObject, SLOT(setDocumentOffset(QPoint)));
    connect(canvasQObject, SIGNAL(canvasUpdated()), this, SIGNAL(requestViewUpdate()));
}

void DocumentImpl::setDocumentSize(const QSize &size)
{
    if (size != d->documentSize) {
        d->documentSize = size;
        Q_EMIT documentSizeChanged();
    }
}
