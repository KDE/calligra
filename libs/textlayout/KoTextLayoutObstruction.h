/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Ko Gmbh <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTLAYOUTOBSTRUCTION_H
#define KOTEXTLAYOUTOBSTRUCTION_H

#include "kotextlayout_export.h"

#include <QLineF>
#include <QMultiMap>
#include <QPolygonF>
#include <QRectF>

class KoShape;
class QTransform;
class QPainterPath;

///  Class that allows us with the runaround of QPainterPaths
class KOTEXTLAYOUT_EXPORT KoTextLayoutObstruction
{
public:
    KoTextLayoutObstruction(KoShape *shape, const QTransform &matrix);

    KoTextLayoutObstruction(const QRectF &rect, bool rtl);

    void init(const QTransform &matrix,
              const QPainterPath &obstruction,
              qreal distanceLeft,
              qreal distanceTop,
              qreal distanceRight,
              qreal distanceBottom,
              qreal borderHalfWidth);

    QRectF limit(const QRectF &content);

    KoShape *shape() const
    {
        return m_shape;
    }

    static qreal xAtY(const QLineF &line, qreal y);

    void changeMatrix(const QTransform &matrix);

    //-------------------------------------------------------------------------------

    QRectF cropToLine(const QRectF &lineRect);

    QRectF getLeftLinePart(const QRectF &lineRect) const;

    QRectF getRightLinePart(const QRectF &lineRect) const;

    bool textOnLeft() const;

    bool textOnRight() const;

    bool textOnBiggerSide() const;

    bool textOnEnoughSides() const;

    bool noTextAround() const;

    // Don't run around unless available space is > than this when m_side == Enough.
    qreal runAroundThreshold() const;

    static bool compareRectLeft(KoTextLayoutObstruction *o1, KoTextLayoutObstruction *o2);

private:
    QPainterPath decoratedOutline(const KoShape *shape, qreal &borderHalfWidth) const;

    enum Side { None, Left, Right, Empty, Both, Bigger, Enough };
    Side m_side;
    QRectF m_bounds;
    QPolygonF m_polygon;

public:
    QRectF m_line;
    QMultiMap<qreal, QLineF> m_edges; // sorted with y-coord
    KoShape *m_shape;
    QRectF m_rect;
    qreal m_distanceLeft;
    qreal m_distanceTop;
    qreal m_distanceRight;
    qreal m_distanceBottom;
    qreal m_borderHalfWidth;
    qreal m_runAroundThreshold;
};

#endif
