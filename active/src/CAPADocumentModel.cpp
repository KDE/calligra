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

#include <KDebug>

CAPADocumentModel::CAPADocumentModel(QObject* parent, KoPADocument* document)
    : KoPADocumentModel(parent, document)
    , m_documentController(0)
    , m_document(document)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[BeginThumbnailRole] = "thumbnail";
    setRoleNames(roles);
}

CADocumentController* CAPADocumentModel::documentController() const
{
    return m_documentController;
}

void CAPADocumentModel::setDocumentController(CADocumentController* controller)
{
    if (m_documentController == controller) {
        return;
    }
    m_documentController = controller;
    if (controller->documentHandler()) {
        KoDocument *doc = qobject_cast<CAAbstractDocumentHandler*>(controller->documentHandler())->document();
        if (doc) {
            m_document = qobject_cast<KoPADocument*>(doc);
            setDocument(m_document);
        }
    }
    emit documentControllerChanged();
}

QVariant CAPADocumentModel::data(const QModelIndex& index, int role) const
{
    if (role == BeginThumbnailRole && m_document) {
        const QString id = m_document->caption() + "slide" + QString::number(index.row());
        QImage image = KoPADocumentModel::data(index, BeginThumbnailRole + 256).value<QImage>();
        CAImageProvider::instance()->addImage(id, image);
        return QString("image://") + QString(CAImageProvider::identificationString) + "/" + id;
    }
    return KoPADocumentModel::data(index, role);
}

#include "CAPADocumentModel.moc"
