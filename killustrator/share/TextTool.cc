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
#include "TextTool.h"
#include "TextTool.moc"
#include "GDocument.h"
#include "Canvas.h"
#include "Coord.h"
#include "CreateTextCmd.h"
#include "SetTextCmd.h"
#include "CommandHistory.h"
#include <qkeycode.h>

TextTool::TextTool (CommandHistory *history) : Tool (history) {
  text = NULL;
  origState = 0L;
}

void TextTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
  if (e->type () == Event_MouseButtonPress) {
    QMouseEvent *me = (QMouseEvent *) e;
    Coord pos (me->x (), me->y ());

    if (text != 0L) {
      if (text->isEmpty ()) {
	// an empty text was entered -> remove the object
	doc->deleteObject (text);
      }
      else
	text->showCursor (false);
    }

    text = 0L;

    QList<GObject> olist;
    if (doc->findContainingObjects (me->x (), me->y (), olist)) {
      QListIterator<GObject> it (olist);
      while (it.current ()) {
	if (it.current ()->isA ("GText")) {
	  text = (GText *) it.current ();

	  if (origState)
	    origState->unref ();
	  origState = text->saveState ();

	  text->showCursor (true);
	  text->updateCursor (pos);
	  break;
	}
	++it;
      }
    }
    if (text == 0) {
      text = new GText ();
      int xpos = me->x (), ypos = me->y ();
      canvas->snapPositionToGrid (xpos, ypos);

      if (origState) {
	origState->unref ();
	origState = 0L;
      }

      text->setOrigin (Coord (xpos, ypos));
      text->showCursor (true);
      doc->insertObject (text);
    }
  }
  else if (e->type () == Event_KeyPress) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Key_Escape) {
      // Cancel editing
      if (text != 0L) {
	if (origState == 0L) {
	  // new text -> remove it
	  doc->deleteObject (text);
	}
	else {
	  // undo modifications
	  text->restoreState (origState);
	}
      }
      emit operationDone ();
    }
    if (text == NULL)
      return; 
    int x = text->cursorX (), y = text->cursorY ();
    bool changed = false;
    if (ke->key () == Key_Left) {
      if (x > 0) {
	x--;
	changed = true;
      }
      else if (y > 0) {
	y--;
	x = text->line (y).length ();
	changed = true;
      }
    }
    else if (ke->key () == Key_Right) {
      if (x < (int) text->line (y).length ()) {
	x++;
	changed = true;
      }
      else if (y < text->lines () - 1) {
	y++; x = 0;
	changed = true;
      }
    }
    else if (ke->key () == Key_Up) {
      if (y > 0) {
	y--;
	changed = true;
      }
    }
    else if (ke->key () == Key_Down) {
      if (y < text->lines () - 1) {
	y++;
	if (x >= (int) text->line (y).length ())
	  x = text->line (y).length ();
	changed = true;
      }
    }
    else if (ke->ascii ()) {
      if (ke->ascii () == 13)
	text->insertChar ('\n');
      else if (ke->ascii () == 8) {
	// backspace
	text->deleteBackward ();
      }
      else if (ke->ascii () == 127) {
	// delete
	text->deleteChar ();
      }
      else
	text->insertChar (ke->ascii ());
    }
    if (changed) {
      text->setCursor (x, y);
    }
  }
  return;
}

void TextTool::activate (GDocument* doc, Canvas* canvas) {
  emit modeSelected ("");
}

void TextTool::deactivate (GDocument *doc, Canvas*) {
  if (text) {
    text->showCursor (false);
    doc->unselectAllObjects ();
    doc->setLastObject (text);
    if (origState == 0L) {
      if (text->isEmpty ()) {
	// an empty text was entered -> remove the object
	doc->deleteObject (text);
      }
      else {
	CreateTextCmd *cmd = new CreateTextCmd (doc, text);
	history->addCommand (cmd);
      }
    }
    else {
      SetTextCmd *cmd = new SetTextCmd (doc, text, origState);
      history->addCommand (cmd);
    }
    text = 0L;
  }
}
