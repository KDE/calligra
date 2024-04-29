/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "CQSpreadsheetListModel.h"

#include <QPainter>

#include <KoZoomHandler.h>
#include <Map.h>
#include <Sheet.h>
#include <ui/SheetView.h>

#include "CQSpreadsheetCanvas.h"

class CQSpreadsheetListModel::Private
{
public:
    Private()
        : canvas(0)
        , map(0)
        , thumbnailSize(64, 64)
    {
    }

    CQSpreadsheetCanvas *canvas;
    Calligra::Sheets::Map *map;

    QHash<int, QPixmap> thumbnails;
    QSize thumbnailSize;
};

CQSpreadsheetListModel::CQSpreadsheetListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(SheetNameRole, "sheetName");
    roleNames.insert(ThumbnailRole, "thumbnail");
    setRoleNames(roleNames);
}

CQSpreadsheetListModel::~CQSpreadsheetListModel()
{
    delete d;
}

QVariant CQSpreadsheetListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !d->map) {
        return QVariant();
    }

    switch (role) {
    case SheetNameRole:
        return d->map->sheet(index.row())->sheetName();
    case ThumbnailRole: {
        if (d->thumbnails.contains(index.row())) {
            return d->thumbnails.value(index.row());
        }

        QPixmap thumbnail(d->thumbnailSize);
        QRect rect(QPoint(0, 0), d->thumbnailSize);

        QPainter p(&thumbnail);

        p.fillRect(rect, Qt::white);

        Calligra::Sheets::SheetView sheetView(d->map->sheet(index.row()));

        qreal zoom = 0.5;
        KoZoomHandler zoomHandler;
        zoomHandler.setZoom(zoom);
        p.setClipRect(rect);
        p.scale(zoom, zoom);
        sheetView.setViewConverter(&zoomHandler);

        QRectF area = zoomHandler.viewToDocument(rect);
        QRect range = sheetView.sheet()->documentToCellCoordinates(area).adjusted(0, 0, 2, 2);
        sheetView.setPaintCellRange(range);
        sheetView.paintCells(p, area, QPointF(0, 0));

        d->thumbnails.insert(index.row(), thumbnail);
        return thumbnail;
    }
    default:
        break;
    }

    return QVariant();
}

int CQSpreadsheetListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (d->map) {
        return d->map->count();
    }

    return 0;
}

QObject *CQSpreadsheetListModel::canvas() const
{
    return d->canvas;
}

QSize CQSpreadsheetListModel::thumbnailSize() const
{
    return d->thumbnailSize;
}

void CQSpreadsheetListModel::setCanvas(QObject *canvas)
{
    if (d->canvas != canvas) {
        d->canvas = qobject_cast<CQSpreadsheetCanvas *>(canvas);
        Q_ASSERT(d->canvas);
        if (d->map) {
            beginRemoveRows(QModelIndex(), 0, d->map->count());
            endRemoveRows();
        }
        d->map = d->canvas->documentMap();
        beginInsertRows(QModelIndex(), 0, d->map->count());
        endInsertRows();
        Q_EMIT canvasChanged();
    }
}

void CQSpreadsheetListModel::setThumbnailSize(const QSize &size)
{
    if (size != d->thumbnailSize) {
        d->thumbnailSize = size;
        d->thumbnails.clear();
        Q_EMIT thumbnailSizeChanged();
    }
}
