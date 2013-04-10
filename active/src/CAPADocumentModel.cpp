/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2012 Shantanu Tushar <shaan7in@gmail.com>
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

#include "CAPADocumentModel.h"
#include "CADocumentController.h"
#include "CAAbstractDocumentHandler.h"
#include "CAImageProvider.h"

#include <KoPADocument.h>

#include <kdebug.h>

CAPADocumentModel::CAPADocumentModel(QObject* parent, KoPADocument* document)
    : KoPADocumentModel(parent, document)
    , m_document(document)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[BeginThumbnailRole] = "thumbnail";
    roles[SlideImageRole] = "slideimage";
    setRoleNames(roles);

    if (document) {
        setDocument(document);
    }
}

QVariant CAPADocumentModel::data(const QModelIndex& index, int role) const
{
    if (!m_document)
        return QVariant();

    if (CAImageProvider::s_imageProvider) {

        if (role == BeginThumbnailRole) {
            const QString id = m_document->caption() + "slidethumb" + QString::number(index.row());
            if (!CAImageProvider::s_imageProvider->containsId(id)) {
                QImage image = KoPADocumentModel::data(index, BeginThumbnailRole + 256).value<QImage>();
                CAImageProvider::s_imageProvider->addImage(id, image);
            }
            return QString("image://") + QString(CAImageProvider::identificationString) + "/" + id;
        } else if (role == SlideImageRole) {
            const QString id = m_document->caption() + "slideimage" + QString::number(index.row());
            if (!CAImageProvider::s_imageProvider->containsId(id)) {
                QImage image = KoPADocumentModel::data(index, BeginThumbnailRole + 1024).value<QImage>();
                CAImageProvider::s_imageProvider->addImage(id, image);
            }
            return QString("image://") + QString(CAImageProvider::identificationString) + "/" + id;
        }
    }
    return KoPADocumentModel::data(index, role);
}

#include "CAPADocumentModel.moc"
