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

#ifndef ggroup_h
#define ggroup_h

#include <qlist.h>
#include <gobject.h>


class GGroup : public GObject {

public:
    GGroup(const QString &name=QString::null);
    // Note: copying changes the iterator of both objects!
    GGroup(const GGroup &rhs);
    GGroup(const QDomElement &element);

    virtual ~GGroup();

    virtual bool isOk() const;
    virtual void setOk(bool ok=true);

    virtual GObject *clone() const;
    virtual GObject *instantiate(const QDomElement &element) const;

    virtual bool plugChild(GObject *child, const Position &pos=Current);
    virtual bool unplugChild(GObject *child);

    virtual const GObject *firstChild() const;
    virtual const GObject *nextChild() const;
    virtual const GObject *lastChild() const;
    virtual const GObject *prevChild() const;
    virtual const GObject *current() const;

    virtual QDomElement save(QDomDocument &doc) const;

    virtual void draw(QPainter &p, const QRect &rect, bool toPrinter=false);

    virtual void recalculate();

    virtual const GObject *hit(const QPoint &p) const;
    virtual bool intersects(const QRect &r) const;
    virtual const QRect &boundingRect() const;

    virtual GObjectM9r *createM9r(GraphitePart *part, GraphiteView *view,
                                  const GObjectM9r::Mode &mode=GObjectM9r::Manipulate);

    virtual const FxPoint origin() const;
    virtual void setOrigin(const FxPoint &origin);
    virtual void moveX(const double &dx);
    virtual void moveY(const double &dy);
    virtual void move(const double &dx, const double &dy);

    virtual void rotate(const FxPoint &center, const double &angle);

    virtual void scale(const FxPoint &origin, const double &xfactor, const double &yfactor);
    virtual void resize(const FxRect &boundingRect);

    virtual void setState(const State state);
    virtual void setFillStyle(const FillStyle &fillStyle);
    virtual void setBrush(const QBrush &brush);
    virtual void setGradient(const Gradient &gradient);
    virtual void setPen(const QPen &pen);

private:
    GGroup &operator=(const GGroup &rhs);

    QList<GObject> m_members;
    mutable QListIterator<GObject> *m_iterator;
};


class GGroupM9r : public G2DObjectM9r {

    Q_OBJECT
public:
    GGroupM9r(GGroup *group, const Mode &mode, GraphitePart *part,
              GraphiteView *view, const QString &type);
    virtual ~GGroupM9r();

    virtual void draw(QPainter &p);

    virtual bool mouseMoveEvent(QMouseEvent *e, QRect &dirty);
    virtual bool mousePressEvent(QMouseEvent *e, QRect &dirty);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QRect &dirty);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QRect &dirty);

    virtual bool keyPressEvent(QKeyEvent *e, QRect &dirty);
    virtual bool keyReleaseEvent(QKeyEvent *e, QRect &dirty);

    virtual GObject *gobject() { return m_group; }

private:
    GGroupM9r(const GGroupM9r &rhs);
    GGroupM9r &operator=(GGroupM9r &rhs);

    GGroup *m_group;
};

#endif // ggroup_h
