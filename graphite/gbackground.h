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

#ifndef gbackground_h
#define gbackground_h

#include <ggroup.h>


class GBackground : public GAbstractGroup {

public:
    GBackground(const QString &name=QString::null) : GAbstractGroup(name), m_transparent(false) {}
    GBackground(const GBackground &rhs) : GAbstractGroup(rhs), m_transparent(false) {}
    GBackground(const QDomElement &element);

    virtual ~GBackground() {}

    virtual GObject *clone() const;
    virtual GObject *instantiate(const QDomElement &element) const;

    virtual QDomElement save(QDomDocument &doc) const;

    void setTransparent(bool transparent=true) { m_transparent=transparent; }
    bool transparent() const { return m_transparent; }

    virtual void draw(QPainter &p, const QRect &rect, bool toPrinter=false) const;

    virtual const QRect &boundingRect() const;

    virtual GObjectM9r *createM9r(GraphitePart *part, GraphiteView *view,
                                  const GObjectM9r::Mode &mode=GObjectM9r::Manipulate);

    virtual const FxPoint origin() const;
    virtual void setOrigin(const FxPoint &origin);

    virtual void resize(const FxRect &boundingRect);

private:
    GBackground &operator=(const GBackground &rhs);

    FxRect m_rect;
    bool m_transparent;
};


class GBackgroundM9r : public G2DObjectM9r {

    Q_OBJECT
public:
    GBackgroundM9r(GBackground *background, const Mode &mode, GraphitePart *part,
              GraphiteView *view, const QString &type);
    virtual ~GBackgroundM9r() {}

    // RMB popup on press
    virtual bool mousePressEvent(QMouseEvent *e, QRect &dirty);

    virtual GObject *gobject() { return m_background; }

private:
    GBackgroundM9r(const GBackgroundM9r &rhs);
    GBackgroundM9r &operator=(GBackgroundM9r &rhs);

    GBackground *m_background;
};

#endif // gbackground_h
