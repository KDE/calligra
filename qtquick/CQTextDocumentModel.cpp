/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "CQTextDocumentModel.h"

#include "CQImageProvider.h"

#include <KWDocument.h>
#include <KWPage.h>

CQTextDocumentModel::CQTextDocumentModel(QObject *parent, KWDocument *document, KoShapeManager *shapemanager)
    : QAbstractListModel(parent)
    , kw_document(document)
    , kw_shapemanager(shapemanager)
    , m_thumbnailSize(QSize(512, 512))
{
}

int CQTextDocumentModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return kw_document->pageManager()->pageCount();
}

QVariant CQTextDocumentModel::data(const QModelIndex &index, int role) const
{
    if (!kw_document || !index.isValid()) {
        return QVariant();
    }

    if (CQImageProvider::s_imageProvider) {
        if (role == Qt::DecorationRole) {
            const QString id = kw_document->caption() + "textData" + QString::number(index.row());
            if (!CQImageProvider::s_imageProvider->containsId(id)) {
                KWPage pagePreview = kw_document->pageManager()->page(index.row() + 1);
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

void CQTextDocumentModel::setThumbnailSize(const QSize &newSize)
{
    m_thumbnailSize = newSize;
    if (newSize.height() == 0) {
        m_thumbnailSize = QSize(512, 512);
    }
    if (CQImageProvider::s_imageProvider) {
        CQImageProvider::s_imageProvider->clearCache();
        dataChanged(index(0), index(kw_document->pageCount() - 1));
    }
    Q_EMIT thumbnailSizeChanged();
}
