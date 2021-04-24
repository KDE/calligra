/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOCONNECTIONSHAPEPRIVATE_P
#define KOCONNECTIONSHAPEPRIVATE_P

#include "KoParameterShape_p.h"

class KoConnectionShapePrivate : public KoParameterShapePrivate
{
public:
    explicit KoConnectionShapePrivate(KoConnectionShape *q);

    /// Returns escape direction of given handle
    QPointF escapeDirection(int handleId) const;

    /// Checks if rays from given points into given directions intersect
    bool intersects(const QPointF &p1, const QPointF &d1, const QPointF &p2, const QPointF &d2, QPointF &isect);

    /// Returns perpendicular direction from given point p1 and direction d1 toward point p2
    QPointF perpendicularDirection(const QPointF &p1, const QPointF &d1, const QPointF &p2);

    /// Populate the path list by a normal way
    void normalPath(const qreal MinimumEscapeLength);

    qreal scalarProd(const QPointF &v1, const QPointF &v2) const;
    qreal crossProd(const QPointF &v1, const QPointF &v2) const;

    /// Returns if given handle is connected to a shape
    bool handleConnected(int handleId) const;

    QVector<QPointF> path;

    KoShape *shape1;
    KoShape *shape2;
    int connectionPointId1;
    int connectionPointId2;
    KoConnectionShape::Type connectionType;
    bool forceUpdate;
    bool hasCustomPath;
    Q_DECLARE_PUBLIC(KoConnectionShape)
};

#endif
