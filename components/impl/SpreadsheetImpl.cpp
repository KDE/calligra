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

#include "SpreadsheetImpl.h"

#include <QtWidgets/QGraphicsWidget>
#include <KoToolManager.h>

#include <KoZoomController.h>
#include <KoViewConverter.h>
#include <sheets/part/Part.h>
#include <sheets/part/Doc.h>
#include <sheets/part/CanvasItem.h>
#include <sheets/Map.h>
#include <sheets/Sheet.h>
#include <Damages.h>

using namespace Calligra::Components;

class SpreadsheetImpl::Private
{
public:
    Private() : part{nullptr}, document{nullptr}
    { }

    Calligra::Sheets::Part* part;
    Calligra::Sheets::Doc* document;
    Calligra::Sheets::CanvasItem* canvas;
};

SpreadsheetImpl::SpreadsheetImpl(QObject* parent)
    : DocumentImpl{parent}, d{new Private}
{
    setDocumentType(DocumentType::Spreadsheet);
}

SpreadsheetImpl::~SpreadsheetImpl()
{
    delete d;
}

bool SpreadsheetImpl::load(const QUrl& url)
{
    if(d->part) {
        delete d->part;
        delete d->document;
    }

    d->part = new Calligra::Sheets::Part{this};
    d->document = new Calligra::Sheets::Doc{d->part};
    setKoDocument(d->document);
    d->part->setDocument(d->document);

    bool retval = d->document->openUrl(url);

    d->canvas = static_cast<Calligra::Sheets::CanvasItem*>(d->part->canvasItem(d->document));

    createAndSetCanvasController(d->canvas);
    createAndSetZoomController(d->canvas);
    connect(d->canvas, &Calligra::Sheets::CanvasItem::documentSizeChanged, this, &SpreadsheetImpl::updateDocumentSize);

    auto sheet = d->document->map()->sheet(0);
    if(sheet) {
        updateDocumentSize(sheet->documentSize().toSize());
    }

    setCanvas(d->canvas);

    return retval;
}

int SpreadsheetImpl::currentIndex()
{
    if (d->document && d->document->map() && d->canvas->activeSheet()) {
        return d->document->map()->indexOf(d->canvas->activeSheet());
    } else {
        return -1;
    }
}

void SpreadsheetImpl::setCurrentIndex(int newValue)
{
    if(newValue != currentIndex()) {
        d->canvas->setActiveSheet(d->document->map()->sheet(newValue));
        emit currentIndexChanged();
    }
}

void SpreadsheetImpl::updateDocumentSize(const QSize& size)
{
    QRectF activeRect = d->canvas->viewConverter()->documentToView(d->canvas->activeSheet()->cellCoordinatesToDocument(d->canvas->activeSheet()->usedArea(true)));
    zoomController()->setDocumentSize(activeRect.size(), false);
    setDocumentSize(activeRect.size().toSize());
}
