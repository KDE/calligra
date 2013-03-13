/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Sujith Haridasan <sujith.h@gmail.com>
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

#ifndef CQTEXTDOCUMENTMODEL_H
#define CQTEXTDOCUMENTMODEL_H

#include <KWDocument.h>

class KWPageManager;
class CQDocumentController;

class CQTextDocumentModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CQTextDocumentModel(QObject* parent, KWDocument* document, KoShapeManager *shapemanager);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

signals:
    void documentControllerChanged();

private:
    KWDocument *kw_document;
    KoShapeManager *kw_shapemanager;
};

#endif // CQTEXTDOCUMENTMODEL_H
