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

#ifndef gobject_h
#define gobject_h

#include <qobject.h>
#include <qbrush.h>
#include <qpen.h>
#include <qrect.h>
#include <qpoint.h>

#include <math.h>

#include <graphiteglobal.h>

class QDomElement;
class QDomDocument;
class QPoint;
class QRect;
class QPainter;
class QMouseEvent;
class QKeyEvent;

class KDialogBase;

class GObject;
class GraphiteView;


// This is the manipulator class for GObject. Manipulators (M9r's)
// are used to handle the creation, selection, movement, rotation,...
// of objects.
// The pure virtual GObject::createM9r() factory method ensures that
// the correct manipulator is created :) (factory method pattern)
// The M9r is used every time a user wants to create or change an object
// interactively.
// First the object is "hit" - then a M9r is created and this M9r is used as
// a kind of EventFilter. Every Event is forwarded to the M9r. If the M9r
// decides to handle the event, it returns true afterwards. If the Event
// remains unhandled, the M9r returns false and the Event has to be processed
// by the calling method.
// Whenever a repaint is needed (movement,...), the dirty rect has to be
// set (i.e. something different to (0, 0, 0, 0)).
// Some of the M9rs can be in two different "modes": Create and Manipulate
// General rule: simple M9rs support Create, complex ones do not :)
class GObjectM9r : public QObject {

public:
    enum Mode { Create, Manipulate };

    virtual ~GObjectM9r();

    const Mode &mode() const { return m_mode; }
    void setMode(const Mode &mode) { m_mode=mode; }

    // call drawHandles
    virtual void draw(QPainter &p) = 0;

    virtual const bool mouseMoveEvent(QMouseEvent */*e*/, GraphiteView */*view*/,
				      QRect &/*dirty*/) { return false; }
    virtual const bool mousePressEvent(QMouseEvent */*e*/, GraphiteView */*view*/,
				       QRect &/*dirty*/) { return false; }
    virtual const bool mouseReleaseEvent(QMouseEvent */*e*/, GraphiteView */*view*/,
					 QRect &/*dirty*/) { return false; }
    virtual const bool mouseDoubleClickEvent(QMouseEvent */*e*/, GraphiteView */*view*/,
					     QRect &/*dirty*/) { return false; }

    virtual const bool keyPressEvent(QKeyEvent */*e*/, GraphiteView */*view*/,
				     QRect &/*dirty*/) { return false; }
    virtual const bool keyReleaseEvent(QKeyEvent */*e*/, GraphiteView */*view*/,
				       QRect &/*dirty*/) { return false; }

    virtual GObject *gobject() = 0;

protected:
    GObjectM9r(const Mode &mode) : QObject(), m_mode(mode), first_call(true),
				   m_dialog(0L) {}

    // This menthod returns a property dialog for an object. It
    // creates an empty KDialogBase (IconList mode!) or returns the
    // existing one.
    // If you decide to override this method make sure that the first
    // thing you do in your implementation is calling this method of
    // your parent. Then add your pages to the dialog.
    // Note: This dialog is modal and it has an "Apply" button. The
    // user is able to change the properties and see the result after
    // pressing 'Apply'.
    // This dialog will be destroyed whenever the M9r gets deleted.
    virtual KDialogBase *createPropertyDialog(QWidget *parent);

    Mode m_mode;
    bool first_call; // Whether this is the first call for this M9r (no hit test!)
    KDialogBase *m_dialog;
};


// Provides a dialog with a "pen" page
class G1DObjectM9r : public GObjectM9r {

    Q_OBJECT
public:
    virtual ~G1DObjectM9r() {}

protected slots:
    virtual void setPenStyle(int style);
    virtual void setPenWidth(int width);
    virtual void setPenColor(const QColor &color);

protected:
    G1DObjectM9r(GObject *object, const Mode &mode) : GObjectM9r(mode),
						      m_object(object) {}
    virtual KDialogBase *createPropertyDialog(QWidget *parent);

private:
    GObject *m_object;
};

