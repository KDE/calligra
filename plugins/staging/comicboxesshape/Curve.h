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

#ifndef _CURVE_H_
#define _CURVE_H_

#include <QPointF>

class Curve
{
public:
    Curve() {}
    Curve(qreal x1, qreal y1, qreal x2, qreal y2);
    Curve(const QPointF& p1, const QPointF& cp, const QPointF& p2);
    QPointF pointAt(qreal r);
    QPointF quadControlPoint();
    QPointF p1() const { return m_p1; }
    QPointF cp() const { return m_cp; }
    QPointF p2() const { return m_p2; }
private:
    QPointF m_p1, m_cp, m_p2;
};

#endif
