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

#include <ToolController.h>

InsertPartTool::InsertPartTool (CommandHistory* history) : Tool (history)
{
  validEntry = false;
  m_id=ToolInsertPart;
}

void InsertPartTool::activate (GDocument *_doc, Canvas *_canvas)
{
   state = S_Init;
   doc = _doc;
   canvas = _canvas;
   canvas->setCursor(Qt::arrowCursor);
   m_toolController->emitModeSelected(m_id,i18n("Insert KOffice parts"));
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

void InsertPartTool::processEvent (QEvent* e, GDocument */*_doc*/, Canvas* /*_canvas*/)
{
  if(!doc->document()->isReadWrite())
    return;
  if (e->type () == QEvent::MouseButtonRelease)
   {
    processButtonReleaseEvent((QMouseEvent *) e);
    m_toolController->emitOperationDone (m_id);
   }
  else
   if (e->type () == QEvent::MouseButtonPress)
   {
     processButtonPressEvent((QMouseEvent *) e);
     m_toolController->emitOperationDone (m_id);
   }
   else
   if (e->type () == QEvent::MouseMove)
   {
      processMouseMoveEvent((QMouseEvent *) e);
      m_toolController->emitOperationDone (m_id);
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
    QPen pen(Qt::red, 1, Qt::DotLine);
    painter.begin(canvas);
    painter.setPen(pen);
    painter.translate(canvas->relativePaperArea().left(), canvas->relativePaperArea().top());
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

void InsertPartTool::processButtonReleaseEvent (QMouseEvent* /*e*/)
 {
  if (state == S_Rubberband)
   {
    if (validEntry) {
       KIllustratorDocument *kdoc = doc->document();
       if(selPoint[0].x() > selPoint[1].x())
        {
	 float s = selPoint[0].x();
	 selPoint[0].x(selPoint[1].x());
	 selPoint[1].x(s);
	}
       if(selPoint[0].y() > selPoint[1].y())
        {
	 float s = selPoint[0].y();
	 selPoint[0].y(selPoint[1].y());
	 selPoint[1].y(s);
	}
       kdoc->insertPart (QRect (static_cast<int>(selPoint[0].x()), static_cast<int>(selPoint[0].y()),
                                static_cast<int>(selPoint[1].x() - selPoint[0].x()),
                                static_cast<int>(selPoint[1].y() - selPoint[0].y())), docEntry);
    }
    canvas->repaint ();
    m_toolController->emitOperationDone (m_id);
    m_toolController->toolSelected( Tool::ToolSelect);
    state = S_Init;
    return;
   }
  if(state == S_Init)
   {
    return;
   }
 }
