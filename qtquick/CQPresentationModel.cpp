/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen-Wander Hiemstra <aw.hiemstra@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "CQPresentationModel.h"

#include <stage/part/KPrDocument.h>

#include "CQPresentationCanvas.h"

class CQPresentationModel::Private
{
public:
    Private()
        : canvas(0)
        , document(0)
        , thumbnailSize(64, 64)
    {
    }

    CQPresentationCanvas *canvas;
    KPrDocument *document;

    QHash<int, QPixmap> thumbnails;
    QSizeF thumbnailSize;
};

CQPresentationModel::CQPresentationModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

CQPresentationModel::~CQPresentationModel() = default;

QHash<int, QByteArray> CQPresentationModel::roleNames() const
{
    return {
        {ThumbnailRole, "thumbnail"},
    };
}

QVariant CQPresentationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() && d->document) {
        return QVariant();
    }

    switch (role) {
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

int CQPresentationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (d->document) {
        return d->document->pageCount();
    }

    return 0;
}

QQuickitem *CQPresentationModel::canvas() const
{
    return d->canvas;
}

QSizeF CQPresentationModel::thumbnailSize() const
{
    return d->thumbnailSize;
}

void CQPresentationModel::setCanvas(QQuickItem *canvas)
{
    if (d->canvas != canvas && qobject_cast<CQPresentationCanvas *>(canvas)) {
        d->canvas = qobject_cast<CQPresentationCanvas *>(canvas);
        connect(d->canvas, SIGNAL(sourceChanged()), SLOT(canvasSourceChanged()));
        canvasSourceChanged();
        Q_EMIT canvasChanged();
    }
}

void CQPresentationModel::setThumbnailSize(const QSizeF &size)
{
    if (size != d->thumbnailSize) {
        d->thumbnailSize = size;
        d->thumbnails.clear(); // Size changed, so cache is invalid

        if (d->document) {
            Q_EMIT dataChanged(index(0, 0), index(d->document->pageCount() - 1));
        }

        Q_EMIT thumbnailSizeChanged();
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
