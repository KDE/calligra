/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <PolylineTool.h>

#include <qkeycode.h>
#include <klocale.h>

#include <GDocument.h>
#include <Canvas.h>
#include <Coord.h>
#include <GPolygon.h>
#include <CreatePolylineCmd.h>
#include <CreatePolygonCmd.h>
#include <AddLineSegmentCmd.h>
#include <CommandHistory.h>

#include <math.h>

PolylineTool::PolylineTool (CommandHistory* history) : Tool (history)
{
  line = 0L;
  last = 0;
  newObj = true;
  points.setAutoDelete (true);
  m_id=ToolLine;
}

void PolylineTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == QEvent::KeyPress) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Qt::Key_Escape && line != 0L) {
      /*
       * Abort the last operation
       */
      if (newObj) {
        // for a new object we have to remove the last point
        line->removePoint (last);
        if (line->numOfPoints () > 2) { // XXXXX ??
          // it's a regular line
          CreatePolylineCmd *cmd = new CreatePolylineCmd (doc, line);
          history->addCommand (cmd);
        }
        else {
          // no valid line - delete it
          doc->deleteObject (line);
          line = 0L;
        }
      }
      else {
        // remove the first point: it's equal to the last point
        // of the original line
        points.removeFirst ();
        // we have to remove the last added point
        line->removePoint (last);
        if (points.count () > 0) {
          if (last != 0)
            last = last - points.count ();
          AddLineSegmentCmd *cmd =
            new AddLineSegmentCmd (doc, line, last, points);
          history->addCommand (cmd);
        }
      }
      if (line)
        doc->setLastObject (line);
      line = 0L; last = 0;
      emit operationDone ();
    }
  }
  else if (e->type () == QEvent::MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    if (me->button () != LeftButton)
      return;

    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    if (line != 0L) {
      // continue creation: add a new segment
      if (last != 0)
        last++;
    }
    else {
      newObj = true;
      points.clear ();

      GPolyline* obj = 0L;

      if (me->state () & ShiftButton) {
        // magnetic mode
        GObject *o = 0L;
        int idx = -1;
        if (doc->findNearestObject ("GPolyline", xpos, ypos,
                                    10, o, idx)) {
          line = (GPolyline *) o;
          last = (idx != 0 ? idx + 1 : idx);
          newObj = false;
        }
      }
      else {
        // look for existing polylines with a point near the mouse pointer
        QList<GObject> olist;

        if (doc->findContainingObjects (xpos, ypos, olist)) {
          QListIterator<GObject> it (olist);
          while (it.current ()) {
            if (it.current ()->isA ("GPolyline")) {
              obj = (GPolyline *) it.current ();
              break;
            }
            ++it;
          }
        }
        if (obj && (last = obj->getNeighbourPoint (Coord (xpos, ypos))) != -1
            && (last == 0 || last == (int) obj->numOfPoints () - 1)) {
          line = obj;
          newObj = false;
          if (last != 0)
            // it's not the first point of the line, so update the
            // index
            last += 1;
        }
      }

      if (line == 0L) {
        // no polyline found, create a new one
        line = new GPolyline ();
        line->addPoint (0, Coord (xpos, ypos));
        last = 1;
        newObj = true;
        doc->insertObject (line);
      }
    }
    line->addPoint (last, Coord (xpos, ypos));
  }
  else if (e->type () == QEvent::MouseMove) {
    if (line == 0L)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();

    if (me->state () & ControlButton) {
      Coord oldp = line->getPoint (last > 0 ? last - 1 : 0);
      if (fabs (xpos - oldp.x ()) > fabs (ypos - oldp.y ()))
        ypos = oldp.y ();
      else
        xpos = oldp.x ();
    }

    canvas->snapPositionToGrid (xpos, ypos);

    line->setPoint (last, Coord (xpos, ypos));
  }
  else if (e->type () == QEvent::MouseButtonRelease) {
    if (line == 0L)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    if (me->state () & ControlButton) {
      Coord oldp = line->getPoint (last > 0 ? last - 1 : 0);
      if (fabs (xpos - oldp.x ()) > fabs (ypos - oldp.y ()))
        ypos = oldp.y ();
      else
        xpos = oldp.x ();
    }
    canvas->snapPositionToGrid (xpos, ypos);

    line->setPoint (last, Coord (xpos, ypos));

    if (! newObj)
      points.append (new Coord (xpos, ypos));

    if (me->button () == RightButton) {
      doc->unselectAllObjects ();

      if ((last > 0 && line->numOfPoints () >= 3 &&
          line->getNeighbourPoint (Coord (xpos, ypos)) == 0) ||
          ((me->state () & ShiftButton) && line->numOfPoints () > 3)) {
        if (me->state () & ShiftButton) {
          line->removePoint (line->numOfPoints () - 1, false);
          // the polyline is closed, so convert it into a polygon
          GPolygon* obj = new GPolygon (line->getPoints ());
          doc->deleteObject (line);
          if (obj->isValid ()) {
            doc->insertObject (obj);
            doc->setLastObject (obj);
            CreatePolygonCmd *cmd = new CreatePolygonCmd (doc, obj);
            history->addCommand (cmd);
          }
        }
      }
      else {
        doc->setLastObject (line);
#if defined(not_yet)
        // XXXX
        // look for existing polylines with a point near the mouse pointer
        QList<GObject> olist;
        GPolyline *obj = 0L;
        if (doc->findContainingObjects (xpos, ypos, olist)) {
          QListIterator<GObject> it (olist);
          while (it.current ()) {
            if (it.current () != line && it.current ()->isA ("GPolyline")) {
              obj = (GPolyline *) it.current ();
              break;
            }
            ++it;
          }
        }
        int olast = -1;
        if (obj && (olast = obj->getNeighbourPoint (Coord (xpos, ypos))) != -1
            && (olast == 0 || olast == (int) obj->numOfPoints () - 1)) {
          // combine the current line with obj
            //cout << "COMBINE" << endl;
        }
#endif
        if (newObj) {
          if (! line->isValid ())
            doc->deleteObject (line);
          else {
            CreatePolylineCmd *cmd = new CreatePolylineCmd (doc, line);
            history->addCommand (cmd);
          }
        }
        else {
          // remove the first point: it's equal to the last point
          // of the original line
          points.removeFirst ();
          if (last != 0)
            last = last - points.count () + 1;
          AddLineSegmentCmd *cmd =
            new AddLineSegmentCmd (doc, line, last, points);
          history->addCommand (cmd);
        }
      }
      line = 0L; last = 0;
    }
  }
  return;
}

void PolylineTool::activate (GDocument* , Canvas*) {
  emit modeSelected (i18n ("Create Polyline"));
}

void PolylineTool::deactivate (GDocument*, Canvas*) {
  line = 0L;
  last = 0;
}

#include <PolylineTool.moc>
