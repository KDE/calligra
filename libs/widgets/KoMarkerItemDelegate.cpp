/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoMarkerItemDelegate.h"

#include <KoMarker.h>
#include <KoPathShape.h>

#include <QPainter>
#include <QPainterPath>
#include <QPen>

KoMarkerItemDelegate::KoMarkerItemDelegate(KoMarkerData::MarkerPosition position, QObject *parent)
    : QAbstractItemDelegate(parent)
    , m_position(position)
{
}

void KoMarkerItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    bool antialiasing = painter->testRenderHint(QPainter::Antialiasing);
    if (!antialiasing) {
        painter->setRenderHint(QPainter::Antialiasing, true);
    }

    KoPathShape pathShape;
    pathShape.moveTo(QPointF(option.rect.left(), option.rect.center().y()));
    pathShape.lineTo(QPointF(option.rect.right(), option.rect.center().y()));
    KoMarker *marker = index.data(Qt::DecorationRole).value<KoMarker *>();
    if (marker != nullptr) {
        pathShape.setMarker(marker, m_position);
    }

    // paint marker
    QPen pen(option.palette.text(), 2);
    QPainterPath path = pathShape.pathStroke(pen);
    painter->fillPath(path, pen.brush());

    if (!antialiasing) {
        painter->setRenderHint(QPainter::Antialiasing, false);
    }

    painter->restore();
}

QSize KoMarkerItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(80, 30);
}
