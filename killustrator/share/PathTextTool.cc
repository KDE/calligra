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
#include "PathTextTool.h"
#include "PathTextTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "GPolyline.h"
#include "GText.h"
#include "CommandHistory.h"
#include <qkeycode.h>
#include <qbitmap.h>
#include <kapp.h>
#include <klocale.h>

#define bigarrow_width 32
#define bigarrow_height 32
#define bigarrow_x_hot 31
#define bigarrow_y_hot 15

static unsigned char bigarrow_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x30, 0x00,
   0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x01,
   0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0xfe, 0xff, 0xff, 0x0f,
   0xfe, 0xff, 0xff, 0x1f, 0xfe, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x7f,
   0xfe, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x3f,
   0xfe, 0xff, 0xff, 0x1f, 0xfe, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xf0, 0x07,
   0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0xf0, 0x00,
   0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x10, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

PathTextTool::PathTextTool (CommandHistory* history) : Tool (history) {
  QBitmap bm (bigarrow_width, bigarrow_height, bigarrow_bits, true);
  cursor = QCursor (bm, bm, bigarrow_x_hot, bigarrow_y_hot);
}

void PathTextTool::activate (GDocument* doc, Canvas* canvas) {
  textObj = 0L;
  oldCursor = canvas->cursor ();

  if (doc->selectionCount () == 1) {
    GObject* obj = doc->getSelection ().front ();
    if (obj->isA ("GText"))
      textObj = (GText *) obj;
  }
  if (textObj)
    canvas->setCursor (cursor);
  else
    emit operationDone ();
}

void PathTextTool::deactivate (GDocument*, Canvas* canvas) {
  canvas->setCursor (oldCursor);
}

void PathTextTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == Event_KeyPress) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Key_Escape) {
      /*
       * Abort the last operation
       */
      textObj = 0L;
    }
  }
  else if (e->type () == Event_MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    if (me->button () == LeftButton) {
      int xpos = me->x (), ypos = me->y ();
      GObject *obj;
      int dummy;

      if ((obj = doc->findContainingObject (xpos, ypos)) != 0L) {
	textObj->setPathObject (obj);
      }
    }
    emit operationDone ();
  }
}
