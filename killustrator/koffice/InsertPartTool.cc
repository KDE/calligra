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
#include "InsertPartTool.h"
#include "InsertPartTool.moc"
#include "GDocument.h"
#include "KIllustrator_doc.h"
#include "Canvas.h"
#include "Coord.h"
#include "CommandHistory.h"
#include <qkeycode.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <kapp.h>
#include <klocale.h>

InsertPartTool::InsertPartTool (CommandHistory* history) : Tool (history) {
  x = y = -1;
  validEntry = false;
}

void InsertPartTool::activate (GDocument* doc, Canvas* canvas) {
  x = y = -1;
}

void InsertPartTool::deactivate (GDocument*, Canvas* canvas) {
  validEntry = false;
}

void InsertPartTool::setPartEntry (KoDocumentEntry& entry) {
  docEntry = entry;
  validEntry = true;
}

void InsertPartTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == (QEvent::Type)2 /*QEvent::KeyPress*/) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Qt::Key_Escape) {
      /*
       * Abort the last operation
       */
      emit operationDone ();
    }
  }
  else if (e->type () == QEvent::MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    x = me->x (); y = me->y ();
    width = height = 0;
  }
  else if (e->type () == QEvent::MouseMove) {
    if (x == -1 || y == -1)
      return;

    QMouseEvent *me = (QMouseEvent *) e;
    width = me->x () - x;
    height = me->y () - y;

    canvas->repaint ();
    QPainter painter;
    painter.save ();
    QPen pen (black, 1);
    painter.begin (canvas);
    painter.setPen (pen);
    float sfactor = canvas->scaleFactor ();
    painter.scale (sfactor, sfactor);
    painter.drawRect (x, y, width, height);
    painter.restore ();
    painter.end ();
  }
  else if (e->type () == QEvent::MouseButtonRelease) {
    if (validEntry) {
       KIllustratorDocument *kdoc = (KIllustratorDocument *) doc;
       kdoc->insertPart (QRect (x, y, width, height), docEntry);
    }
    canvas->repaint ();
    emit operationDone ();
  }
}
