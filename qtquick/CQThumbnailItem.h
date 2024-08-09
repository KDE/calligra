/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQTHUMBNAILITEM_H
#define CQTHUMBNAILITEM_H

#include <QQuickPaintedItem>

class CQThumbnailItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QPixmap content READ content WRITE setContent NOTIFY contentChanged)

public:
    explicit CQThumbnailItem(QQuickItem *parent = nullptr);
    ~CQThumbnailItem();

    void paint(QPainter *painter) override;

    QPixmap content() const;

public Q_SLOTS:
    void setContent(const QPixmap &content);

Q_SIGNALS:
    void contentChanged();

private:
    class Private;
    Private *const d;
};

#endif // CQTHUMBNAILITEM_H
