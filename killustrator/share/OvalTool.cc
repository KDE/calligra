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
#include "OvalTool.h"
#include "OvalTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "CreateOvalCmd.h"
#include "CommandHistory.h"
#include <qkeycode.h>
#include <kapp.h>
#include <klocale.h>

OvalTool::OvalTool (CommandHistory *history) : Tool (history) {
  oval = NULL;
}

void OvalTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == Event_MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    bool flag = me->state () & ControlButton;
    oval = new GOval (flag);
    oval->setStartPoint (Coord (xpos, ypos));
    oval->setEndPoint (Coord (xpos, ypos));
    doc->insertObject (oval);
  }
  else if (e->type () == Event_MouseMove) {
    if (oval == NULL)
      return; 
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    oval->setEndPoint (Coord (xpos, ypos));
  }
  else if (e->type () == Event_MouseButtonRelease) {
    if (oval == NULL)
      return;
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    oval->setEndPoint (Coord (xpos, ypos));
    doc->unselectAllObjects ();
    doc->setLastObject (oval);

    CreateOvalCmd *cmd = new CreateOvalCmd (doc, oval);
    history->addCommand (cmd);

    oval = NULL;
  }
  return;
}

void OvalTool::activate (GDocument* doc, Canvas* canvas) {
  emit modeSelected (i18n ("Create Oval"));
}

