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

#ifndef gobject_h
#define gobject_h

#include <qlist.h>
#include <qbrush.h>
#include <qpen.h>
#include <qrect.h>
#include <qpoint.h>
#include <qwidget.h>

#include <kdialogbase.h>

#include <math.h>

#include <graphiteglobal.h>

class QDomElement;
class QDomDocument;
class QPoint;
class QRect;
class QPainter;
class QMouseEvent;
class QKeyEvent;
class QResizeEvent;
class QLineEdit;
class QComboBox;
class QVButtonGroup;
class QWidgetStack;
class QCheckBox;
class QSlider;
class QSizePolicy;

class KDialogBase;
class KColorButton;
class KIntSpinBox;

class GObject;
class GraphiteView;
class GraphitePart;
class PWidget;


// This is the manipulator class for GObject. Manipulators (M9r's)
// are used to handle the creation, selection, movement, rotation,...
// of objects. They also provide a property dialog (lazy creation)
// The pure virtual GObject::createM9r() factory method ensures that
// the correct manipulator is created :) (factory method pattern)
// The M9r is used every time a user wants to create or change an object
// interactively.
// First the object is "hit" - then a M9r is created and this M9r is used as
// a kind of EventFilter. Every Event is forwarded to the M9r. If the M9r
// decides to handle the event, it returns true afterwards. If the Event
// remains unhandled, the M9r returns false and the Event has to be processed
// by the calling method.
// Note: The M9r is bound to a specific view and it won't work (correctly)
// if you use one M9r for more than one view. Maybe I need some sort of
// map or dict which relates a view to a M9r? (TODO)
// Whenever a repaint is needed (movement,...), the dirty rect has to be
// set (i.e. something different to (0, 0, 0, 0)).
// Some of the M9rs can be in two different "modes": Create and Manipulate
// General rule: simple M9rs support Create, complex ones do not :)
class GObjectM9r : public KDialogBase {

    Q_OBJECT
public:
    enum Mode { Create, Manipulate };

    virtual ~GObjectM9r();

    const Mode &mode() const { return m_mode; }
    void setMode(const Mode &mode) { m_mode=mode; }

    const GraphiteView *view() const { return m_view; }

    // the handles() contain the handle rects for mouse over stuff afterwards
    virtual void draw(QPainter &p);

    // return false when you couldn't handle the event
    virtual bool mouseMoveEvent(QMouseEvent */*e*/, QRect &/*dirty*/) { return false; }
    virtual bool mousePressEvent(QMouseEvent */*e*/, QRect &/*dirty*/) { return false; }
    virtual bool mouseReleaseEvent(QMouseEvent */*e*/, QRect &/*dirty*/) { return false; }
    virtual bool mouseDoubleClickEvent(QMouseEvent */*e*/, QRect &/*dirty*/) { return false; }

    // default impl!
    virtual bool keyPressEvent(QKeyEvent */*e*/, QRect &/*dirty*/) { return false; }
    virtual bool keyReleaseEvent(QKeyEvent */*e*/, QRect &/*dirty*/) { return false; }

    virtual GObject *gobject() { return m_object; }

protected slots:
    // All these slots just tell us that something has been changed
    // Yes, I know that this is an ugly hack :(
    // It was necessary to either have this or to store the temporary
    // values (because of the Ok/Apply/Cancel stuff).
    virtual void slotChanged(const QString &);
    virtual void slotChanged(int);
    virtual void slotChanged(const QColor &);

    virtual void slotOk();
    virtual void slotApply();
    virtual void slotCancel();

protected:
    GObjectM9r(GObject *object, const Mode &mode, GraphitePart *part,
               GraphiteView *view, const QString &type);

    // This menthod returns a property dialog for the object. It
    // creates a dialog (i.e. adds a few pages to *this). The
    // dialog is cached for further use and destroyed on destrucion
    // of this object.
    // If you decide to override this method make sure that the first
    // thing you do in your implementation is checking whether the dialog
    // has been created already. Then call the method of your parent.
    // Then add your pages to the dialog and initialize the contents.
    // Note: This dialog is modal and it has an "Apply" button. The
    // user is able to change the properties and see the result after
    // clicking 'Apply'.
    virtual void createPropertyDialog();

