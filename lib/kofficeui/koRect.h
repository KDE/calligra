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

#ifndef koRect_h
#define koRect_h

#include "koPoint.h"
/**
 * A rect whose coordinates are floating-point values ( "double"s ).
 * The API isn't documented, it's a perfect mirror of QRect.
 */
class KoRect {

public:
    KoRect()
        : m_tl(), m_br() {}
    KoRect(const KoPoint &topleft, const KoPoint &bottomright)
        : m_tl(topleft), m_br(bottomright) {}
    KoRect(const double &left, const double &top, const double &width, const double &height)
        : m_tl(left,top), m_br(left+width,top+height) {}
    ~KoRect() {}

    bool isNull() const { return m_tl == m_br; }
    bool isEmpty() const { return m_tl.x() > m_br.x() || m_tl.y() > m_br.y(); }
    bool isValid() const { return m_tl.x() <= m_br.x() && m_tl.y() <= m_br.y(); }
    KoRect normalize() const;

    double left() const { return m_tl.x(); }
    double top() const { return m_tl.y(); }
    double right() const { return m_br.x(); }
    double bottom() const { return m_br.y(); }

    double& rLeft() { return m_tl.rx(); }
    double& rTop() { return m_tl.ry(); }
    double& rRight() { return m_br.rx(); }
    double& rBottom() { return m_br.ry(); }

    double x() const { return left(); }
    double y() const { return top(); }

    void setLeft(const double &left) { m_tl.setX(left); }
    void setTop(const double &top) { m_tl.setY(top); }
    void setRight(const double &right) { m_br.setX(right); }
    void setBottom(const double &bottom) { m_br.setY(bottom); }

    KoPoint topLeft() const { return m_tl; }
    KoPoint bottomRight() const { return m_br; }
    KoPoint topRight() const { return KoPoint(m_br.x(), m_tl.y()); }
    KoPoint bottomLeft() const { return KoPoint(m_tl.x(), m_br.y()); }
    //KoPoint center() const;

    void moveTopLeft(const KoPoint &topleft);
    void moveBottomRight(const KoPoint &bottomright);
    void moveTopRight(const KoPoint &topright);
    void moveBottomLeft(const KoPoint &bottomleft);
    //void moveCenter(const KoPoint &center);
    void moveBy(const double &dx, const double &dy);

    void setRect(const double &x, const double &y, const double &width, const double &height);
    //void setRect(const QRect &rect);
    void setCoords(const double &x1, const double &y1, const double &x2, const double &y2);

    //KoSize size() const;
    double width() const { return m_br.x()-m_tl.x(); }
    double height() const { return m_br.y()-m_tl.y(); }
    void setWidth(const double &width) { m_br.setX(m_tl.x()+width); }
    void setHeight(const double &height) { m_br.setY(m_tl.y()+height); }
    //void setSize(const KoSize &size);

    KoRect &operator|=(const KoRect &rhs);
    KoRect &operator&=(const KoRect &rhs);
    bool contains(const KoPoint &p, bool proper=false) const;
    bool contains(const double &x, const double &y, bool proper=false) const;
    bool contains(const KoRect &r, bool proper=false) const;
    KoRect unite(const KoRect &r) const;
    KoRect intersect(const KoRect &r) const;
    bool intersects(const KoRect &r) const;

private:
    KoPoint m_tl, m_br;
};



#endif
