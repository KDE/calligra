/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "DocumentImpl.h"

using namespace Calligra::Components;

class DocumentImpl::Private
{
public:
    Private() : type{Global::UnknownType},
        canvas{nullptr},
        finder{nullptr},
        canvasController{nullptr},
        zoomController{nullptr}
    { }

    Global::DocumentType type;
    QGraphicsWidget* canvas;
    KoFindBase* finder;
    KoCanvasController* canvasController;
    KoZoomController* zoomController;
};

DocumentImpl::DocumentImpl(QObject* parent)
    : QObject{parent}, d{new Private}
{

}

DocumentImpl::~DocumentImpl()
{

}

Global::DocumentType DocumentImpl::documentType() const
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

void DocumentImpl::setDocumentType(Global::DocumentType type)
{
    d->type = type;
}

void DocumentImpl::setCanvas(QGraphicsWidget* newCanvas)
{
    d->canvas = newCanvas;
}

void DocumentImpl::setFinder(KoFindBase* newFinder)
{
    d->finder = newFinder;
}

void DocumentImpl::setCanvasController(KoCanvasController* controller)
{
    d->canvasController = controller;
}

void DocumentImpl::setZoomController(KoZoomController* controller)
{
    d->zoomController = controller;
}
