/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen-Wander Hiemstra <aw.hiemstra@gmail.com>
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

#include "CQPresentationModel.h"

#include <stage/part/KPrDocument.h>

#include "CQPresentationCanvas.h"

class CQPresentationModel::Private
{
public:
    Private() : canvas(0), document(0), thumbnailSize(64, 64) { }

    CQPresentationCanvas* canvas;
    KPrDocument* document;

    QHash<int, QPixmap> thumbnails;
    QSizeF thumbnailSize;
};

CQPresentationModel::CQPresentationModel(QObject* parent)
    : QAbstractListModel(parent), d(new Private)
{
    QHash<int, QByteArray> roles;
    roles.insert(ThumbnailRole, "thumbnail");
    setRoleNames(roles);
}

CQPresentationModel::~CQPresentationModel()
{
    delete d;

}

QVariant CQPresentationModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() && d->document) {
        return QVariant();
    }

    switch(role) {
        case ThumbnailRole: {
            if (d->thumbnails.contains(index.row())) {
                QPixmap thumb = d->thumbnails.value(index.row());

                if (!thumb.isNull()) {
                    return thumb;
                }

                d->thumbnails.remove(index.row());
            }

            QPixmap pixmap = d->document->pageThumbnail(d->document->pageByIndex(index.row(), false), d->thumbnailSize.toSize());
            d->thumbnails.insert(index.row(), pixmap);
            return pixmap;
        }
        default:
            break;
    }

    return QVariant();
}

QPixmap CQPresentationModel::thumbnail(int index) const
{
    return data(this->index(index), ThumbnailRole).value<QPixmap>();
}

int CQPresentationModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (d->document) {
        return d->document->pageCount();
    }

    return 0;
}

QDeclarativeItem* CQPresentationModel::canvas() const
{
    return d->canvas;
}

QSizeF CQPresentationModel::thumbnailSize() const
{
    return d->thumbnailSize;
}

void CQPresentationModel::setCanvas(QDeclarativeItem* canvas)
{
    if (d->canvas != canvas && qobject_cast<CQPresentationCanvas*>(canvas))
    {
        d->canvas = qobject_cast<CQPresentationCanvas*>(canvas);
        connect(d->canvas, SIGNAL(sourceChanged()), SLOT(canvasSourceChanged()));
        canvasSourceChanged();
        emit canvasChanged();
    }
}

void CQPresentationModel::setThumbnailSize(const QSizeF& size)
{
    if (size != d->thumbnailSize) {
        d->thumbnailSize = size;
        d->thumbnails.clear(); //Size changed, so cache is invalid

        if (d->document) {
            emit dataChanged(index(0, 0), index(d->document->pageCount() - 1));
        }

        emit thumbnailSizeChanged();
    }
}

void CQPresentationModel::canvasSourceChanged()
{
    if (d->canvas->document()) {
        if (d->document) {
            beginRemoveRows(QModelIndex(), 0, d->document->pageCount());
            endRemoveRows();
        }
        d->document = d->canvas->document();
        beginInsertRows(QModelIndex(), 0, d->document->pageCount());
        endInsertRows();
    }
}
