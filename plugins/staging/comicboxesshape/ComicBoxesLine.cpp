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

#include "ComicBoxesLine.h"

ComicBoxesLine::ComicBoxesLine(const QLineF& _line ) : m_line(_line), m_line1(0), m_line2(0)
{
}

ComicBoxesLine::ComicBoxesLine(ComicBoxesLine* _line1, qreal _c1, ComicBoxesLine* _line2, qreal _c2) : m_line1(_line1), m_line2(_line2)
{
    setC1(_c1);
    setC2(_c2);
}

bool ComicBoxesLine::isEditable() const
{
    return !isAbsoluteLine();
}

bool ComicBoxesLine::isAbsoluteLine() const
{
    return m_line1 == 0;
}

QLineF ComicBoxesLine::line() const
{
    if(isAbsoluteLine())
    {
        return m_line;
    } else {
        QLineF l1 = m_line1->line();
        QLineF l2 = m_line2->line();
        return QLineF(l1.pointAt(m_c1), l2.pointAt(m_c2));
    }
}

qreal ComicBoxesLine::c1() const
{
    return m_c1;
}

void ComicBoxesLine::setC1(qreal _c1)
{
    Q_ASSERT(_c1 >= 0);
    Q_ASSERT(_c1 <= 1);
    m_c1 = _c1;
}

qreal ComicBoxesLine::c2() const
{
    return m_c2;
}

void ComicBoxesLine::setC2(qreal _c2)
{
    Q_ASSERT(_c2 >= 0);
    Q_ASSERT(_c2 <= 1);
    m_c2 = _c2;
}

ComicBoxesLine* ComicBoxesLine::line1()
{
    return m_line1;
}

ComicBoxesLine* ComicBoxesLine::line2()
{
    return m_line2;
}
