/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <FreeHandTool.h>

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

FreeHandTool::FreeHandTool (CommandHistory* history)
   :Tool (history)
{
   line = 0L;
   last = 0;
   newObj = true;
   buttonIsDown = false;
   points.setAutoDelete (true);
   m_id=ToolFreeHand;
}

void FreeHandTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == QEvent::MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    if (me->button () != LeftButton)
      return;

    buttonIsDown = true;

    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    if (line != 0L) {
      // continue creation: add a new segment
      if (last != 0)
        last++;
    }
    else {
      newObj = true;

      QList<GObject> olist;
      // look for existing polylines with a point near the mouse pointer
      if (doc->findContainingObjects (xpos, ypos, olist)) {
        QListIterator<GObject> it (olist);
        while (it.current ()) {
          if (it.current ()->isA ("GPolyline")) {
            GPolyline* obj = (GPolyline *) it.current ();
            if ((last =
                 obj->getNeighbourPoint (Coord (xpos, ypos))) != -1
                && (last == 0 || last == (int) obj->numOfPoints () - 1)) {
              line = obj;
              newObj = false;
              if (last != 0)
                // it's not the first point of the line, so update the
                // index
                last += 1;
              points.clear ();
              break;
            }
          }
          ++it;
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
//    line->addPoint (last, Coord (xpos, ypos));
  }
  else if (e->type () == QEvent::MouseMove) {
    if (line == 0L || !buttonIsDown)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    Coord np (xpos, ypos);
    Coord oldp = line->getPoint (last > 0 ? last - 1 : 0);
    if (np != oldp) {
      line->addPoint (last, np);
      if (last != 0)
        last++;
      if (! newObj)
        points.append (new Coord (xpos, ypos));
    }
  }
  else if (e->type () == QEvent::MouseButtonRelease) {
    buttonIsDown = false;
    if (line == 0L)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    Coord np (xpos, ypos);
    Coord oldp = line->getPoint (last > 0 ? last - 1 : 0);
    if (np != oldp) {
      line->addPoint (last, np);
      if (last != 0)
        last++;
      if (! newObj)
        points.append (new Coord (xpos, ypos));
    }

    doc->unselectAllObjects ();

    if (last > 0 && line->numOfPoints () >= 3 &&
          line->getNeighbourPoint (Coord (xpos, ypos)) == 0) {
        // the polyline is closed, so convert it into a polygon
        GPolygon* obj = new GPolygon (line->getPoints ());
        doc->deleteObject (line);
        doc->insertObject (obj);
        doc->setLastObject (obj);
        CreatePolygonCmd *cmd = new CreatePolygonCmd (doc, obj);
        history->addCommand (cmd);
    }
    else {
        doc->setLastObject (line);
        if (newObj) {
          CreatePolylineCmd *cmd = new CreatePolylineCmd (doc, line);
          history->addCommand (cmd);
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
  else if (e->type () == QEvent::KeyPress) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Qt::Key_Escape)
      emit operationDone ();
  }
  return;
}

void FreeHandTool::activate (GDocument* /*doc*/, Canvas*) {
  buttonIsDown = false;
  emit modeSelected (i18n ("Create FreeHand Line"));
}

void FreeHandTool::deactivate (GDocument*, Canvas*) {
  line = 0L;
  last = 0;
  buttonIsDown = false;
}

#include <FreeHandTool.moc>
