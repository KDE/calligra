// The abstract base classes for all graphic objects

#ifndef kgobject_h
#define kgobject_h

#include <qobject.h>
#include <qbrush.h>
#include <qpen.h>

class QDomElement;
class QDomDocument;
class QPoint;
class QRect;
class QPainter;
class QWMatrix;
class KActionCollection;

class KGGroup;

class KGObject : public QObject {

    Q_OBJECT

public:
    virtual KGObject *clone() const = 0;           // exact copy of "this" (calls the Copy-CTOR)

    virtual ~KGObject();

    const QString &name() const { return m_name; } // name of the object (e.g. "Line001")
    void setName(const QString &name);             // set the name

    virtual QDomElement save(const QDomDocument &doc) const; // save the object to xml
    // Just add this element to the one from the derived object and call the KGObject()
    // CTOR on loading (from the virtual CTOR in the part)

    const QPoint origin() const { return m_origin; }  // the origin coordinate of the obj
    virtual void setOrigin(const QPoint &origin);
    virtual void moveX(const int &dx);
    virtual void moveY(const int &dy);
    virtual void move(const QSize &d);

    virtual void draw(const QPainter &p, const bool toPrinter=false) const = 0;  // guess :)

    enum STATE { NO_HANDLES, HANDLES, ROTATION_HANDLES, INVISIBLE };   // all possible states
    const STATE state() const { return m_state; }          // what's the current state?
    virtual void setState(const STATE state);              // set the state

    const KGGroup *group() const { return m_group; }       // are we in a group? which one?
    virtual void setGroup(const KGGroup *group);           // set the group

    virtual const bool contains(const QPoint &p) const = 0;   // does the object contain this point?
    virtual const bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect &boundingRect() const = 0;            // the bounding rectangle of this object

    const KActionCollection *popupActions() const { return popup; } // return all the actions provided from
                                                                    // that object. Don't forget to init it!

    const QBrush brush() const { return m_brush; }         // Fill style (brush)
    virtual void setBrush(const QBrush &brush);
    const QPen pen() const { return m_pen; }               // Outline style (pen)
    virtual void setPen(const QPen &pen);

    // (TODO)
    // Position ja/nein (ja/nein==hier oder Pool)?
    // Double buffering
    // ...

signals:
    void requestRepaint();                     // request a complete repaint
    void requestRepaint(const QRect &);        // request a repaint for this rect

protected:
    KGObject(const QString &name=QString::null);
    KGObject(const KGObject &rhs);
    KGObject(const QDomElement &element);        // create an object from xml (loading)

    STATE m_state;                               // are there handles to draw or not?

    mutable bool boundingRectDirty;              // is the cached bounding rect still correct?
    mutable QRect bounds;                        // bounding rect (cache)

    KActionCollection *popup;                    // all the actions which are available

    QString m_name;                              // name of the object
    QBrush m_brush;
    QPen m_pen;
    KGGroup *m_group;
    QPoint m_origin;

private:
    KGObject &operator=(const KGObject &rhs);    // don't assign the objects, clone them
};
#endif
