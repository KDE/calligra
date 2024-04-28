/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQPRESENTATIONMODEL_H
#define CQPRESENTATIONMODEL_H

#include <QAbstractListModel>
#include <QPixmap>

class QDeclarativeItem;
class CQPresentationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeItem *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QSizeF thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)

public:
    enum Roles {
        ThumbnailRole = Qt::UserRole,
    };

    explicit CQPresentationModel(QObject *parent = 0);
    ~CQPresentationModel();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual int rowCount(const QModelIndex &parent) const;

    Q_INVOKABLE QPixmap thumbnail(int index) const;

    QDeclarativeItem *canvas() const;

    QSizeF thumbnailSize() const;

public Q_SLOTS:
    void setCanvas(QDeclarativeItem *canvas);
    void setThumbnailSize(const QSizeF &size);
    void canvasSourceChanged();

Q_SIGNALS:
    void canvasChanged();
    void thumbnailSizeChanged();

private:
    class Private;
    Private *const d;
};

#endif // CQPRESENTATIONMODEL_H
