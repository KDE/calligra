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
#include <qkeycode.h>
#include <kapp.h>
#include <klocale.h>

RectangleTool::RectangleTool (CommandHistory* history) : Tool (history) {
  rect = NULL;
}

void RectangleTool::processEvent (QEvent* e, GDocument *doc, 
				  Canvas* canvas) {
  if (e->type () == Event_MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    bool flag = me->state () & ControlButton;
    rect = new GPolygon (flag ? GPolygon::PK_Square : GPolygon::PK_Rectangle);

    rect->addPoint (0, Coord (xpos, ypos));
    rect->addPoint (1, Coord (xpos, ypos));
    rect->addPoint (2, Coord (xpos, ypos));
    rect->addPoint (3, Coord (xpos, ypos));
    doc->insertObject (rect);
  }
  else if (e->type () == Event_MouseMove) {
    if (rect == NULL)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);
    rect->setEndPoint (Coord (xpos, ypos));
  }
  else if (e->type () == Event_MouseButtonRelease) {
    if (rect == NULL)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    rect->setEndPoint (Coord (xpos, ypos));
    CreateRectangleCmd *cmd = new CreateRectangleCmd (doc, rect);
    history->addCommand (cmd);

    doc->unselectAllObjects ();
    doc->setLastObject (rect);
    rect = NULL;
  }
}

void RectangleTool::activate (GDocument* doc, Canvas* canvas) {
  emit modeSelected (i18n ("Create Rectangle"));
}

