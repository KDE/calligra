/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#include "EditPointTool.h"

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>

#include "kontour_view.h"
#include "GPage.h"
#include "GObject.h"
#include "Canvas.h"
#include "ToolController.h"
#include "MovePointCmd.h"

EditPointTool::EditPointTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *editpoint = new ToolSelectAction(actionCollection(), "ToolAction");
  KAction *mT1 = new KAction(i18n("Edit Point"), "", 0, actionCollection());
  editpoint->insert(mT1);
  mode = MovePoint;
}

void EditPointTool::activate()
{
  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
  toolController()->view()->canvas()->withBasePoints(true);
  toolController()->view()->activeDocument()->activePage()->updateSelection();
  obj = 0L;
  pointIdx = -1;
}

void EditPointTool::deactivate()
{
  toolController()->view()->canvas()->withBasePoints(false);
}

void EditPointTool::processEvent(QEvent *e)
{
  KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
  GPage *page = toolController()->view()->activeDocument()->activePage();
  Canvas *canvas = toolController()->view()->canvas();

  if(page->selectionIsEmpty())
    return;

  if(!doc->isReadWrite())
    return;
  if(e->type() == QEvent::MouseButtonPress)
    processButtonPressEvent((QMouseEvent *)e, page, canvas);
  else if(e->type() == QEvent::MouseMove)
    processMouseMoveEvent((QMouseEvent *)e, page, canvas);
  else if(e->type() == QEvent::MouseButtonRelease)
    processButtonReleaseEvent((QMouseEvent *)e, page, canvas);
  else if(e->type() == QEvent::KeyPress)
    processKeyPressEvent((QKeyEvent *)e, page, canvas);
}

void EditPointTool::processButtonPressEvent(QMouseEvent *me, GPage *page, Canvas *canvas)
{
  double x = me->x() - canvas->xOffset();
  double y = me->y() - canvas->yOffset();
  //canvas->snapPositionToGrid (xpos, ypos);

  obj = 0L;
  pointIdx = -1;
  // TODO for performance reasons check if an object from the selection
  // has to be edited
  for(QPtrListIterator<GObject>it(page->getSelection()); it.current(); ++it)
  {
    GObject *o = *it;
    int idx = o->getNeighbourPoint(KoPoint(x, y), 3.0);
    if(idx != -1)
    {
      kdDebug() << "INDEX = " << idx << endl;
      obj = o;
      pointIdx = idx;
      mStartPoint.setX(x);
      mStartPoint.setY(y);
      mLastPoint = mStartPoint;
      break;
    }
  }
    // if no currently selected object was found at the mouse position ...
/*    if (obj == 0L)
    {
      if ((obj = doc->activePage()->findContainingObject (qRound (xpos), qRound (ypos))) != 0L)
      {
      // select and edit this object
        doc->activePage()->unselectAllObjects ();
        doc->activePage()->selectObject (obj);
        pointIdx = obj->getNeighbourPoint (Coord (xpos, ypos));
        startPos = Coord (xpos, ypos);
        lastPos = startPos;
        canvas->setCursor (*cursor);
      }
    }*/
}

void EditPointTool::processMouseMoveEvent(QMouseEvent *me, GPage *page, Canvas *canvas)
{
  double x = me->x() - canvas->xOffset();
  double y = me->y() - canvas->yOffset();
//  canvas->snapPositionToGrid(xpos, ypos);
/*  if(obj == 0L)
  {
         bool isOver = false;
         int pidx;

         for (QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it)
         {
            GObject* o = *it;
            if ((pidx = o->getNeighbourPoint (Coord (xpos, ypos))) != -1)
            {
               if (mode == RemovePoint && o->isA ("GBezier")) {
                  if (((GBezier *) o)->isEndPoint (pidx))
                  {
                     isOver = true;
                     break;
                  }
               }
               else
                  isOver = true;
            }
         }
         if (isOver)
            canvas->setCursor (*cursor);
         else
            canvas->setCursor(Qt::crossCursor);
      }
      else */
  if(pointIdx != -1)
  {
    double dx = x - mLastPoint.x();
    double dy = y - mLastPoint.y();
    if(dx != 0 || dy != 0)
      obj->movePoint(pointIdx, dx, dy, me->state() & Qt::ControlButton);
    mLastPoint.setX(x);
    mLastPoint.setY(y);
    page->document()->emitChanged(obj->boundingBox(), true);
  }
}

void EditPointTool::processButtonReleaseEvent(QMouseEvent *me, GPage *page, Canvas *canvas)
{
  double x = me->x() - canvas->xOffset();
  double y = me->y() - canvas->yOffset();
//  canvas->snapPositionToGrid(xpos, ypos);
  if(mode == MovePoint)
  {
    if(pointIdx != -1)
    {
      double dx = mStartPoint.x() - mLastPoint.x();
      double dy = mStartPoint.y() - mLastPoint.y();
      if(dx != 0 || dy != 0)
        obj->movePoint(pointIdx, dx, dy, me->state() & Qt::ControlButton);
      dx = x - mStartPoint.x();
      dy = y - mStartPoint.y();
      if(dx != 0 || dy != 0)
      {
        MovePointCmd *cmd = new MovePointCmd(toolController()->view()->activeDocument(), obj, pointIdx, dx, dy);
        KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
        doc->history()->addCommand(cmd);
      }
      pointIdx = -1;
    }
  }
//  toolController()->selectTool("Select");
}

void EditPointTool::processKeyPressEvent(QKeyEvent *, GPage *, Canvas *)
{
}

#include "EditPointTool.moc"
