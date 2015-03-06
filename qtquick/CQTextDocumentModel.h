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

#ifndef CQTEXTDOCUMENTMODEL_H
#define CQTEXTDOCUMENTMODEL_H

#include <QAbstractListModel>
#include <KWDocument.h>


class CQTextDocumentModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)
public:
    CQTextDocumentModel(QObject* parent, KWDocument* document, KoShapeManager *shapemanager);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QSize thumbnailSize() const;
    void setThumbnailSize(const QSize& newSize);
Q_SIGNALS:
    void documentControllerChanged();
    void thumbnailSizeChanged();

private:
    KWDocument *kw_document;
    KoShapeManager *kw_shapemanager;
    QSize m_thumbnailSize;
};

#endif // CQTEXTDOCUMENTMODEL_H
