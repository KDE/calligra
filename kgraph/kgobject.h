// The abstract base classes for all graphic objects

#ifndef kgobject_h
#define kgobject_h

#include <qobject.h>

class QDomElement;
class QDomDocument;
class QPoint;
class QRect;
class QPainter;
class QActionCollection;

class KGObject : public QObject {

    Q_OBJECT

public:
    KGObject();
    KGObject(const KGObject &rhs);
    KGObject(const QDomElement &e);              // create an object from xml (loading)

    virtual KGObject *clone() const = 0;         // exact copy of "this" (calls the Copy-CTOR)

    virtual ~KGObject();

    virtual QDomElement save(const QDomDocument &doc) const;  // save the object to xml
    // Just add this element to the one from the derived object and call the KGObject()
    // CTOR on loading (from the virtual CTOR in the part)

    virtual void draw(const QPainter &p) const = 0;        // guess :)

    enum STATE { NO_HANDLES, HANDLES, ROTATION_HANDLES, INVISIBLE }; // all possible states
    virtual void setState(const STATE state=NO_HANDLES);   // set the state
    virtual const STATE state() const { return m_state; }  // what's the current state?

    virtual const bool contains(const QPoint &p) const = 0;   // does the object contain this point?
    virtual const bool intersects(const QRect &r) const = 0;  // does the object intersect the rectangle?
    virtual const QRect boundingRect() const = 0;             // the bounding rectangle of this object

    const QActionCollection* popupActions() const { return popup; } // return all the actions provided from
                                                                   // that object. Don't forget to init it!

    // (TODO)
    // TileID (mehrere möglich!): Hier oder im Pool?
    // Z-Koordinate oder Reihenfolge in der QList vom ObjectPool?
    // Position ja/nein (ja/nein==hier oder Pool)?
    // Zoom ja/nein?
    // Rotation ja/nein?
    // Linieneigenschaften (Farbe, Style, Breite,...)
    // ...
signals:
    void requestRepaint();  // maybe only for a specific region? (requestRepaint(const QRect &))???

private:
    KGObject &operator=(const KGObject &rhs);    // don't assign the objects, clone them

    STATE m_state;                               // are there handles to draw or not?

    mutable bool boundingRectDirty;              // is the cached bounding rect still correct?
    mutable QRect bounds;                        // bounding rect (cache)

    QActionCollection *popup;                     // all the actions which are available
};


// (TODO) This is the abstract base class for all objects which have a "filled area"
// like rectangles, circles,... All the others (lines, ploy-lines,...) inhertit
// the plain KGObject
class KGFilledObject : public KGObject {

    Q_OBJECT

public:
    // fill style, color, brush,...
private:

};
#endif
