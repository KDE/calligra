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

#ifndef gtool_h
#define gtool_h

#include <qapp.h>

class QMouseEvent;
class QKeyEvent;
class QPainter;
class QRegion;
class GObject;

// This class handles the user input when creating a new object.
// Every primitive object (i.e. every object which can be created
// via the insert menu, like lines, rectangles,...) represents
// a tool (i.e. rect -> GRectTool). The doc holds a pointer to
// a GTool and whenever a tool is active, this pointer is !=0L
// If that is the case, the doc forwards all the events to this
// object. If the object decides to handle the event, it has to
// return "true" - if if returns "false" (i.e. it was not able
// to handle the event), the object gets destroyed (Tool-Mode
// is off, again). Before destorying the GTool object and setting
// the ptr back to 0L, we fetch the new object and insert it
// into the hierarchy. The draw stuff is forwared to the new
// object being created...

class GTool {

public:
    virtual ~GTool() { QApplication::restoreOverrideCursor(); }
    
    virtual void draw(const QPainter &p, const QRegion &reg, const bool toPrinter=false) const = 0;
    
    virtual const bool mousePressEvent(QMouseEvent */*e*/) { return false; }
    virtual const bool mouseReleaseEvent(QMouseEvent */*e*/) { return false; }
    virtual const bool mouseDoubleClickEvent(QMouseEvent */*e*/) { return false; }
    virtual const bool mouseMoveEvent(QMouseEvent */*e*/) { return false; }

    virtual const bool keyPressEvent(QKeyEvent */*e*/) { return false; }
    virtual const bool keyReleaseEvent(QKeyEvent */*e*/) { return false; }
    
    virtual GObject *gobject() = 0;  // fetch the new object

protected:
    GTool() { QApplication::setOverrideCursor(Qt::crossCursor); }
};
#endif // gtool_h
