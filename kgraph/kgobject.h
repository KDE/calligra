/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

#ifndef kgobject_h
#define kgobject_h

#include <qobject.h>
#include <qbrush.h>
#include <qpen.h>

#include <kgraph_global.h>

class QDomElement;
class QDomDocument;
class QPoint;
class QRect;
class QPainter;
class KActionCollection;
class KGGroup;
class KGObjectPool;

class KGObject : public QObject {

    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QPoint origin READ origin WRITE setOrigin)
    Q_PROPERTY(STATE state READ state WRITE setState)
    Q_ENUMS(STATE)
    Q_ENUMS(FILL_STYLE)
    Q_PROPERTY(FILL_STYLE fillStyle READ fillStyle WRITE setFillStyle)
    Q_ENUMS(Gradient)
    Q_PROPERTY(Gradient gradient READ gradient WRITE setGradient)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(QPen pen READ pen WRITE setPen)

public:
    virtual KGObject *clone() const = 0;           // exact copy of "this" (calls the Copy-CTOR)
    const KGObjectPool * const pool() const { return m_pool; }

    virtual ~KGObject();

    virtual QDomElement save(QDomDocument &doc) const; // save the object to xml
    // Just add this element to the one from the derived object. Call the KGObject()
    // CTOR on loading (from the virtual CTOR in the part)

    virtual void draw(const QPainter &p, const bool toPrinter=false) const = 0;  // guess :)

    virtual const bool contains(const QPoint &p) const = 0;   // does the object contain this point?
    virtual const bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect &boundingRect() const = 0;            // the bounding rectangle of this object

    const KActionCollection *popupActions() const { return popup; } // return all the actions provided from
                                                                    // that object. Don't forget to init it!
    const QString &name() const { return m_name; }       // name of the object (e.g. "Line001")
    void setName(const QString &name) { m_name=name; }   // set the name

    const QPoint origin() const { return m_origin; }  // the origin coordinate of the obj
    virtual void setOrigin(const QPoint &origin) = 0;
    virtual void moveX(const int &dx) = 0;
    virtual void moveY(const int &dy) = 0;
    virtual void move(const int &dx, const int &dy) = 0;

    enum STATE { VISIBLE, HANDLES, ROTATION_HANDLES, INVISIBLE, DELETED };   // all possible states
    const STATE state() const { return m_state; }               // what's the current state?
    virtual void setState(const STATE state) { m_state=state; } // set the state

    const KGGroup *temporaryGroup() const { return tmpGroup; }
    virtual const bool setTemporaryGroup(KGGroup *group);  // temporary group during a "select" state
    const KGGroup *group() const { return m_group; }       // are we in a group? which one?
    virtual const bool setGroup(KGGroup *group);           // set the group

    enum FILL_STYLE { BRUSH, GRADIENT };                   // all possible fill styles
    const FILL_STYLE fillStyle() const { return m_fillStyle; }
    virtual void setFillStyle(const FILL_STYLE &fillStyle) { m_fillStyle=fillStyle; }
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
    KGObject(const KGObjectPool * const pool, const QString &name=QString::null);
    KGObject(const KGObject &rhs);
    KGObject(const KGObjectPool * const pool, const QDomElement &element); // create an object from xml (loading)

    const KGObjectPool * const m_pool;

    STATE m_state;                               // are there handles to draw or not?
    KGGroup *tmpGroup;

    mutable bool boundingRectDirty;              // is the cached bounding rect still correct?
    mutable QRect bounds;                        // bounding rect (cache)

    KActionCollection *popup;                    // all the actions which are available

    QString m_name;                              // name of the object
    FILL_STYLE m_fillStyle;
    QBrush m_brush;
    Gradient m_gradient;
    QPen m_pen;
    KGGroup *m_group;
    QPoint m_origin;

private:
    KGObject &operator=(const KGObject &rhs);    // don't assign the objects, clone them
};
#endif