// TODO G2DObjectM9r - with an additional gradient/brush page

// The abstract base classes for all graphic objects. This class is
// implemented as a composite (pattern) - sort of :)
// There are complex classes (classes which are composed of many
// objects, like a group) and leaf classes which don't have any
// children.
// The resulting tree represents the Z-Order of the document. (First
// the object draws "itself" and then its children)
class GObject {

public:
    enum State { Visible, Handles, Rot_Handles, Invisible, Deleted }; // all possible states
    enum FillStyle { Brush, GradientFilled };  // all possible fill styles
    enum Position { First, Last, Current }; // where to insert the new child object

    virtual ~GObject() {}

    virtual const bool isOk() const { return m_ok; }
    virtual void setOk(const bool &ok=true) { m_ok=ok; }

    virtual GObject *clone() const = 0;           // exact copy of "this" (calls the Copy-CTOR)
    // create an object and initialize it with the given XML (calls the XML-CTOR)
    virtual GObject *instantiate(const QDomElement &element) const = 0;

    const GObject *parent() const { return m_parent; }
    void setParent(GObject *parent) const;   // parent==0L - no parent, parent==this - illegal

    // These two methods are only implemented for "complex" objetcs!
    // The child is inserted at GObject::Position
    virtual const bool plugChild(GObject */*child*/, const Position &/*pos*/=Current) { return false; }
    virtual const bool unplugChild(GObject */*child*/) { return false; }

    // These methods are used to access the object's children
    // Implemented via QListIterator - Leaf classes don't override
    // that default behavior...
    virtual const GObject *firstChild() const { return 0L; }
    virtual const GObject *nextChild() const { return 0L; }
    virtual const GObject *lastChild() const { return 0L; }
    virtual const GObject *prevChild() const { return 0L; }
    virtual const GObject *current() const { return 0L; }

    virtual QDomElement save(QDomDocument &doc) const; // save the object (and all its children) to xml

    // toPrinter is set when we print the document - this means we don't
    // have to paint "invisible" (normally they are colored gray) objects
    // Whenever you paint an object, check if it is within the requested
    // area (region). Add the area of "your" object (boundingRect) to the
    // region to maintain a correct Z order!
    virtual void draw(QPainter &p, QRegion &reg, const bool toPrinter=false) = 0;
    // Used to draw the handles/rot-handles when selected
    virtual void drawHandles(QPainter &p) = 0;

    const int &zoom() const { return m_zoom; }
    virtual void setZoom(const short &zoom=100) { m_zoom=zoom; }
    // don't forget to set it for all children! Note: Check, if the zoom is equal to the last
    // one - don't change it, then...

    // does the object contain this point? (Note: finds the most nested child which is hit!)
    virtual const GObject *hit(const QPoint &p) const = 0;
    virtual const bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect &boundingRect() const = 0;            // the bounding rectangle of this object

    virtual GObjectM9r *createM9r(const GObjectM9r::Mode &mode=GObjectM9r::Manipulate) = 0;

    const QString &name() const { return m_name; }       // name of the object (e.g. "Line001")
    void setName(const QString &name) { m_name=name; }   // set the name

    virtual const QPoint origin() const = 0;             // the origin coordinate of the obj
    virtual void setOrigin(const QPoint &origin) = 0;
    virtual void moveX(const int &dx) = 0;
    virtual void moveY(const int &dy) = 0;
    virtual void move(const int &dx, const int &dy) = 0;

    // Note: radians!
    virtual void rotate(const QPoint &center, const double &angle) = 0;
    virtual void setAngle(const double &angle) { m_angle=angle; }
    const double &angle() const { return m_angle; }

    virtual void scale(const QPoint &origin, const double &xfactor, const double &yfactor) = 0;
    virtual void resize(const QRect &boundingRect) = 0;  // resize, that it fits in this rect

    const State &state() const { return m_state; }               // what's the current state?
    virtual void setState(const State state) { m_state=state; } // set the state

