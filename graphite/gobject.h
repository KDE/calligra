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

// The abstract base classes for all graphic objects. This class is
// implemented as a composite (pattern) - sort of :)
// There are complex classes (classes which are composed of many
// objects, like a group) and leaf classes which don't have any
// children.
// The resulting tree represents the Z-Order of the document.

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
class QMouseEvent;
class QKeyEvent;

class GObjectM9r;


class GObject {

public:
    enum State { Visible, Handles, Rot_Handles, Invisible, Deleted };  // all possible states
    enum FillStyle { Brush, GradientFilled };                     // all possible fill styles
    enum Position { First, Last, Current };

    virtual ~GObject();

    virtual GObject *clone() const = 0;           // exact copy of "this" (calls the Copy-CTOR)
    virtual GObject *instantiate(const QDomElement /*&element*/) {}  // used in the object factory

    const GObject *parent() const { return m_parent; }
    void setParent(GObject *parent);               // parent==0L - no parent, parent==this - illegal

    // These two methods are only implemented for "complex" objetcs!
    // The child is inserted at GObject::Position
    virtual const bool plugChild(GObject */*child*/, const Position &/*pos*/=Current) { return false; }
    virtual const bool unplugChild(GObject */*child*/, const Position &/*pos*/=Current) { return false; }

    // These methods are used to access the object's children
    // Implemented via QListIterator - Leaf classes
    virtual const GObject *firstChild() { return 0L; }
    virtual const GObject *nextChild() { return 0L; }
    virtual const GObject *lastChild() { return 0L; }
    virtual const GObject *prevChild() { return 0L; }
    virtual const GObject *current() { return 0L; }

    virtual QDomElement save(QDomDocument &doc) const; // save the object to xml

    // toPrinter is set when we print the document - this means we don't
    // have to paint "invisible" (normally they are colored gray) objects
    virtual void draw(const QPainter &p, const bool toPrinter=false) const = 0;  // guess :)
    // (TODO) Do we need more/other args?

    virtual const GObject *hit(const QPoint &p) const = 0;   // does the object contain this point?
    virtual const bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect &boundingRect() const = 0;            // the bounding rectangle of this object

    virtual GObjectM9r *createM9r();        // create a Manipulator (M9r :) for that object

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


// This is the manipulator class for GObject. Manipulators (M9r's)
// are used to handle the selection, movement, rotation,... of objects.
// The pure virtual GObject::createM9r() method ensures that the correct
// manipulator is created :)
// The M9r is used every time a user wants to change an object. First the
// object is "hit" - then a M9r is created and this M9r is used as a kind
// of EventFilter. Every Event is forwarded to the M9r. If the M9r handles
// the event, it returns true. If the Event remains unhandled, the M9r
// returns false and the Event has to be processed  by the calling method.
class GObjectM9r {
public:
    virtual ~GObjectM9r() {}
    
    virtual const bool mouseMoveEvent(QMouseEvent */*e*/) { return false; }
    virtual const bool mousePressEvent(QMouseEvent */*e*/) { return false; }
    virtual const bool mouseReleaseEvent(QMouseEvent */*e*/) { return false; }
    virtual const bool mouseDoubleClickEvent(QMouseEvent */*e*/) { return false; }

    virtual const bool keyPressEvent(QKeyEvent */*e*/) { return false; }
    virtual const bool keyReleaseEvent(QKeyEvent */*e*/) { return false; }

private:
    GObjectM9r &operator=(GObjectM9r &rhs);  // no nasty tricks, please :)
};
#endif
