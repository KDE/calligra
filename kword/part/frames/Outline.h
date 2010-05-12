/* This file is part of the KDE project
 * Copyright (C) 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Ko Gmbh <casper.boemann@kogmbh.com>
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

#ifndef OUTLINE_H
#define OUTLINE_H

#include <QMatrix>
#include <QMultiMap>
#include <QPainterPath>

class KoShape;
class KWFrame;

///  Class that allows us with the runaround of QPainterPaths
class Outline
{
public:
    Outline(KWFrame *frame, const QMatrix &matrix);
    Outline(KoShape *shape, const QMatrix &matrix);

    void init(const QMatrix &matrix, const QPainterPath &outline, qreal distance);

    QRectF limit(const QRectF &content);

    KoShape *shape() const { return m_shape; }

    static qreal xAtY(const QLineF &line, qreal y);

    void changeMatrix(const QMatrix &matrix);

private:
    enum Side { None, Left, Right, Empty }; // TODO support Auto and Both
    Side m_side;
    QMultiMap<qreal, QLineF> m_edges; //sorted with y-coord
    QRectF m_bounds;
    KoShape *m_shape;
    qreal m_distance;
};

#endif
