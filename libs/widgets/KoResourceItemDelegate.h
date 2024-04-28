/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEITEMDELEGATE_H
#define KORESOURCEITEMDELEGATE_H

#include "KoCheckerBoardPainter.h"
#include <QAbstractItemDelegate>

/// The resource item delegate for rendering the resource preview
class KoResourceItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit KoResourceItemDelegate(QObject *parent = nullptr);
    ~KoResourceItemDelegate() override = default;
    /// reimplemented
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const override;
    /// reimplemented
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;

private:
    KoCheckerBoardPainter m_checkerPainter;
};

#endif // KORESOURCEITEMDELEGATE_H