    const FillStyle &fillStyle() const { return m_fillStyle; }
    virtual void setFillStyle(const FillStyle &fillStyle) { m_fillStyle=fillStyle; }
    const QBrush &brush() const { return m_brush; }         // Fill style (brush)
    virtual void setBrush(const QBrush &brush) { m_brush=brush; }
    const Gradient &gradient() const { return m_gradient; } // Gradient filled
    virtual void setGradient(const Gradient &gradient) { m_gradient=gradient; }
    const QPen &pen() const { return m_pen; }               // Pen for the lines
    virtual void setPen(const QPen &pen) { m_pen=pen; }

protected:
    GObject(const QString &name=QString::null);
    GObject(const GObject &rhs);
    GObject(const QDomElement &element);

    // zoomIt zooms a value according to the current zoom setting and returns
    // the zoomed value. The original value is not changed
    const double zoomIt(const double &value) const;
    const int zoomIt(const int &value) const;
    const unsigned int zoomIt(const unsigned int &value) const;
    const QPoint zoomIt(const QPoint &point) const;

    // rotatePoint rotates a given point. The "point" (x, y, or QPoint) passed as
    // function argument is changed! (we're using radians!)
    void rotatePoint(int &x, int &y, const double &angle, const QPoint &center) const;
    void rotatePoint(unsigned int &x, unsigned int &y, const double &angle, const QPoint &center) const;
    void rotatePoint(double &x, double &y, const double &angle, const QPoint &center) const;
    void rotatePoint(QPoint &p, const double &angle, const QPoint &center) const;

    // scalePoint scales a given point. The "point" (x, y, or QPoint) passed as
    // function argument is changed!
    void scalePoint(int &x, int &y, const double &xfactor, const double &yfactor,
		    const QPoint &center) const;
    void scalePoint(unsigned int &x, unsigned int &y, const double &xfactor,
		    const double &yfactor, const QPoint &center) const;
    void scalePoint(double &x, double &y, const double &xfactor, const double &yfactor,
		    const QPoint &center) const;
    void scalePoint(QPoint &p, const double &xfactor, const double &yfactor,
		    const QPoint &center) const;

    const int double2Int(const double &value) const;   // convert back to int

    QString m_name;                              // name of the object
    State m_state;                               // are there handles to draw or not?
    mutable GObject *m_parent;
    int m_zoom;                                  // zoom value 100 -> 100% -> 1
    double m_angle;			         // angle (radians!)

    mutable bool m_boundingRectDirty;            // is the cached bounding rect still correct?
    mutable QRect m_boundingRect;           // bounding rect (cache) - don't use directly!

    FillStyle m_fillStyle;
    QBrush m_brush;
    Gradient m_gradient;
    QPen m_pen;

    bool m_ok;      // used to express errors (e.g. during loading)

private:
    GObject &operator=(const GObject &rhs); // don't assign the objects, clone them
};


inline const double GObject::zoomIt(const double &value) const {
    if(m_zoom==100)
	return value;
    return (static_cast<double>(m_zoom)*value)/100.0;
}

inline const int GObject::zoomIt(const int &value) const {
    if(m_zoom==100)
	return value;
    return (m_zoom*value)/100;
}

inline const unsigned int GObject::zoomIt(const unsigned int &value) const {
    if(m_zoom==100)
	return value;
    return (m_zoom*value)/100;
}

inline const QPoint GObject::zoomIt(const QPoint &point) const {
    if(m_zoom==100)
	return point;
    return QPoint(zoomIt(point.x()), zoomIt(point.y()));
}

