/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

#ifndef gline_h
#define gline_h

#include <gobject.h>


class GLine : public GObject {

public:
    GLine(const QPoint &a, const QPoint &b, const QString &name=QString::null);
    GLine(const QString &name=QString::null);
    GLine(const GLine &rhs);
    GLine(const QDomElement &element);
    virtual ~GLine() {}

    virtual GLine *clone() const;           // exact copy of "this" (calls the Copy-CTOR)
    // create a line and initialize it with the given XML (calls the XML-CTOR)
    virtual GLine *instantiate(const QDomElement &element) const;

    virtual QDomElement save(QDomDocument &doc) const; // save the line to xml

    virtual void draw(QPainter &p, const QRect &rect, bool toPrinter=false);
    // Do we need this? Maybe even lines should have rectangular handles...
    virtual void drawHandles(QPainter &p, QList<QRect> *handles=0L);

    virtual void recalculate();

    virtual const GLine *hit(const QPoint &p) const;
    virtual bool intersects(const QRect &r) const;
    virtual const QRect &boundingRect() const;

    virtual GObjectM9r *createM9r(GraphitePart *part, GraphiteView *view,
                                  const GObjectM9r::Mode &mode=GObjectM9r::Manipulate);

    virtual const QPoint origin() const { return m_a; }
    virtual void setOrigin(const QPoint &origin);
    virtual void moveX(const int &dx);
    virtual void moveY(const int &dy);
    virtual void move(const int &dx, const int &dy);

    virtual void rotate(const QPoint &center, const double &angle);
    virtual void scale(const QPoint &origin, const double &xfactor, const double &yfactor);
    virtual void resize(const QRect &boundingRect);

    const QPoint &a() const { return m_a; }
    void setA(const QPoint &a) { m_a=a; }
    const QPoint &b() const { return m_b; }
    void setB(const QPoint &b) { m_b=b; }

private:
    GLine &operator=(const GLine &rhs);    // don't assign the objects, clone them
    QPoint m_a, m_b;
};


class GLineM9r : public G1DObjectM9r {

    Q_OBJECT
public:
    GLineM9r(GLine *line, const Mode &mode, GraphitePart *part,
             GraphiteView *view, const QString &type);
    virtual ~GLineM9r();

    virtual void draw(QPainter &p);

    virtual bool mouseMoveEvent(QMouseEvent *e, QRect &dirty);
    virtual bool mousePressEvent(QMouseEvent *e, QRect &dirty);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QRect &dirty);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QRect &dirty);

    virtual bool keyPressEvent(QKeyEvent *e, QRect &dirty);
    virtual bool keyReleaseEvent(QKeyEvent *e, QRect &dirty);

    virtual GObject *gobject() { return m_line; }

private:
    GLineM9r(const GLineM9r &rhs);
    GLineM9r &operator=(GLineM9r &rhs);

    GLine *m_line;
};
#endif
