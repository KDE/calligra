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
#include "EditPointTool.h"
#include "EditPointTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "CommandHistory.h"
#include "EditPointCmd.h"
#include <qkeycode.h>
#include <qbitmap.h>
#include <kapp.h>
#include <klocale.h>

#define right_ptr_width 16
#define right_ptr_height 16
#define right_ptr_x_hot 12
#define right_ptr_y_hot 1
static unsigned char right_ptr_bits[] = {
   0x00, 0x00, 0x00, 0x10, 0x00, 0x18, 0x00, 0x1c, 0x00, 0x1e, 0x00, 0x1f,
   0x80, 0x1f, 0xc0, 0x1f, 0xe0, 0x1f, 0xf0, 0x1f, 0xf8, 0x1e, 0x1c, 0x1c,
   0x00, 0x1c, 0x00, 0x18, 0x00, 0x18, 0x00, 0x10};

#define right_ptrmsk_width 16
#define right_ptrmsk_height 16
static unsigned char right_ptrmsk_bits[] = {
   0x00, 0x30, 0x00, 0x38, 0x00, 0x3c, 0x00, 0x3e, 0x00, 0x3f, 0x80, 0x3f,
   0xc0, 0x3f, 0xe0, 0x3f, 0xf0, 0x3f, 0xf8, 0x3f, 0xfc, 0x3f, 0x7e, 0x3e,
   0x0e, 0x3c, 0x00, 0x38, 0x00, 0x38, 0x00, 0x30};

EditPointTool::EditPointTool (CommandHistory* history) : Tool (history) {
  obj = 0L;
  pointIdx = -1;

  cursor = new QCursor (QBitmap (right_ptr_width,
				 right_ptr_height,
				 right_ptr_bits, true), 
			QBitmap (right_ptrmsk_width,
				 right_ptrmsk_height,
				 right_ptrmsk_bits, true),
			right_ptr_x_hot, right_ptr_y_hot);
}

EditPointTool::~EditPointTool () {
  delete cursor;
}

void EditPointTool::processEvent (QEvent* e, GDocument *doc, 
				  Canvas* canvas) {
  if (doc->selectionIsEmpty ())
    return;

  if (e->type () == Event_MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    obj = 0L;

#ifdef NO_LAYERS
    QListIterator<GObject> it (doc->getSelection ());
    for (; it.current (); ++it) {
      GObject* o = it.current ();
      int idx = o->getNeighbourPoint (Coord (xpos, ypos));
      if (idx != -1) {
	obj = o;
	pointIdx = idx;
	startPos = Coord (xpos, ypos);
	lastPos = startPos;
	canvas->setCursor (*cursor);
	break;
      }
    }
#else
    for (list<GObject*>::iterator it = doc->getSelection ().begin ();
	 it != doc->getSelection ().end (); it++) {
      GObject* o = *it;
      int idx = o->getNeighbourPoint (Coord (xpos, ypos));
      if (idx != -1) {
	obj = o;
	pointIdx = idx;
	startPos = Coord (xpos, ypos);
	lastPos = startPos;
	canvas->setCursor (*cursor);
	break;
      }
    }
#endif
  }
  else if (e->type () == Event_MouseMove) {
    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);

    if (obj == 0L) {
      bool isOver = false;
#ifdef NO_LAYERS
      QListIterator<GObject> it (doc->getSelection ());
      for (; it.current (); ++it) {
	GObject* o = it.current ();
	if (o->getNeighbourPoint (Coord (xpos, ypos)) != -1)
	  isOver = true;
      }
#else
      for (list<GObject*>::iterator it = doc->getSelection ().begin ();
	   it != doc->getSelection ().end (); it++) {
	GObject* o = *it;
	if (o->getNeighbourPoint (Coord (xpos, ypos)) != -1)
	  isOver = true;
      }
#endif
      if (isOver)
	canvas->setCursor (*cursor);
      else
	canvas->setCursor (arrowCursor);
    }
    else {
      float dx = xpos - lastPos.x ();
      float dy = ypos - lastPos.y ();
      if (dx != 0 || dy != 0) {
	obj->movePoint (pointIdx, dx, dy);
	lastPos = Coord (xpos, ypos);
      }
    }
  }
  else if (e->type () == Event_MouseButtonRelease) {
    if (obj == 0L)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    int xpos = me->x (), ypos = me->y ();
    canvas->snapPositionToGrid (xpos, ypos);
    float dx = xpos - lastPos.x ();
    float dy = ypos - lastPos.y ();
    if (dx != 0 || dy != 0) 
      obj->movePoint (pointIdx, dx, dy);

    EditPointCmd *cmd = new EditPointCmd (doc, obj, pointIdx,
					  xpos - startPos.x (), 
					  ypos - startPos.y ());
    history->addCommand (cmd);
    canvas->setCursor (arrowCursor);

    obj = 0L;
  }
}

void EditPointTool::activate (GDocument* doc, Canvas* canvas) {
  emit modeSelected (i18n ("Edit Point"));
  if (! doc->selectionIsEmpty ()) {
    doc->handle ().show (false);
    // redraw with highlighted points
    canvas->showBasePoints (true);
  }
}

void EditPointTool::deactivate (GDocument* doc, Canvas* canvas) {
  if (!doc->selectionIsEmpty ()) {
    doc->handle ().show (true);
    // redraw with unhighlighted points
    canvas->showBasePoints (false);
  }
}
