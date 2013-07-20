/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2012 Sujith Haridasan <sujith.h@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CATextDocumentModel.h"
#include "CADocumentController.h"
#include "CAAbstractDocumentHandler.h"
#include "CAImageProvider.h"

#include <KWPage.h>
#include <KWDocument.h>
#include <kdebug.h>

CATextDocumentModel::CATextDocumentModel(QObject* parent, KWDocument* document, KoShapeManager *shapemanager)
    : kw_document(document), kw_shapemanager(shapemanager)
{
}

int CATextDocumentModel::rowCount(const QModelIndex& parent) const
{
    return kw_document->pageManager()->pageCount();
}

QVariant CATextDocumentModel::data(const QModelIndex& index, int role) const
{

    if(!kw_document || !index.isValid()) {
       return QVariant();
    }

    if (CAImageProvider::s_imageProvider) {
        if(role == Qt::DecorationRole) {
            const QString id = kw_document->caption() + "textData" + QString::number(index.row());
            if (!CAImageProvider::s_imageProvider->containsId(id)) {
                KWPage pagePreview = kw_document->pageManager()->page(index.row()+1);
                QSize size = QSize(512,512);
                QImage image = pagePreview.thumbnail(size, kw_shapemanager);
                if(image.isNull() == true) {
                    return QVariant();
                }
                CAImageProvider::s_imageProvider->addImage(id, image);
            }
            return QString("image://" + QString(CAImageProvider::identificationString) + '/' + id);
        }
    }
    return QVariant();
}

#include "CATextDocumentModel.moc"
