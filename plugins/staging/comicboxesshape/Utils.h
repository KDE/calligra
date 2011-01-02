/*
 *  Copyright (c) 2011 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QPoint>
#include <cmath>

inline bool near(qreal a, qreal b)
{
    return qAbs(a - b) < 1e-6;
}

inline bool near(const QPointF& a, const QPointF& b, qreal d)
{
    return (a - b).manhattanLength() < d;
}

inline qreal norm2_2(const QPointF& pt)
{
    return (pt.x() * pt.x() + pt.y() * pt.y());
}

inline qreal norm2(const QPointF& pt)
{
    return std::sqrt(norm2_2(pt));
}

// See http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
inline qreal projection(const QLineF& l, const QPointF& pt_1 )
{
    qreal top = (pt_1.x() - l.p1().x()) * (l.p2().x() - l.p1().x()) + (pt_1.y() - l.p1().y()) * (l.p2().y() - l.p1().y());
    return top / norm2_2(l.p2() - l.p1());
}
