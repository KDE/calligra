/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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
    // Note: copying changes the iterator for both objects!
    GGroup(const GGroup &rhs);
    GGroup(const QDomElement &element);

    virtual ~GGroup();

    virtual const bool isOk() const;
    virtual void setOk(const bool &ok=true);

    virtual GObject *clone() const;
    virtual GObject *instantiate(const QDomElement &element) const;

    virtual const bool plugChild(GObject *child, const Position &pos=Current);
    virtual const bool unplugChild(GObject *child);

    virtual const GObject *firstChild() const;
    virtual const GObject *nextChild() const;
    virtual const GObject *lastChild() const;
    virtual const GObject *prevChild() const;
    virtual const GObject *current() const;

    virtual QDomElement save(QDomDocument &doc) const;

    virtual void draw(QPainter &p, QRegion &reg, const bool toPrinter=false);
    virtual void drawHandles(QPainter &p, QList<QRect> *handles=0L);

    virtual void setZoom(const short &zoom=100);

    virtual const GObject *hit(const QPoint &p) const;
    virtual const bool intersects(const QRect &r) const;
    virtual const QRect &boundingRect() const;

    virtual GObjectM9r *createM9r(GraphitePart *part, const GObjectM9r::Mode &mode=GObjectM9r::Manipulate);

    virtual const QPoint origin() const;
    virtual void setOrigin(const QPoint &origin);
    virtual void moveX(const int &dx);
    virtual void moveY(const int &dy);
    virtual void move(const int &dx, const int &dy);

    virtual void rotate(const QPoint &center, const double &angle);

    virtual void scale(const QPoint &origin, const double &xfactor, const double &yfactor);
    virtual void resize(const QRect &boundingRect);

    virtual void setState(const State state);
    virtual void setFillStyle(const FillStyle &fillStyle);
    virtual void setBrush(const QBrush &brush);
    virtual void setGradient(const Gradient &gradient);
    virtual void setPen(const QPen &pen);

protected:
    QList<GObject> m_members;
    mutable QListIterator<GObject> *m_iterator;

private:
    GGroup &operator=(const GGroup &rhs);
};
#endif // ggroup_h
