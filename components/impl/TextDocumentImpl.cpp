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

#include "TextDocumentImpl.h"

#include <QtWidgets/QGraphicsWidget>

#include <kservice.h>

#include <KWPart.h>
#include <KWDocument.h>

using namespace Calligra::Components;

class TextDocumentImpl::Private
{
public:
    Private() : part{nullptr}, document{nullptr}
    { }

    KWPart* part;
    KWDocument* document;
};

TextDocumentImpl::TextDocumentImpl(QObject* parent)
    : DocumentImpl{parent}, d{new Private}
{
    setDocumentType(Global::TextDocumentType);
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
    d->part->setDocument(d->document);

    bool retval = d->document->openUrl(url);

    setCanvas(static_cast<QGraphicsWidget*>(d->part->canvasItem(d->document)));

    return retval;
}
