/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSNAPDATA_H
#define KOSNAPDATA_H

#include <KoPathSegment.h>
#include <QVector>
#include <flake_export.h>

/**
 * This class is used to provide additional data to the snap guide.
 *
 * Some shapes might have internal data it wants snapping support for,
 * i.e. the axis of a chart shape, the row of a table shape, etc.
 * As the data is internal and special to that shape, the snap guide
 * does not know about it and can therefore not provide any snapping
 * to it.
 * So the shape can put that data in form of points or segments into
 * that class which the snap guide can retrieve and use accordingly.
 */
class FLAKE_EXPORT KoSnapData
{
public:
    KoSnapData();
    ~KoSnapData();

    /// Returns list of points to snap to
    QVector<QPointF> snapPoints() const;

    /// Sets list of points to snap to
    void setSnapPoints(const QVector<QPointF> &snapPoints);

    /// Returns list of segments to snap to
    QList<KoPathSegment> snapSegments() const;

    /// Sets list of segments to snap to
    void setSnapSegments(const QList<KoPathSegment> &snapSegments);

private:
    QVector<QPointF> m_points;
    QList<KoPathSegment> m_segments;
};

#endif // KOSNAPDATA_H
