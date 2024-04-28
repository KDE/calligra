/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOLINESTYLEITEMDELEGATE_H
#define KOLINESTYLEITEMDELEGATE_H

#include <QAbstractItemDelegate>

/// The line style item delegate for rendering the styles
class KoLineStyleItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit KoLineStyleItemDelegate(QObject *parent = nullptr);
    ~KoLineStyleItemDelegate() override = default;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif
