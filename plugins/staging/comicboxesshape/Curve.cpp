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

#include "Curve.h"
#include <qmath.h>

Curve::Curve(qreal x1, qreal y1, qreal x2, qreal y2) : m_p1(x1, y1), m_p2(x2, y2)
{
    m_cp = 0.5 * (m_p1 + m_p2);
    m_t = 0.5;
}

Curve::Curve(const QPointF& p1, const QPointF& cp, qreal t, const QPointF& p2) : m_p1(p1), m_cp(cp), m_p2(p2), m_t(t)
{
}

QPointF Curve::quadControlPoint()
{
    if(m_t == 0.0) return m_p1;
    if(m_t == 1.0) return m_p2;
    return (m_cp - (1 - m_t) * (1 - m_t) * m_p1 - m_t * m_t * m_p2 ) / ( 2 * (1 - m_t) * m_t ) ;
}

QPointF Curve::pointAt(qreal t)
{
// //     return (1 - t) * m_p1 + t * m_p2;
    // http://en.wikipedia.org/wiki/Bézier_curve#Quadratic_B.C3.A9zier_curves
    QPointF cpq = quadControlPoint();
    return (1 - t) * (1-t) * m_p1 + 2 * (1-t)*t * cpq + t * t * m_p2;
}
