/* This file is part of the KDE project
 * Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoMarkerItemDelegate.h"

#include <KoMarker.h>

#include <QPainter>
#include <QPen>

KoMarkerItemDelegate::KoMarkerItemDelegate(QObject *parent)
: QAbstractItemDelegate(parent)
{
}

KoMarkerItemDelegate::~KoMarkerItemDelegate()
{
}

void KoMarkerItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    KoMarker *marker = index.data(Qt::DecorationRole).value<KoMarker*>();
    if (marker) {
        painter->save();
        bool anitaliasing = painter->testRenderHint(QPainter::Antialiasing);
        if (!anitaliasing) {
            painter->setRenderHint(QPainter::Antialiasing, true);
        }
        // paint marker
        QPen pen(option.palette.text(), 1);
        painter->setPen(pen);
        QPainterPath path = marker->path();
        painter->drawPath(path);
        if (!anitaliasing) {
            painter->setRenderHint(QPainter::Antialiasing, false);
        }

        painter->restore();
    }
}

QSize KoMarkerItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    return QSize(100, 15);
}
