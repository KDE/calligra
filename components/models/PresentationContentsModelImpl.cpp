/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "PresentationContentsModelImpl.h"

#include <KoPAPageBase.h>
#include <KoPageLayout.h>
#include <stage/part/KPrDocument.h>

using namespace Calligra::Components;

class PresentationContentsModelImpl::Private
{
public:
    Private() = default;

    KPrDocument *document;

    QHash<int, QImage> thumbnails;
    QSize thumbnailSize;
};

PresentationContentsModelImpl::PresentationContentsModelImpl(KoDocument *document)
    : d{new Private}
{
    d->document = qobject_cast<KPrDocument *>(document);
    Q_ASSERT(d->document);
}

PresentationContentsModelImpl::~PresentationContentsModelImpl()
{
    delete d;
}

int PresentationContentsModelImpl::rowCount() const
{
    return d->document->pageCount();
}

QVariant PresentationContentsModelImpl::data(int index, ContentsModel::Role role) const
{
    KoPAPageBase *page = d->document->pageByIndex(index, false);
    switch (role) {
    case ContentsModel::TitleRole:
        return QString("%1: %2").arg(index + 1).arg(page->name());
    case ContentsModel::LevelRole:
        return 0;
    case ContentsModel::ThumbnailRole: {
        if (d->thumbnails.contains(index)) {
            return d->thumbnails.value(index);
        }

        if (d->thumbnailSize.isNull()) {
            return QImage{};
        }

        QImage thumb = page->thumbImage(d->thumbnailSize);
        d->thumbnails.insert(index, thumb);
        return thumb;
    }
    case ContentsModel::ContentIndexRole:
        return index;
    default:
        return QVariant();
    }
}

void PresentationContentsModelImpl::setThumbnailSize(const QSize &size)
{
    d->thumbnailSize = size;
    d->thumbnails.clear();
}

QImage PresentationContentsModelImpl::thumbnail(int index, int width) const
{
    KoPAPageBase *page = d->document->pageByIndex(index, false);
    QSize thumbSize{width, int((page->pageLayout().height / page->pageLayout().width) * width)};
    return page->thumbImage(thumbSize);
}
