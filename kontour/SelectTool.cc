/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#include "SelectTool.h"

#include <qpainter.h>

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>

#include <stdlib.h>  //for abs()

#include "kontour_view.h"
#include "kontour_doc.h"
#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"
#include "Canvas.h"
#include "ToolController.h"
#include "TranslateCmd.h"

SelectTool::SelectTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *select = new ToolSelectAction(actionCollection(), "ToolAction");
  KAction *mT1 = new KAction(i18n("Select"), "selecttool", 0, actionCollection());
  select->insert(mT1);
}
  
void SelectTool::activate()
{
  ctype = C_Arrow;
}

void SelectTool::deactivate()
{
}

void SelectTool::processEvent(QEvent *e)
{
  KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
  GPage *page = toolController()->view()->activeDocument()->activePage();
  Canvas *canvas = toolController()->view()->canvas();
  
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

void SelectTool::processButtonPressEvent(QMouseEvent *e, GPage *page, Canvas *canvas)
{
  int xpos = e->x();
  int ypos = e->y();
  bool shiftFlag = e->state() & Qt::ShiftButton;
  p1.setX(xpos);
  p1.setY(ypos);
  //TODO * zoom factor....
  double x = xpos - canvas->xOffset();
  double y = ypos - canvas->yOffset();
  if(state == S_Init)
  {
    mHL = page->document()->indexOfHorizHelpline(y);
    if(mHL != -1)
    {
      state = S_DragHorizHelpline;
      prevcoord = ypos;
      return;
    }
    mHL = page->document()->indexOfVertHelpline(x);
    if(mHL != -1)
    {
      state = S_DragVertHelpline;
      prevcoord = xpos;
      return;
    }
    GObject *obj = page->findContainingObject(x, y);
    if(obj)
    {
      /* an object will be selected */
      state = S_Pick;
      if(!shiftFlag)
        page->unselectAllObjects();
      /* add the object to the selection */
      page->selectObject(obj);
//      origbox = page->boundingBoxForSelection();
    }
    else
    {
      /* no object */
      state = S_Rubberband;
      page->unselectAllObjects();
    }
  }
  else if(state == S_Pick)
  {
    QRect r = canvas->onCanvas(page->boundingBoxForSelection());
    if(r.contains(xpos, ypos))
    {
      state = S_Translate;
      if(ctype != C_Move)
      {
        canvas->setCursor(Qt::SizeAllCursor);
        ctype = C_Move;
      } 
    }
    else
    {
      state = S_Rubberband;
      page->unselectAllObjects();
    }
  }
}

void SelectTool::processMouseMoveEvent(QMouseEvent *e, GPage *page, Canvas *canvas)
{
  int xpos = e->x();
  int ypos = e->y();
  double x = e->x() - canvas->xOffset();
  double y = e->y() - canvas->yOffset();
  if(state == S_Init)
  {
    int mHL = page->document()->indexOfHorizHelpline(y);
    if(mHL != -1)
    {
      if(ctype != C_Vert)
      {
        canvas->setCursor(Qt::sizeVerCursor);
        ctype = C_Vert;
      }
      return;
    }
    mHL = page->document()->indexOfVertHelpline(x);
    if(mHL != -1)
    {
      if(ctype != C_Horiz)
      {
        canvas->setCursor(Qt::sizeHorCursor);
        ctype = C_Horiz;
      }
      return;
    }
    if(ctype != C_Arrow)
    {
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
    }
  }
  else if(state == S_Pick)
  {
    if(e->state() & Qt::LeftButton)
    {
      QRect r = canvas->onCanvas(page->boundingBoxForSelection());
      if(r.contains(xpos, ypos))
      {
        state = S_Translate;
        if(ctype != C_Move)
        {
          canvas->setCursor(Qt::SizeAllCursor);
          ctype = C_Move;
        } 
      }
    }
  }
  else if(state == S_DragHorizHelpline)
  {
    page->document()->updateHorizHelpline(mHL, y);
    canvas->updateBuf(QRect(0, prevcoord, canvas->width(), 1));
    canvas->repaint(0, prevcoord, canvas->width(), 1);
    canvas->updateBuf(QRect(0, ypos, canvas->width(), 1));
    canvas->repaint(0, ypos, canvas->width(), 1);
    prevcoord = ypos;
  }
  else if(state == S_DragVertHelpline)
  {
    page->document()->updateVertHelpline(mHL, x);
    canvas->updateBuf(QRect(prevcoord, 0, 1, canvas->height()));
    canvas->repaint(prevcoord, 0, 1, canvas->height());
    canvas->updateBuf(QRect(xpos, 0, 1, canvas->height()));
    canvas->repaint(xpos, 0, 1, canvas->height());
    prevcoord = xpos;
  }
  else if(state == S_Rubberband)
  {
    canvas->repaint(r);
    if(p1.x() <= e->x())
    {
      r.setLeft(p1.x());
      r.setRight(e->x());
    }
    else
    {
      r.setLeft(e->x());
      r.setRight(p1.x());
    }
    if(p1.y() <= e->y())
    {
      r.setTop(p1.y());
      r.setBottom(e->y());
    }
    else
    {
      r.setTop(e->y());
      r.setBottom(p1.y());
    }
    QPainter p(canvas);
    p.setPen(QPen(blue, 1, Qt::DotLine));
    p.drawRect(r);
  }
  else if(state == S_Translate)
  {
    if(e->state() & Qt::ControlButton)
    {
      if(abs(xpos - p1.x()) > abs(ypos - p1.y()))
        ypos = p1.y();
      else
        xpos = p1.x();
    }
    translate(page, xpos - p1.x(), ypos - p1.y(), true);
  }
}

void SelectTool::processButtonReleaseEvent(QMouseEvent *e, GPage *page, Canvas *canvas)
{
  int xpos = e->x();
  int ypos = e->y();
  if(state == S_Rubberband)
  {
    QPtrList<GObject> olist;
    KoRect selRect(KoPoint(r.left() - canvas->xOffset(), r.top() - canvas->yOffset()), KoPoint(r.right() - canvas->xOffset(), r.bottom() - canvas->yOffset()));
    if(page->findObjectsContainedIn(selRect.normalize(), olist))
    {
      QPtrListIterator<GObject> it(olist);
      for(; it.current(); ++it)
        page->selectObject(it.current());
//      state = S_Pick;
      state = S_Init;
      canvas->repaint(r);
    }
    else
    {
      /* no object found - repaint canvas to remove the rubberband */
	  canvas->updateBuf();
      canvas->repaint();
      state = S_Init;
    }
  }
  else if(state == S_DragHorizHelpline)
    state = S_Init;
  else if(state == S_DragVertHelpline)
    state = S_Init;
  else if(state == S_Translate)
  {
    state = S_Pick;
    if(e->state() & Qt::ControlButton)
    {
      if(abs(xpos - p1.x()) > abs(ypos - p1.y()))
        ypos = p1.y();
      else
        xpos = p1.x();
    }
    translate(page, xpos - p1.x(), ypos - p1.y(), true, true);
    canvas->setCursor(Qt::arrowCursor);
    ctype = C_Arrow;
  }
}

void SelectTool::processKeyPressEvent(QKeyEvent *e, GPage *page, Canvas *canvas)
{
  kdDebug(38000) << "SelectTool::processKeyPressEvent()" << endl;
  if(page->selectionIsEmpty())
    return;

  if(e->key() == Qt::Key_Escape)
  {
    /* clear selection */
    page->unselectAllObjects ();
    return;
  }

  double big_step = 10.0;
  double small_step = 2.0;
/*  float dx = 0, dy = 0;
  bool shift = ke->state () & Qt::ShiftButton;

  switch (ke->key ()) {
  case Qt::Key_Left:
    dx = (shift ? -small_step : -big_step);
    break;
  case Qt::Key_Right:
    dx = (shift ? small_step : big_step);
    break;
  case Qt::Key_Up:
    dy = (shift ? -small_step : -big_step);
    break;
  case Qt::Key_Down:
    dy = (shift ? small_step : big_step);
    break;
  case Qt::Key_Tab:
      kdDebug(38000) << "<tab>" << endl;
  default:
    break;
  }
  if (dx != 0 || dy != 0)
    translate (doc, canvas, dx, dy, false, true); */
}

void SelectTool::translate(GPage *page, double dx, double dy, bool snap, bool permanent)
{
/*  if (snap) {
    const Rect& obox = origbox;
    Rect newbox = canvas->snapTranslatedBoxToGrid (obox.translate (dx, dy));
    if (! (newbox == obox)) {
      dx = newbox.x () - obox.x ();
      dy = newbox.y () - obox.y ();
    }
  }*/
  if(dx == 0 && dy == 0)
    return;
//  kdDebug(38000) << "DX=" << dx << " DY=" << dy << endl;
  if(permanent)
  {
    QListIterator<GObject> it(page->getSelection());
    for(; it.current(); ++it)
      (*it)->setWorkInProgress(false);
    KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
    TranslateCmd *cmd = new TranslateCmd(page->document(), dx, dy);
    doc->history()->addCommand(cmd);
  }
  else
  {
    QListIterator<GObject> it(page->getSelection());
    QWMatrix m;
    m.translate(dx, dy);
    for(; it.current(); ++it)
    {
      (*it)->setWorkInProgress(true);
      (*it)->initTmpMatrix();
      (*it)->ttransform(m, true);
    }
    page->updateSelection();
  }
  
  MeasurementUnit unit = toolController()->view()->unit();
  QString u = unitToString(unit);
  double xval, yval;
  xval = cvtPtToUnit(unit, dx);
  yval = cvtPtToUnit(unit, dy);

  QString msgbuf = i18n("Translate");
  msgbuf += " [";
  msgbuf += QString::number(xval, 'f', 3);
  msgbuf += QString(" ") + u + QString(", ");
  msgbuf += QString::number(yval, 'f', 3);
  msgbuf += QString(" ") + u + QString("]");
  toolController()->view()->setStatus(msgbuf);
}

#include "SelectTool.moc"
