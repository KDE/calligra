/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
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

#include "CQTextDocumentModel.h"

#include "CQImageProvider.h"

#include <KWPage.h>
#include <KWDocument.h>

CQTextDocumentModel::CQTextDocumentModel(QObject* parent, KWDocument* document, KoShapeManager *shapemanager)
    : QAbstractListModel(parent), kw_document(document), kw_shapemanager(shapemanager), m_thumbnailSize(QSize(512, 512))
{
}

int CQTextDocumentModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return kw_document->pageManager()->pageCount();
}

QVariant CQTextDocumentModel::data(const QModelIndex& index, int role) const
{

    if (!kw_document || !index.isValid()) {
       return QVariant();
    }

    if (CQImageProvider::s_imageProvider) {
        if (role == Qt::DecorationRole) {
            const QString id = kw_document->caption() + "textData" + QString::number(index.row());
            if (!CQImageProvider::s_imageProvider->containsId(id)) {
                KWPage pagePreview = kw_document->pageManager()->page(index.row()+1);
                QImage image = pagePreview.thumbnail(m_thumbnailSize, kw_shapemanager);
                if (image.isNull() == true) {
                    return QVariant();
                }
                CQImageProvider::s_imageProvider->addImage(id, image);
            }
            return QString("image://%1/%2").arg(CQImageProvider::identificationString).arg(id);
        }
    }
    return QVariant();
}

QSize CQTextDocumentModel::thumbnailSize() const
{
    return m_thumbnailSize;
}

void CQTextDocumentModel::setThumbnailSize(const QSize& newSize)
{
    m_thumbnailSize = newSize;
    if (newSize.height() == 0) {
        m_thumbnailSize = QSize(512, 512);
    }
    if (CQImageProvider::s_imageProvider) {
        CQImageProvider::s_imageProvider->clearCache();
        dataChanged(index(0), index(kw_document->pageCount() - 1));
    }
    emit thumbnailSizeChanged();
}
