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
#include <KDebug>

CQTextDocumentModel::CQTextDocumentModel(QObject* parent, KWDocument* document, KoShapeManager *shapemanager)
    : kw_document(document), kw_shapemanager(shapemanager)
{
}

int CQTextDocumentModel::rowCount(const QModelIndex& parent) const
{
    return kw_document->pageManager()->pageCount();
}

QVariant CQTextDocumentModel::data(const QModelIndex& index, int role) const
{

    if(!kw_document || !index.isValid()) {
       return QVariant();
    }

    if (CQImageProvider::s_imageProvider) {
        if(role == Qt::DecorationRole) {
            const QString id = kw_document->caption() + "textData" + QString::number(index.row());
            if (!CQImageProvider::s_imageProvider->containsId(id)) {
                KWPage pagePreview = kw_document->pageManager()->page(index.row()+1);
                QSize size = QSize(512,512);
                QImage image = pagePreview.thumbnail(size, kw_shapemanager);
                if(image.isNull() == true) {
                    return QVariant();
                }
                CQImageProvider::s_imageProvider->addImage(id, image);
            }
            return QString("image://") + QString(CQImageProvider::identificationString) + "/" + id;
        }
    }
    return QVariant();
}
