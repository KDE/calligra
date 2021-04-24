/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQTHUMBNAILITEM_H
#define CQTHUMBNAILITEM_H

#include <QDeclarativeItem>

class CQThumbnailItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QPixmap content READ content WRITE setContent NOTIFY contentChanged)

public:
    explicit CQThumbnailItem(QDeclarativeItem* parent = 0);
    ~CQThumbnailItem();

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* );

    QPixmap content() const;

public Q_SLOTS:
    void setContent(const QPixmap& content);

Q_SIGNALS:
    void contentChanged();

private:
    class Private;
    Private * const d;
};

#endif // CQTHUMBNAILITEM_H
