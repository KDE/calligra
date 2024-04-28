/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOMARKERMODEL_H
#define KOMARKERMODEL_H

#include <KoMarkerData.h>
#include <QAbstractListModel>

class KoMarker;

class KoMarkerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    KoMarkerModel(const QList<KoMarker *> markers, KoMarkerData::MarkerPosition position, QObject *parent = nullptr);
    ~KoMarkerModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int markerIndex(KoMarker *marker) const;
    QVariant marker(int index, int role = Qt::UserRole) const;
    KoMarkerData::MarkerPosition position() const;

private:
    QList<KoMarker *> m_markers;
    KoMarkerData::MarkerPosition m_markerPosition;
};

#endif /* KOMARKERMODEL_H */
