/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "SpreadsheetContentsModelImpl.h"

#include <QPainter>

#include <KoZoomHandler.h>
#include <sheets/core/Map.h>
#include <sheets/core/Sheet.h>
#include <sheets/part/Doc.h>
#include <sheets/ui/SheetView.h>

using namespace Calligra::Components;

class SpreadsheetContentsModelImpl::Private
{
public:
    Private() = default;

    QImage renderThumbnail(Calligra::Sheets::Sheet *sheet, int width, int height);

    Calligra::Sheets::Doc *document;

    QHash<int, QImage> thumbnails;

    QSize thumbnailSize;
};

SpreadsheetContentsModelImpl::SpreadsheetContentsModelImpl(KoDocument *document)
    : d{new Private}
{
    d->document = qobject_cast<Calligra::Sheets::Doc *>(document);
    Q_ASSERT(d->document);
}

SpreadsheetContentsModelImpl::~SpreadsheetContentsModelImpl()
{
    delete d;
}

int SpreadsheetContentsModelImpl::rowCount() const
{
    return d->document->map()->count();
}

QVariant SpreadsheetContentsModelImpl::data(int index, ContentsModel::Role role) const
{
    Calligra::Sheets::Sheet *sheet = dynamic_cast<Calligra::Sheets::Sheet *>(d->document->map()->sheet(index));
    switch (role) {
    case ContentsModel::TitleRole:
        return sheet->sheetName();
    case ContentsModel::LevelRole:
        return 0;
    case ContentsModel::ThumbnailRole: {
        if (d->thumbnails.contains(index)) {
            return d->thumbnails.value(index);
        }

        if (d->thumbnailSize.isNull()) {
            return QImage{};
        }

        QImage thumbnail = d->renderThumbnail(sheet, d->thumbnailSize.width(), d->thumbnailSize.height());
        d->thumbnails.insert(index, thumbnail);
        return thumbnail;
    }
    case ContentsModel::ContentIndexRole:
        return index;
    default:
        return QVariant();
    }
}

void SpreadsheetContentsModelImpl::setThumbnailSize(const QSize &size)
{
    d->thumbnailSize = size;
    d->thumbnails.clear();
}

QImage SpreadsheetContentsModelImpl::thumbnail(int index, int width) const
{
    Calligra::Sheets::Sheet *sheet = dynamic_cast<Calligra::Sheets::Sheet *>(d->document->map()->sheet(index));
    return d->renderThumbnail(sheet, width, width);
}

QImage SpreadsheetContentsModelImpl::Private::renderThumbnail(Calligra::Sheets::Sheet *sheet, int width, int height)
{
    QImage thumbnail{width, height, QImage::Format_RGB32};
    QRect rect{0, 0, width, height};

    QPainter p{&thumbnail};

    p.fillRect(rect, Qt::white);

    Calligra::Sheets::SheetView sheetView{sheet};

    qreal zoom = 0.5;
    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(zoom);
    p.setClipRect(rect);
    p.scale(zoom, zoom);
    sheetView.setViewConverter(&zoomHandler);

    QRectF area = zoomHandler.viewToDocument(rect);
    QRect range = sheet->documentToCellCoordinates(area).adjusted(0, 0, 2, 2);
    sheetView.setPaintCellRange(range);
    sheetView.paintCells(p, area, QPointF(0, 0));

    return thumbnail;
}
