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

#include <TextTool.h>

#include <qkeycode.h>
#include <klocale.h>

#include "KIllustrator_doc.h"
#include "GText.h"
#include "GDocument.h"
#include "GPage.h"
#include <Canvas.h>
#include <Coord.h>
#include <CreateTextCmd.h>
#include <SetTextCmd.h>
#include <CommandHistory.h>
#include "ToolController.h"

TextTool::TextTool (CommandHistory *history) : Tool (history)
{
  text = 0L;
  origState = 0L;
  m_id=ToolText;
}

void TextTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas)
{
  if(!doc->document()->isReadWrite())
      return;
   if (e->type () == QEvent::MouseButtonPress)
   {
      QMouseEvent *me = (QMouseEvent *) e;
      Coord pos (me->x (), me->y ());

      if (text != 0L)
      {
         if (text->isEmpty ())
         {
            // an empty text was entered -> remove the object
            doc->activePage()->deleteObject (text);
         }
         else
                text->showCursor (false);
        }

        text = 0L;

        QList<GObject> olist;
        if (doc->activePage()->findContainingObjects (me->x (), me->y (), olist)) {
            QListIterator<GObject> it (olist);
            while (it.current ()) {
                if (it.current ()->isA ("GText")) {
                    text = (GText *) it.current ();

                    if (origState)
                        origState->unref ();
                    origState = text->saveState ();

                    text->updateCursor (pos);
                    text->showCursor (true);
                    break;
                }
                ++it;
            }
        }
        if (text == 0) {
            text = new GText (doc);
            float xpos = me->x (), ypos = me->y ();
            canvas->snapPositionToGrid (xpos, ypos);

            if (origState) {
                origState->unref ();
                origState = 0L;
            }

            text->setOrigin (Coord (xpos, ypos));
            text->showCursor (true);
            doc->activePage()->insertObject (text);
        }
    }
    else if (e->type () == QEvent::KeyPress) {
        QKeyEvent *ke = (QKeyEvent *) e;
        if (ke->key () == Qt::Key_Escape) {
            // Cancel editing
            if (text != 0L) {
                if (origState == 0L) {
                    // new text -> remove it
                    doc->activePage()->deleteObject (text);
                }
                else {
                    // undo modifications
                    text->restoreState (origState);
                }
            }
            m_toolController->emitOperationDone (m_id);
        }
        if (text == 0L)
            return;
        int x = text->cursorX (), y = text->cursorY ();
        bool changed = false;
        if (ke->key () == Qt::Key_Left) {
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
        else if (ke->key () == Qt::Key_Right) {
            if (x < (int) text->line (y).length ()) {
                x++;
                changed = true;
            }
            else if (y < text->lines () - 1) {
                y++; x = 0;
                changed = true;
            }
        }
        else if (ke->key () == Qt::Key_Up) {
            if (y > 0) {
                y--;
                if (x >= (int) text->line (y).length ())
                    x = text->line (y).length ();
                changed = true;
            }
        }
        else if (ke->key () == Qt::Key_Down) {
            if (y < text->lines () - 1) {
                y++;
                if (x >= (int) text->line (y).length ())
                    x = text->line (y).length ();
                changed = true;
            }
        }
        else if (ke->key () == Qt::Key_Home) {
            x = 0;
            changed = true;
        }
        else if (ke->key () == Qt::Key_End) {
            x = text->line (y).length ();
            changed = true;
        }
        else if (ke->key()==Qt::Key_Return)
            text->insertChar("\n");
        else if (ke->key()==Qt::Key_Backspace)
            text->deleteBackward ();
        else if(ke->key() == Qt::Key_Delete)
            text->deleteChar ();
        else if(!ke->text().isEmpty())
            text->insertChar(ke->text());
        if(changed) {
            text->setCursor (x, y);
        }
    }
    return;
}

void TextTool::activate (GDocument* /*doc*/, Canvas* canvas)
{
   canvas->setCursor(Qt::ibeamCursor);
   m_toolController->emitModeSelected (m_id,i18n("Write some prosa..."));
}

void TextTool::deactivate (GDocument *doc, Canvas*) {
  if (text) {
    text->showCursor (false);
    doc->activePage()->unselectAllObjects ();
    doc->activePage()->setLastObject (text);
    if (origState == 0L) {
      if (text->isEmpty ()) {
        // an empty text was entered -> remove the object
        doc->activePage()->deleteObject (text);
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

