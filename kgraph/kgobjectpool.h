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

// The class which handles the memory management
// It simply stores all the objects in a list and
// deletes every element in the list if it gets destroyed.
// The order in this list also represents the Z-sorting for
// the chart's elements (a kind of "Painter's Algorithm").

#ifndef kgobjectpool_h
#define kgobjectpool_h

#include <qlist.h>

#include <kgobject.h>
#include <kggenericpool.h>

class QRect;
class KGraphPart;


class KGObjectPool : public KGGenericPool<KGObject> {

public:
    KGObjectPool(const KGraphPart * const part);
    virtual ~KGObjectPool() {}

    virtual const bool remove(const unsigned int &index);
    virtual const bool remove(const KGObject *object);

    const bool toFront(const KGObject *object); // moves the object to the front (last item)
    const bool toBack(const KGObject *object);  // moves the object to the back (first item)
    const bool forwardOne(const KGObject *object);  // moves it one step towards the user
    const bool backwardOne(const KGObject *object); // moves it one step away form the user

    KGObject *createObject(const QDomElement &e);   // load an object from XML. The Right
                                                    // Object(tm) is created.

    //KGLine *createLine(); // default line (from (0|0) to (1|1))
    //KGLine *createLine(const QDomElement &e);   // used to "load" a line object
    //KGLine *createLine(const QPoint &a, const QPoint &b);

public slots:
    void requestRepaint(const QRect &r);
    void requestRepaint();
    
private:
    KGObjectPool &operator=(const KGObjectPool &rhs);

    const KGraphPart * const m_part;  // a ptr to our part (b/c of SLOTs)

    // This list stores the active objects or objects which
    // are created at the moment. These objects need serious
    // repainting and if the "conditions" are OK we only have
    // to bitBlt the old double buffer and draw a few objects.
    // This should generally be faster than drawing everything.
    // The "conditions": No object from the pool requested
    // a repaint (partly (=region) or fully) and we don't have
    // to draw a transparent view.
    // Whenever an object from the pool is removed and inserted
    // here I have to make sure that setAutoDelete is set to
    // false and set back to true after the removal!!!
    // (TODO): Check out the double buffer stuff with the
    // Canvas :)
    QList<KGObject> activeObject;
    
    bool m_dirty;       // a repaint was requested
    QRect rect;         // for this region (0, 0, 0, 0) -> total repaint
};
#endif // kgobjectpool_h
