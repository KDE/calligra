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

// The abstract base classes for all graphic objects

#ifndef gobject_h
#define gobject_h

#include <qbrush.h>
#include <qpen.h>

#include <graphiteglobal.h>

class QDomElement;
class QDomDocument;
class QPoint;
class QRect;
class QPainter;
class KActionCollection;


class GObject {

public:
    enum State { Visible, Handles, Rot_Handles, Invisible, Deleted };  // all possible states
    enum FillStyle { Brush, GradientFilled };                     // all possible fill styles
    
    virtual ~GObject();

    virtual GObject *clone() const = 0;           // exact copy of "this" (calls the Copy-CTOR)
    virtual GObject *instantiate(const QDomElement /*&element*/) {}  // used in the object factory
    
    const GObject *parent() const { return m_parent; }
    void setParent(GObject *parent);
    
    virtual const bool plugChild(GObject */*child*/) { return true; }
    virtual const bool unplugChild(GObject */*child*/) { return true; }
    // (TODO) Do we need an arg to insert childs where we want to (begin/end/...)?

    virtual QDomElement save(QDomDocument &doc) const; // save the object to xml
    
    virtual void draw(const QPainter &p, const bool toPrinter=false) const = 0;  // guess :)
    // (TODO) Do we need more/other args?

    virtual const bool contains(const QPoint &p) const = 0;   // does the object contain this point?
    virtual const bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect &boundingRect() const = 0;            // the bounding rectangle of this object

    //const KActionCollection *popupActions() const { return popup; } // return all the actions provided by
    // (TODO) Use Simon's new magic actionList stuff for that (plain XML instead of actions)
    
    const QString &name() const { return m_name; }       // name of the object (e.g. "Line001")
    void setName(const QString &name) { m_name=name; }   // set the name

    virtual const QPoint origin() const = 0;             // the origin coordinate of the obj
    virtual void setOrigin(const QPoint &origin) = 0;
    virtual void moveX(const int &dx) = 0;
    virtual void moveY(const int &dy) = 0;
    virtual void move(const int &dx, const int &dy) = 0;

    const State state() const { return m_state; }               // what's the current state?
    virtual void setState(const State state) { m_state=state; } // set the state

    const FillStyle fillStyle() const { return m_fillStyle; }
    virtual void setFillStyle(const FillStyle &fillStyle) { m_fillStyle=fillStyle; }
    const QBrush brush() const { return m_brush; }         // Fill style (brush)
    virtual void setBrush(const QBrush &brush) { m_brush=brush; }
    const Gradient gradient() const { return m_gradient; } // Gradient filled
    virtual void setGradient(const Gradient &gradient) { m_gradient=gradient; }
    const QPen pen() const { return m_pen; }               // Pen for the lines
    virtual void setPen(const QPen &pen) { m_pen=pen; }

signals:
    void requestRepaint();                     // request a complete repaint
    void requestRepaint(const QRect &);        // request a repaint for this rect

protected:
    GObject(const QString &name=QString::null);
    GObject(const GObject &rhs);
    GObject(const QDomElement &element); // create an object from xml (loading)

    QString m_name;                              // name of the object
    State m_state;                               // are there handles to draw or not?
    GObject *m_parent;

    mutable bool boundingRectDirty;              // is the cached bounding rect still correct?
    mutable QRect bounds;                        // bounding rect (cache)

    FillStyle m_fillStyle;
    QBrush m_brush;
    Gradient m_gradient;
    QPen m_pen;

private:
    GObject &operator=(const GObject &rhs);    // don't assign the objects, clone them
};
#endif
