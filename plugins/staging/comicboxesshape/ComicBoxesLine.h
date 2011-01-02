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

#ifndef _COMIC_BOXES_LINE_H_
#define _COMIC_BOXES_LINE_H_

#include "Curve.h"

class QTransform;

/**
 * Describe a line, it can either be absolute representing the extremeties of the page,
 * or relative to other lines.
 */
class ComicBoxesLine
{
public:
    /**
     * This constructor define an absolute ComicBoxesLine, only used
     * to construct the border of the page.
     */
    ComicBoxesLine(const Curve& _line );
    ComicBoxesLine(ComicBoxesLine* _line1, qreal _c1, ComicBoxesLine* _line2, qreal _c2);
    /**
     * @return true if the line can be edited
     */
    bool isEditable() const;
    /**
     * @return true if m_line1 == 0, and m_line is defined
     */
    bool isAbsoluteLine() const;
    /**
     * This return the line in the box coordinate, or m_line if it is an absolute line
     */
    Curve curve() const;
    qreal c1() const;
    void setC1(qreal _c1);
    qreal c2() const;
    void setC2(qreal _c2);
    void setCP(const QPointF& );
    QPointF cp() const;
    QTransform lineCoordinateToShapeCoordinate() const;
    ComicBoxesLine* line1();
    ComicBoxesLine* line2();
private:
    Curve m_curve;
    ComicBoxesLine* m_line1;
    ComicBoxesLine* m_line2;
    qreal m_c1, m_c2;
    QPointF m_cp;
    
};

#endif
