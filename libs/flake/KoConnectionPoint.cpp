/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoConnectionPoint.h"

KoConnectionPoint::KoConnectionPoint()
    : position(0, 0)
    , escapeDirection(AllDirections)
    , alignment(AlignNone)
{
}

KoConnectionPoint::KoConnectionPoint(const QPointF &pos)
    : position(pos)
    , escapeDirection(AllDirections)
    , alignment(AlignNone)
{
}

KoConnectionPoint::KoConnectionPoint(const QPointF &pos, EscapeDirection direction)
    : position(pos)
    , escapeDirection(direction)
    , alignment(AlignNone)
{
}

KoConnectionPoint::KoConnectionPoint(const QPointF &pos, EscapeDirection direction, Alignment alignment)
    : position(pos)
    , escapeDirection(direction)
    , alignment(alignment)
{
}

KoConnectionPoint KoConnectionPoint::defaultConnectionPoint(PointId connectionPointId)
{
    switch (connectionPointId) {
    case TopConnectionPoint:
        return KoConnectionPoint(QPointF(0.5, 0.0));
    case RightConnectionPoint:
        return KoConnectionPoint(QPointF(1.0, 0.5));
    case BottomConnectionPoint:
        return KoConnectionPoint(QPointF(0.5, 1.0));
    case LeftConnectionPoint:
        return KoConnectionPoint(QPointF(0.0, 0.5));
    default:
        return KoConnectionPoint();
    }
}
