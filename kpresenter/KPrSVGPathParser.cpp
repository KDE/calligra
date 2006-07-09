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

#include "KPrSVGPathParser.h"


ObjType KPrSVGPathParser::getType( QString &d )
{
    m_type = true;
    m_pathType = 0;
    ObjType objType = OT_UNDEFINED;
    parseSVG( d, true );
    if ( m_pathType & UNSUPPORTED )
    {
        objType = OT_UNDEFINED;
    }
    else if ( m_pathType & SEEN_CLOSE )
    {
        objType = OT_CLOSED_LINE;
    }
    else if ( m_pathType & NO_BEZIER )
    {
        objType = OT_FREEHAND;
    }
    else if ( ( m_pathType & SEEN_QUADRIC ) && ! ( m_pathType & SEEN_CUBIC ) )
    {
        objType = OT_QUADRICBEZIERCURVE;
    }
    else if ( m_pathType & SEEN_CUBIC )
    {
        objType = OT_CUBICBEZIERCURVE;
    }
    return objType;
}


KoPointArray KPrSVGPathParser::getPoints( QString &d, bool convert2lines )
{
    m_type = false;
    m_pointIdx = 0;
    m_convert2lines = convert2lines;
    parseSVG( d, true );
    return m_points;
}


void KPrSVGPathParser::svgMoveTo( double x1, double y1, bool /*abs*/ )
{
    if ( m_type )
    {
        if ( m_pathType != 0 )
        {
            m_pathType |= UNSUPPORTED;
        }
        else
        {
            m_pathType |= SEEN_MOVE;
        }
    }
    else
    {
        m_curPoint = KoPoint( x1, y1 );
    }
}


void KPrSVGPathParser::svgLineTo( double x1, double y1, bool /*abs*/ )
{
    if ( m_type )
    {
        if ( m_pathType & SEEN_MOVE )
        {
            if ( m_pathType & SEEN_LINE )
            {
                m_pathType |= NO_BEZIER;
            }
            m_pathType |= SEEN_LINE;
        }
        else
        {
            m_pathType |= UNSUPPORTED;
        }
    }
    else
    {
        m_points.putPoints( m_pointIdx, 2, m_curPoint.x(), m_curPoint.y(), x1, y1 );
        m_curPoint = KoPoint( x1, y1 );
        m_pointIdx += 2; 
    }
}


void KPrSVGPathParser::svgCurveToCubic(  double x1, double y1, double x2, double y2, double x, double y, bool /*abs*/ )
{
    if ( m_type )
    {
        if ( m_pathType & SEEN_MOVE )
        {
            if ( m_pathType & SEEN_LINE )
            {
                m_pathType |= NO_BEZIER;
            }

            if ( x1 == x2 && y1 == y2 )
            {
                m_pathType |= SEEN_QUADRIC;
            }
            else
            {
                m_pathType |= SEEN_CUBIC;
            }
        }
        else
        {
            m_pathType |= UNSUPPORTED;
        }
    }
    else
    {
        if ( m_convert2lines )
        {
            KoPointArray bezierPoints;
            bezierPoints.putPoints( 0, 4, m_curPoint.x(), m_curPoint.y(), x1, y1, x2, y2, x, y );

            bezierPoints = bezierPoints.cubicBezier();
            KoPointArray::ConstIterator it;
            for ( it = bezierPoints.begin(); it != bezierPoints.end(); ++it ) 
            {
                KoPoint point = (*it);
                m_points.putPoints( m_pointIdx, 1, point.x(), point.y() );
                ++m_pointIdx;
            }
        }
        else
        {
            m_points.putPoints( m_pointIdx, 4, m_curPoint.x(), m_curPoint.y(), x, y, x1, y1, x2, y2 );
            m_pointIdx += 4; 
        }
        m_curPoint = KoPoint( x, y );
    }
}


// TODO
void KPrSVGPathParser::svgClosePath()
{
    if ( m_type )
    {
        if ( m_pathType & SEEN_CLOSE )
        {
            m_pathType |= UNSUPPORTED;
        }
        else
        {
            m_pathType |= SEEN_CLOSE;
        }
    }
    else
    {
    }
}


