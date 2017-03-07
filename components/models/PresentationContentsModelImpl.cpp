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

#include "PresentationContentsModelImpl.h"

#include <stage/part/KPrDocument.h>
#include <KoPAPageBase.h>
#include <KoPageLayout.h>

using namespace Calligra::Components;

class PresentationContentsModelImpl::Private
{
public:
    Private()
    { }

    KPrDocument* document;

    QHash<int, QImage> thumbnails;
    QSize thumbnailSize;
};

PresentationContentsModelImpl::PresentationContentsModelImpl(KoDocument* document)
    : d{new Private}
{
    d->document = qobject_cast<KPrDocument*>(document);
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
    KoPAPageBase* page = d->document->pageByIndex(index, false);
    switch(role) {
        case ContentsModel::TitleRole:
            return QString("%1: %2").arg(index + 1).arg(page->name());
        case ContentsModel::LevelRole:
            return 0;
        case ContentsModel::ThumbnailRole: {
            if(d->thumbnails.contains(index)) {
                return d->thumbnails.value(index);
            }

            if(d->thumbnailSize.isNull()) {
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

void PresentationContentsModelImpl::setThumbnailSize(const QSize& size)
{
    d->thumbnailSize = size;
    d->thumbnails.clear();
}

QImage PresentationContentsModelImpl::thumbnail(int index, int width) const
{
    KoPAPageBase* page = d->document->pageByIndex(index, false);
    QSize thumbSize{width, int((page->pageLayout().height / page->pageLayout().width) * width)};
    return page->thumbImage(thumbSize);
}
