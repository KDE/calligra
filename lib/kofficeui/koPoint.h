/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef koPoint_h
#define koPoint_h

/**
 * A point whose coordinates are floating-point values ( "double"s ).
 * The API isn't documented, it's a perfect mirror of QPoint.
 */
class KoPoint {

public:
    KoPoint() { m_x = 0; m_y = 0; }
    KoPoint(const double &x, const double &y) : m_x(x), m_y(y) {}
    ~KoPoint() {}

    KoPoint &operator=(const KoPoint &rhs) { m_x=rhs.x(); m_y=rhs.y(); return *this; }
    bool operator==(const KoPoint &rhs) const { return m_x==rhs.x() && m_y==rhs.y(); }

    double x() const { return m_x; }
    double y() const { return m_y; }
    void setX(const double &x) { m_x = x; }
    void setY(const double &y) { m_y = y; }

    double &rx() { return m_x; }
    double &ry() { return m_y; }

    // many missing operators

    // Not in QPoint:
    void setCoords(const double &x, const double &y) { m_x = x; m_y = y; }

private:
    double m_x, m_y;
};

#endif
