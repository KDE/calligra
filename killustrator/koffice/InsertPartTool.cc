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

#include <InsertPartTool.h>
#include <kdebug.h>
#include <GDocument.h>
#include <KIllustrator_doc.h>
#include <Canvas.h>
#include <Coord.h>
#include <CommandHistory.h>
#include <qkeycode.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <klocale.h>

InsertPartTool::InsertPartTool (CommandHistory* history) : Tool (history)
 {
  validEntry = false;
 }

void InsertPartTool::activate (GDocument *_doc, Canvas *_canvas)
 {
  state = S_Init;
  doc = _doc;
  canvas = _canvas;
 }

void InsertPartTool::deactivate (GDocument*, Canvas* )
 {
  validEntry = false;
 }

void InsertPartTool::setPartEntry (KoDocumentEntry& entry)
 {
  docEntry = entry;
  validEntry = true;
 }

void InsertPartTool::processEvent (QEvent* e, GDocument *_doc, Canvas* _canvas)
 {
  if (e->type () == QEvent::MouseButtonRelease)
   {
    processButtonReleaseEvent((QMouseEvent *) e);
    emit operationDone ();
   }
  else
   if (e->type () == QEvent::MouseButtonPress)
    {
     processButtonPressEvent((QMouseEvent *) e);
     emit operationDone ();
    }
   else
    if (e->type () == QEvent::MouseMove)
     {
      processMouseMoveEvent((QMouseEvent *) e);
      emit operationDone ();
     }
 }

void InsertPartTool::processButtonPressEvent (QMouseEvent* e)
 {
  /************
   * S_Init
   */
  if (state == S_Init)
   {
    state = S_Rubberband;
    selPoint[0].x(e->x());
    selPoint[0].y(e->y());
    selPoint[1].x(e->x());
    selPoint[1].y(e->y());
   }
 }
 
void InsertPartTool::processMouseMoveEvent (QMouseEvent* e)
 {
  if (state == S_Rubberband)
   {
    selPoint[1].x(e->x());
    selPoint[1].y(e->y());
    canvas->repaint();
    QPainter painter;
    painter.save();
    QPen pen(red, 1, DotLine);
    painter.begin(canvas);
    painter.setPen(pen);
    float sfactor = canvas->scaleFactor();
    painter.scale(sfactor, sfactor);
    Rect selRect(selPoint[0], selPoint[1]);
    painter.drawRect((int) selRect.x (), (int) selRect.y (),
                      (int) selRect.width (), (int) selRect.height ());
    painter.restore();
    painter.end();
    return;
   }
 }
 
void InsertPartTool::processButtonReleaseEvent (QMouseEvent* e)
 {
  if (state == S_Rubberband)
   {
    if (validEntry) {
       KIllustratorDocument *kdoc = doc->document();
       if(selPoint[0].x() > selPoint[1].x())
        {
	 int s = selPoint[0].x();
	 selPoint[0].x(selPoint[1].x());
	 selPoint[1].x(s);
	}
       if(selPoint[0].y() > selPoint[1].y())
        {
	 int s = selPoint[0].y();
	 selPoint[0].y(selPoint[1].y());
	 selPoint[1].y(s);
	}
       kdoc->insertPart (QRect (selPoint[0].x(), selPoint[0].y(), selPoint[1].x() - selPoint[0].x(), selPoint[1].y() - selPoint[0].y()), docEntry);
    }
    canvas->repaint ();
    emit operationDone ();
    state = S_Init;
    return;
   }
  if(state == S_Init)
   {
    return;
   }
 }

#include <InsertPartTool.moc>