    bool created() const { return m_created; }
    //bool firstCall() const { return m_firstCall; }
    //void setFirstCall(bool firstCall) { m_firstCall=firstCall; }
    QList<QRect> *handles() const { return m_handles; }

private:
    GObjectM9r(const GObjectM9r &rhs);
    GObjectM9r &operator=(const GObjectM9r &rhs);

    GObject *m_object;
    Mode m_mode;
    GraphitePart *m_part;     // we need that for the history
    QList<QRect> *m_handles;  // contains all the handle rects
    //bool m_firstCall : 1; // Whether this is the first call for this M9r (no hit test!)
    bool m_pressed;           // mouse button pressed?
    bool m_changed;           // true, if the Apply button is "active"
    bool m_created;           // dia created?

    QString m_type;         // Type of object (e.g. "Line", "Rectangle")
    QLineEdit *m_line;      // line ed. for the name field
    GraphiteView *m_view;   // "our" parent view
};


// This class adds a "Pen" dialog page to the empty dialog
class G1DObjectM9r : public GObjectM9r {

    Q_OBJECT
public:
    virtual ~G1DObjectM9r() {}

protected slots:
    virtual void slotApply();

protected:
    G1DObjectM9r(GObject *object, const Mode &mode, GraphitePart *part,
                 GraphiteView *view, const QString &type) :
        GObjectM9r(object, mode, part, view, type) {}
    virtual void createPropertyDialog();

private:
    G1DObjectM9r(const G1DObjectM9r &rhs);
    G1DObjectM9r &operator=(const G1DObjectM9r &rhs);

    KIntSpinBox *m_width;
    KColorButton *m_color;
    QComboBox *m_style;
};


// This class adds a fill style (none/brush/gradient) page to the dialog
class G2DObjectM9r : public G1DObjectM9r {

    Q_OBJECT
public:
    virtual ~G2DObjectM9r() {}

    void updatePage();

protected slots:
    virtual void slotChanged(int x);
    virtual void slotChanged(const QColor &x);

    virtual void slotApply();
    virtual void resizeEvent(QResizeEvent *e);  // update the preview on resize

protected:
    G2DObjectM9r(GObject *object, const Mode &mode, GraphitePart *part,
                 GraphiteView *view, const QString &type, bool gradient=true) :
        G1DObjectM9r(object, mode, part, view, type), m_gradient(gradient) {}
    virtual void createPropertyDialog();

private slots:
    void slotBalance();   // activate/deactivate the sliders (xfactor/yfactor)

private:
    G2DObjectM9r(const G2DObjectM9r &rhs);
    G2DObjectM9r &operator=(const G2DObjectM9r &rhs);
    void updatePreview(int btn);

    QVButtonGroup *m_style;
    PWidget *m_preview;
    QWidgetStack *m_stack;
    KColorButton *m_brushColor;
    QComboBox *m_brushStyle;
    bool m_gradient;  // do we show the gradient widget?
    KColorButton *m_gradientCA, *m_gradientCB;
    QComboBox *m_gradientStyle;
    QCheckBox *m_unbalanced;
    QSlider *m_xfactor, *m_yfactor;
};


// The abstract base classes for all graphic objects. This class is
// implemented as a composite (pattern) - sort of :)
// There are complex classes (classes which are composed of many
// objects, like a group) and leaf classes which don't have any
// children (e.g. line, rect,...).
// The resulting tree represents the Z-Order of the document. (First
// the object draws "itself" and then its children)
class GObject {

public:
    enum State { Visible, Handles, Rot_Handles, Invisible, Deleted }; // all possible states
    enum FillStyle { Brush, GradientFilled };  // all possible fill styles
    enum Position { First, Last, Current }; // where to insert the new child object

    virtual ~GObject() {}

    virtual bool isOk() const { return m_ok; }
    virtual void setOk(bool ok=true) { m_ok=ok; }

