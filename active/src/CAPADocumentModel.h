/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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

#ifndef CAPADOCUMENTMODEL_H
#define CAPADOCUMENTMODEL_H

#include <KoPADocumentModel.h>

class KoPADocument;
class CADocumentController;

class CAPADocumentModel : public KoPADocumentModel
{
    Q_OBJECT
public:
    enum DataRoles {
        SlideImageRole = BeginThumbnailRole + 1
    };
    explicit CAPADocumentModel(QObject* parent = 0, KoPADocument* document = 0);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

signals:
    void documentControllerChanged();

private:
    KoPADocument *m_document;
};

#endif // CAPADOCUMENTMODEL_H
