/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoMarkerModel.h"

// Calligra
#include <KoMarker.h>
// Qt
#include <QSize>

KoMarkerModel::KoMarkerModel(const QList<KoMarker *> markers, KoMarkerData::MarkerPosition position, QObject *parent)
    : QAbstractListModel(parent)
    , m_markers(markers)
    , m_markerPosition(position)
{
}

int KoMarkerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_markers.count();
}

QVariant KoMarkerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DecorationRole:
        if (index.row() < m_markers.size()) {
            return QVariant::fromValue<KoMarker *>(m_markers.at(index.row()));
        }
        return QVariant();
    case Qt::SizeHintRole:
        return QSize(80, 30);
    default:
        return QVariant();
    }
}

int KoMarkerModel::markerIndex(KoMarker *marker) const
{
    return m_markers.indexOf(marker);
}

QVariant KoMarkerModel::marker(int index, int role) const
{
    if (index < 0) {
        return QVariant();
    }

    switch (role) {
    case Qt::DecorationRole:
        if (index < m_markers.size()) {
            return QVariant::fromValue<KoMarker *>(m_markers.at(index));
        }
        return QVariant();
    case Qt::SizeHintRole:
        return QSize(80, 30);
    default:
        return QVariant();
    }
}

KoMarkerData::MarkerPosition KoMarkerModel::position() const
{
    return m_markerPosition;
}
