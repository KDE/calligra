/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOMARKERITEMDELEGATE_H
#define KOMARKERITEMDELEGATE_H

// Calligra
#include <KoMarkerData.h>
// Qt
#include <QAbstractItemDelegate>

class KoMarkerItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit KoMarkerItemDelegate(KoMarkerData::MarkerPosition position, QObject *parent = nullptr);
    ~KoMarkerItemDelegate() override = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    KoMarkerData::MarkerPosition m_position;
};

#endif /* KOMARKERITEMDELEGATE_H */
