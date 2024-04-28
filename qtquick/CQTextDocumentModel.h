/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CQTEXTDOCUMENTMODEL_H
#define CQTEXTDOCUMENTMODEL_H

#include <KWDocument.h>
#include <QAbstractListModel>

class CQTextDocumentModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)
public:
    CQTextDocumentModel(QObject *parent, KWDocument *document, KoShapeManager *shapemanager);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QSize thumbnailSize() const;
    void setThumbnailSize(const QSize &newSize);
Q_SIGNALS:
    void documentControllerChanged();
    void thumbnailSizeChanged();

private:
    KWDocument *kw_document;
    KoShapeManager *kw_shapemanager;
    QSize m_thumbnailSize;
};

#endif // CQTEXTDOCUMENTMODEL_H