    virtual GObject *clone() const = 0;           // exact copy of "this" (calls the Copy-CTOR)
    // create an object and initialize it with the given XML (calls the XML-CTOR)
    virtual GObject *instantiate(const QDomElement &element) const = 0;

    // When the zoom factor changes we have to propagate this to all children!
    virtual void setDirty() = 0;  // don't forget to call this default impl.!
    bool dirty() const { return m_dirty; }

    const GObject *parent() const { return m_parent; }
    void setParent(GObject *parent) const;   // parent==0L - no parent, parent==this - illegal

    // These two methods are only implemented for "complex" objetcs!
    // The child is inserted at GObject::Position
    virtual bool plugChild(GObject */*child*/, const Position &/*pos*/=Current) { return false; }
    virtual bool unplugChild(GObject */*child*/) { return false; }

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
    virtual void draw(QPainter &p, const QRect &rect, bool toPrinter=false) const = 0;
    // Used to draw the handles/rot-handles when selected
    // All handles which are drawn are added to the list if the list
    // is != 0L. Use this list to check "mouseOver" stuff
    // drawing: setROP(Not)
    virtual void drawHandles(QPainter &p, QList<QRect> *handles=0L) const;

    // does the object contain this point? (Note: finds the most nested child which is hit!)
    virtual const GObject *hit(const QPoint &p) const = 0;
    virtual bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect &boundingRect() const = 0;  // the bounding rectangle of this object

    virtual GObjectM9r *createM9r(GraphitePart *part, GraphiteView *view,
                                  const GObjectM9r::Mode &mode=GObjectM9r::Manipulate) = 0;

    QString name() const { return m_name; }       // name of the object (e.g. "Line001")
    void setName(const QString &name) { m_name=name; }   // set the name

    virtual const FxPoint origin() const = 0;             // the origin coordinate of the obj
    virtual void setOrigin(const FxPoint &origin) = 0;
    virtual void moveX(const double &dx) = 0;
    virtual void moveY(const double &dy) = 0;
    virtual void move(const double &dx, const double &dy) = 0;

    // Note: radians!
    virtual void rotate(const FxPoint &center, const double &angle) = 0;
    virtual const double &angle() const { return m_angle; }

    virtual void scale(const FxPoint &origin, const double &xfactor, const double &yfactor) = 0;
    virtual void resize(const FxRect &boundingRect) = 0;  // resize, that it fits in this rect

    const State &state() const { return m_state; }              // what's the current state?
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

    // This one has to be called when the status is "dirtiy" and we access the coords.
    virtual void recalculate() const = 0; // don't forget to call!

    bool boundingRectDirty() const { return m_boundingRectDirty; }
    void setBoundingRectDirty(bool dirty=true) const { m_boundingRectDirty=dirty; }
    void setBoundingRect(const QRect &br) const { m_boundingRect=br; }

    void setAngle(const double angle) const { m_angle=angle; }

private:
    GObject &operator=(const GObject &rhs); // don't assign the objects, clone them

    QString m_name;                    // name of the object
    State m_state;                     // are there handles to draw or not?
    mutable GObject *m_parent;
    mutable double m_angle;            // angle (radians!)

    mutable bool m_boundingRectDirty;  // is the cached bounding rect still correct?
    mutable QRect m_boundingRect;      // bounding rect (cache) - don't use directly!

    FillStyle m_fillStyle;
    QBrush m_brush;
    Gradient m_gradient;
    QPen m_pen;

    bool m_ok;      // used to express errors (e.g. during loading)
    mutable bool m_dirty;   // the zoom factor changed (->recalc on next use)
};


// This *huge* class is needed to present the preview pixmap.
// It is simply a plain Widget which tries to get all the free
// space it can get (in x and y direction).
class PWidget : public QWidget {

public:
    PWidget(QWidget *w, G2DObjectM9r *mgr) : QWidget(w), manager(mgr) {}

protected:
    void showEvent(QShowEvent *) { manager->updatePage(); }

private:
    G2DObjectM9r *manager;
};

#endif