inline void GObject::rotatePoint(int &x, int &y, const double &angle, const QPoint &center) const {

    double alpha=angle/2;
    double dx=static_cast<double>(x-center.x());
    double dy=static_cast<double>(y-center.y());
    double r=std::sqrt(dx*dx+dy*dy);
    double s=QABS(2*r*std::sin(alpha));
    double gamma1=std::asin( QABS(dy)/r );
    double gamma;

    if(dx>=0 && dy>=0)
	gamma=-gamma1;
    else if(dx<0 && dy>=0)
	gamma=gamma1+M_PI;
    else if(dx<0 && dy<0)
	gamma=M_PI-gamma1;
    else // dx>=0 && dy<0
	gamma=gamma1;

    double beta=gamma+angle+M_PI_2-QABS(alpha);
    y+=double2Int(s*std::sin(beta));
    x+=double2Int(s*std::cos(beta));
}

inline void GObject::rotatePoint(unsigned int &x, unsigned int &y, const double &angle, const QPoint &center) const {

    // This awkward stuff with the tmp variables is a workaround for
    // "old" compilers (egcs-1.1.2 :)
    int _x=static_cast<int>(x);
    int _y=static_cast<int>(y);
    rotatePoint(_x, _y, angle, center);
    x=static_cast<unsigned int>(_x);
    y=static_cast<unsigned int>(_y);
}

inline void GObject::rotatePoint(double &x, double &y, const double &angle, const QPoint &center) const {

    double alpha=angle/2;
    double dx=x-static_cast<double>(center.x());
    double dy=y-static_cast<double>(center.y());
    double r=std::sqrt(dx*dx+dy*dy);
    double s=QABS(2*r*std::sin(alpha));
    double gamma1=std::asin( QABS(dy)/r );
    double gamma;

    if(dx>=0 && dy>=0)
	gamma=-gamma1;
    else if(dx<0 && dy>=0)
	gamma=gamma1+M_PI;
    else if(dx<0 && dy<0)
	gamma=M_PI-gamma1;
    else // dx>=0 && dy<0
	gamma=gamma1;

    double beta=gamma+angle+M_PI_2-QABS(alpha);
    y+=s*std::sin(beta);
    x+=s*std::cos(beta);
}

inline void GObject::rotatePoint(QPoint &p, const double &angle, const QPoint &center) const {
    rotatePoint(p.rx(), p.ry(), angle, center);
}

inline void GObject::scalePoint(int &x, int &y, const double &xfactor, const double &yfactor,
			 const QPoint &center) const {
    if(xfactor<=0 || yfactor<=0)
	return;
    x=double2Int( static_cast<double>(center.x()) + static_cast<double>(x-center.x())*xfactor );
    y=double2Int( static_cast<double>(center.y()) + static_cast<double>(y-center.y())*yfactor );
}

inline void GObject::scalePoint(unsigned int &x, unsigned int &y, const double &xfactor,
			 const double &yfactor, const QPoint &center) const {
    // This awkward stuff with the tmp variables is a workaround for
    // "old" compilers (egcs-1.1.2 :)
    int _x=static_cast<int>(x);
    int _y=static_cast<int>(y);
    scalePoint(_x, _y, xfactor, yfactor, center);
    x=static_cast<unsigned int>(_x);
    y=static_cast<unsigned int>(_y);
}

inline void GObject::scalePoint(double &x, double &y, const double &xfactor, const double &yfactor,
			 const QPoint &center) const {
    if(xfactor<=0 || yfactor<=0)
	return;
    x=static_cast<double>(center.x()) + static_cast<double>(x-center.x())*xfactor;
    y=static_cast<double>(center.y()) + static_cast<double>(y-center.y())*yfactor;
}

inline void GObject::scalePoint(QPoint &p, const double &xfactor, const double &yfactor,
			 const QPoint &center) const {
    scalePoint(p.rx(), p.ry(), xfactor, yfactor, center);
}

inline const int GObject::double2Int(const double &value) const {

    if( static_cast<double>((value-static_cast<int>(value)))>=0.5 )
	return static_cast<int>(value)+1;
    else if( static_cast<double>((value-static_cast<int>(value)))<=-0.5 )
	return static_cast<int>(value)-1;
    else
	return static_cast<int>(value);
}
#endif
