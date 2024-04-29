/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQSPREADSHEETLISTMODEL_H
#define CQSPREADSHEETLISTMODEL_H

#include <QAbstractListModel>

class CQSpreadsheetListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)

public:
    enum RoleNames { SheetNameRole = Qt::UserRole, ThumbnailRole };

    explicit CQSpreadsheetListModel(QObject *parent = nullptr);
    virtual ~CQSpreadsheetListModel();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;

    QObject *canvas() const;

    QSize thumbnailSize() const;

public Q_SLOTS:
    void setCanvas(QObject *canvas);
    void setThumbnailSize(const QSize &size);

Q_SIGNALS:
    void canvasChanged();
    void thumbnailSizeChanged();

private:
    class Private;
    Private *const d;
};

#endif // CQSPREADSHEETLISTMODEL_H
