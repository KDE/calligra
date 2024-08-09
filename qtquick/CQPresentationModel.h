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

class QQuickItem;
class CQPresentationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QSizeF thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)

public:
    enum Roles {
        ThumbnailRole = Qt::UserRole,
    };

    explicit CQPresentationModel(QObject *parent = nullptr);
    ~CQPresentationModel();

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QPixmap thumbnail(int index) const;

    QQuickItem *canvas() const;

    QSizeF thumbnailSize() const;

public Q_SLOTS:
    void setCanvas(QQuickItem *canvas);
    void setThumbnailSize(const QSizeF &size);
    void canvasSourceChanged();

Q_SIGNALS:
    void canvasChanged();
    void thumbnailSizeChanged();

private:
    class Private;
    std::unique_ptr<Private> const d;
};

#endif // CQPRESENTATIONMODEL_H
