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

#include <iostream.h>
#include "RectangleTool.h"
#include "RectangleTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "CreateRectangleCmd.h"
#include "CommandHistory.h"
#include "units.h"
#include "PStateManager.h"
#include <qkeycode.h>
#include <kapp.h>
#include <klocale.h>

RectangleTool::RectangleTool (CommandHistory* history) : Tool (history) {
  rect = NULL;
}

void RectangleTool::processEvent (QEvent* e, GDocument *doc,
				  Canvas* canvas) {
  if (e->type () == QEvent::MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    bool flag = me->state () & ControlButton;
    rect = new GPolygon (flag ? GPolygon::PK_Square : GPolygon::PK_Rectangle);

    rect->addPoint (0, Coord (xpos, ypos));
    rect->addPoint (1, Coord (xpos, ypos));
    rect->addPoint (2, Coord (xpos, ypos));
    rect->addPoint (3, Coord (xpos, ypos));
    doc->insertObject (rect);
    emit modeSelected (flag ? i18n ("Create Square") :
		       i18n ("Create Rectangle"));
  }
  else if (e->type () == QEvent::MouseMove) {
    if (rect == NULL)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);
    rect->setEndPoint (Coord (xpos, ypos));
    bool flag = me->state () & ControlButton;

    Rect r = rect->boundingBox ();
    MeasurementUnit unit =
      PStateManager::instance ()->defaultMeasurementUnit ();
    const char *u = unitToString (unit);
    float xval, yval, wval, hval;
    xval = cvtPtToUnit (unit, r.x ());
    yval = cvtPtToUnit (unit, r.y ());
    wval = cvtPtToUnit (unit, r.width ());
    hval = cvtPtToUnit (unit, r.height ());

    sprintf (msgbuf, "%s [%.3f %s, %.3f %s, %.3f %s, %.3f %s]",
	     flag ? i18n ("Create Square").ascii() :
	     i18n ("Create Rectangle").ascii(), xval, u, yval, u, wval, u, hval, u);
    emit modeSelected (msgbuf);
  }
  else if (e->type () == QEvent::MouseButtonRelease) {
    if (rect == NULL)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    float xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    rect->setEndPoint (Coord (xpos, ypos));
    if (! rect->isValid ()) {
      doc->deleteObject (rect);
    }
    else {
      CreateRectangleCmd *cmd = new CreateRectangleCmd (doc, rect);
      history->addCommand (cmd);

      doc->unselectAllObjects ();
      doc->setLastObject (rect);
    }
    rect = NULL;
  }
  else if (e->type () == QEvent::KeyPress) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Qt::Key_Escape)
      emit operationDone ();
  }
}

void RectangleTool::activate (GDocument* doc, Canvas* canvas) {
  emit modeSelected (i18n ("Create Rectangle"));
}

