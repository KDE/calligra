/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRSVGPATHPARSER_H
#define KPRSVGPATHPARSER_H

#include <svgpathparser.h>
#include "global.h"
#include "KoPointArray.h"

class KPrSVGPathParser : public SVGPathParser
{
public:
    /**
     * @brief Destructor
     */
    virtual ~KPrSVGPathParser() {};

    /**
     * @brief Get the type of object which can handle the path.
     * 
     * @param d the svg:d attribute of the draw:path object
     *
     * @return The object type. This can be:
     *             OT_FREEHAND
     *             OT_CLOSED_LINE
     *             OT_QUADRICBEZIERCURVE
     *             OT_CUBICBEZIERCURVE
     *             or
     *             OT_UNDEFINED if path is not supported
     */
    ObjType getType( QString &d );

    /**
     * @brief Get the points of the path
     *
     * @param d the svg:d attribute of the draw:path object
     * @param convert2lines true if curves should be replaced by lines.
     *
     * @return points of the path
     */
    KoPointArray getPoints( QString &d, bool convert2lines = false );
    
protected:
    virtual void svgMoveTo( double x1, double y1, bool abs = true );
    virtual void svgLineTo( double x1, double y1, bool abs = true );
    virtual void svgCurveToCubic(  double x1, double y1, double x2, double y2, double x, double y, bool abs = true );
    virtual void svgClosePath();

private:
    /// if the type or the points should be analysed
    bool m_type;
    enum PathType
    {
        SEEN_MOVE = 1,
        SEEN_LINE = 2,
        SEEN_CUBIC = 4,
        SEEN_QUADRIC = 8,
        SEEN_CLOSE = 16,
        NO_BEZIER = 32,
        UNSUPPORTED = 64
    };
    /// the flags of the path
    int m_pathType;

    /// the points of the path
    KoPointArray m_points;
    /// the current point in the path
    KoPoint m_curPoint;
    /// the index of the points in m_points
    int m_pointIdx;
    /// if true curves are converted to lines
    bool m_convert2lines;
};

#endif /* KPRSVGPATHPARSER_H */
