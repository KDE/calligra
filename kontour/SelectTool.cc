/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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
#include <qcursor.h>

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>

#include <stdlib.h>  //for abs()

#include "kontour_global.h"
#include "kontour_view.h"
#include "kontour_doc.h"
#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"
#include "Canvas.h"
#include "ToolController.h"
#include "TranslateCmd.h"
#include "RotateCmd.h"
#include "ScaleCmd.h"
#include "ShearCmd.h"

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
  toolController()->view()->canvas()->setCursor(Qt::arrowCursor);
  GPage *page = toolController()->view()->activeDocument()->activePage();
  page->handle().show(true);
/*  if(page->lastObject())
  {
    if(page->selectionIsEmpty())
      page->selectObject(page->lastObject());
    state = S_Pick;
  }
  else
    state = S_Init;*/

  if(page->selectionIsEmpty())
  {
    toolController()->view()->setStatus(i18n("Selection mode"));
    state = S_Init;
  }
  else
  {
    KoRect box = page->boundingBoxForSelection();
    MeasurementUnit unit = toolController()->view()->unit();
    QString u = unitToString(unit);
    double x = cvtPtToUnit(unit, box.x());
    double y = cvtPtToUnit(unit, box.y());
    double w = cvtPtToUnit(unit, box.width());
    double h = cvtPtToUnit(unit, box.height());
    QString msgbuf;
    if(page->selectionCount() > 1)
    {
      msgbuf = i18n("Multiple Selection");
      msgbuf +=" [";
      msgbuf += QString::number(x, 'f', 3);
      msgbuf += QString(" ") + u + QString(", ");
      msgbuf += QString::number(y, 'f', 3);
      msgbuf += QString(" ") + u + QString(", ");
      msgbuf += QString::number(w, 'f', 3);
      msgbuf += QString(" ") + u + QString(", ");
      msgbuf += QString::number(h, 'f', 3);
      msgbuf += QString(" ") + u + QString("]");
    }
    else
    {
      GObject *sobj = page->getSelection().first();
      msgbuf =sobj->typeName();
      msgbuf += " [";
      msgbuf += QString::number(x, 'f', 3);
      msgbuf += QString(" ") + u + QString(", ");
      msgbuf += QString::number(y, 'f', 3);
      msgbuf += QString(" ") + u + QString(", ");
      msgbuf += QString::number(w, 'f', 3);
      msgbuf += QString(" ") + u + QString(", ");
      msgbuf += QString::number(h, 'f', 3);
      msgbuf += QString(" ") + u + QString("]");
    }
    toolController()->view()->setStatus(msgbuf);
    state = S_Pick;
  }
  page->updateSelection();
}

void SelectTool::deactivate()
{
  toolController()->view()->activeDocument()->activePage()->handle().show(false);
  toolController()->view()->activeDocument()->activePage()->updateSelection();
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
  fp = canvas->point(e->x(), e->y());
  p1.setX(xpos);
  p1.setY(ypos);
  if(state == S_Init || state == S_Pick)
  {
    if(ctype == C_Arrow)
    {
      mHL = page->document()->indexOfHorizHelpline(fp.y());
      if(mHL != -1)
      {
        state = S_DragHorizHelpline;
        prevcoord = ypos;
        return;
      }
      mHL = page->document()->indexOfVertHelpline(fp.x());
      if(mHL != -1)
      {
        state = S_DragVertHelpline;
        prevcoord = xpos;
        return;
      }
      GObject *obj = page->findContainingObject(fp.x(), fp.y());
      if(obj)
      {
        /* an object will be selected */
        state = S_Press;
        canvas->setCursor(Qt::SizeAllCursor);
        ctype = C_Move;
        if(!shiftFlag)
          page->unselectAllObjects();
        /* add the object to the selection */
        page->selectObject(obj);
	page->updateSelection();
      }
      else
      {
        /* no object */
        state = S_Rubberband;
        page->unselectAllObjects();
	page->updateSelection();
	r.setLeft(xpos);
	r.setRight(xpos);
	r.setTop(ypos);
	r.setBottom(ypos);
      }
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
      if(ctype == C_Move)
        state = S_Translate;
      else if(ctype == C_Size)
        state = S_Scale;
      else if(ctype == C_Shear)
        state = S_Shear;
      else if(ctype == C_Rotate)
        state = S_Rotate;
      else if(ctype == C_MoveRotCenter)
        state = S_MoveRotCenter;
      else
      {
        state = S_Rubberband;
        page->unselectAllObjects();
      }
    }
    else
    {
      mask = page->handle().contains(KoPoint(x, y));
      if(mask)
      {
        switch(mask)
        {
        /* Resize */
        case(Kontour::HPosLeft | Kontour::HPosTop):
          if(ctype != C_Size)
	  {
	    ctype = C_Size;
            canvas->setCursor(Qt::sizeFDiagCursor);
	  }
          break;
        case(Kontour::HPosTop):
          if(ctype != C_Size)
	  {
	    ctype = C_Size;
            canvas->setCursor(Qt::sizeVerCursor);
	  }
          break;
        case(Kontour::HPosTop | Kontour::HPosRight):
          if(ctype != C_Size)
	  {
	    ctype = C_Size;
            canvas->setCursor(Qt::sizeBDiagCursor);
	  }
          break;
        case(Kontour::HPosRight):
          if(ctype != C_Size)
	  {
	    ctype = C_Size;
            canvas->setCursor(Qt::sizeHorCursor);
	  }
          break;
        case(Kontour::HPosRight | Kontour::HPosBottom):
          if(ctype != C_Size)
	  {
	    ctype = C_Size;
            canvas->setCursor(Qt::sizeFDiagCursor);
	  }
          break;
        case(Kontour::HPosBottom):
          if(ctype != C_Size)
          {
            ctype = C_Size;
            canvas->setCursor(Qt::sizeVerCursor);
          }
          break;
        case(Kontour::HPosBottom | Kontour::HPosLeft):
          if(ctype != C_Size)
          {
            ctype = C_Size;
            canvas->setCursor(Qt::sizeBDiagCursor);
          }
          break;
        case(Kontour::HPosLeft):
          if(ctype != C_Size)
          {
	    ctype = C_Size;
            canvas->setCursor(Qt::sizeHorCursor);
          }
	  break;
        /* Shear */
        case(Kontour::HPosTopR):
          if(ctype != C_Shear)
	  {
	    ctype = C_Shear;
            canvas->setCursor(Qt::sizeHorCursor);
	  }
          break;
        case(Kontour::HPosRightR):
          if(ctype != C_Shear)
	  {
	    ctype = C_Shear;
            canvas->setCursor(Qt::sizeVerCursor);
	  }
          break;
        case(Kontour::HPosBottomR):
          if(ctype != C_Shear)
          {
            ctype = C_Shear;
            canvas->setCursor(Qt::sizeHorCursor);
          }
          break;
        case(Kontour::HPosLeftR):
          if(ctype != C_Shear)
          {
	    ctype = C_Shear;
            canvas->setCursor(Qt::sizeVerCursor);
          }
	  break;
        /* Rotate */
	case(Kontour::HPosLeftR | Kontour::HPosTopR):
          if(ctype != C_Rotate)
	  {
	    ctype = C_Rotate;
            canvas->setCursor(Qt::sizeBDiagCursor);
	  }
          break;
        case(Kontour::HPosTopR | Kontour::HPosRightR):
          if(ctype != C_Rotate)
	  {
	    ctype = C_Rotate;
            canvas->setCursor(Qt::sizeFDiagCursor);
	  }
          break;
        case(Kontour::HPosRightR | Kontour::HPosBottomR):
          if(ctype != C_Rotate)
	  {
	    ctype = C_Rotate;
            canvas->setCursor(Qt::sizeBDiagCursor);
	  }
          break;
        case(Kontour::HPosBottomR | Kontour::HPosLeftR):
          if(ctype != C_Rotate)
          {
            ctype = C_Rotate;
            canvas->setCursor(Qt::sizeFDiagCursor);
          }
          break;
	/* Rotate center */
	case(Kontour::HPosCenter):
          if(ctype != C_MoveRotCenter)
          {
            ctype = C_MoveRotCenter;
            canvas->setCursor(Qt::SizeAllCursor);
          }
          break;
	}
        return;
      }
      QRect r = canvas->onCanvas(page->boundingBoxForSelection());
      if(r.contains(xpos, ypos))
      {
        if(ctype != C_Move)
        {
          canvas->setCursor(Qt::SizeAllCursor);
          ctype = C_Move;
        }
        return;
      }
      if(ctype != C_Arrow)
      {
        canvas->setCursor(Qt::arrowCursor);
        ctype = C_Arrow;
	return;
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
  else if(state == S_MoveRotCenter)
  {
    page->handle().rotCenter(KoPoint(x, y));
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
  else if(state == S_Scale)
  {
    double xoff = xpos - p1.x();
    double yoff = ypos - p1.y();
/*    if(e->state () & Qt::ControlButton)
    {
      if (fabs (xoff) > fabs (yoff)) {
        yoff = xoff;
        if ((mask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
            (mask & (Handle::HPos_Right | Handle::HPos_Top)))
          yoff = -yoff;
      }
      else {
        xoff = yoff;
        if ((mask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
            (mask & (Handle::HPos_Right | Handle::HPos_Top)))
          xoff = -xoff;
      }
    }*/
    if(mask == (Kontour::HPosLeft | Kontour::HPosBottom) || mask == (Kontour::HPosLeft | Kontour::HPosTop) || mask == (Kontour::HPosRight | Kontour::HPosBottom) || mask == (Kontour::HPosRight | Kontour::HPosTop))
      scale(page, xoff, yoff, true);
    else
      scale(page, xoff, yoff, false);
  }
  else if(state == S_Shear)
  {
    shear(page, x - fp.x(), y - fp.y());
  }
  else if(state == S_Rotate)
  {
    rotate(page, fp.x(), fp.y(), x, y);
  }
}

void SelectTool::processButtonReleaseEvent(QMouseEvent *e, GPage *page, Canvas *canvas)
{
  int xpos = e->x();
  int ypos = e->y();
  double x = e->x() - canvas->xOffset();
  double y = e->y() - canvas->yOffset();
  if(state == S_Rubberband)
  {
    QPtrList<GObject> olist;
    float zoom = toolController()->view()->activeDocument()->zoomFactor();
    KoRect selRect(KoPoint((r.left() - canvas->xOffset()) / zoom, (r.top() - canvas->yOffset()) / zoom),
	               KoPoint((r.right() - canvas->xOffset()) / zoom, (r.bottom() - canvas->yOffset()) / zoom));
    if(page->findObjectsContainedIn(selRect.normalize(), olist))
    {
      QPtrListIterator<GObject> it(olist);
      for(; it.current(); ++it)
        page->selectObject(it.current());
      page->updateSelection();
      canvas->repaint(r);
      state = S_Pick;
    }
    else
    {
      /* no object found - repaint canvas to remove the rubberband */
      canvas->repaint();
      state = S_Init;
    }
  }
  else if(state == S_DragHorizHelpline)
    state = S_Init;
  else if(state == S_DragVertHelpline)
    state = S_Init;
  else if(state == S_MoveRotCenter)
  {
    state = S_Pick;
  }
  else if(state == S_Press)
    state = S_Pick;
  else if(state == S_Pick)
  {
    if(ctype == C_Move)
    {
      /* Node Edit Tool */
      toolController()->selectTool("EditPoint");
    }
  }
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
  }
  else if(state == S_Scale)
  {
    state = S_Pick;
    //    canvas->snapPositionToGrid (xpos, ypos);
    double xoff = xpos - p1.x();
    double yoff = ypos - p1.y();
/*    if(e->state () & Qt::ControlButton)
    {
      if (fabs (xoff) > fabs (yoff)) {
        yoff = xoff;
        if ((mask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
            (mask & (Handle::HPos_Right | Handle::HPos_Top)))
          yoff = -yoff;
      }
      else {
        xoff = yoff;
        if ((mask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
            (mask & (Handle::HPos_Right | Handle::HPos_Top)))
          xoff = -xoff;
      }
    }*/
    if(mask == (Kontour::HPosLeft | Kontour::HPosBottom) || mask == (Kontour::HPosLeft | Kontour::HPosTop) || mask == (Kontour::HPosRight | Kontour::HPosBottom) || mask == (Kontour::HPosRight | Kontour::HPosTop))
      scale(page, xoff, yoff, true, true);
    else
      scale(page, xoff, yoff, false, true);
  }
  else if(state == S_Shear)
  {
    state = S_Pick;
    kdDebug(38000) << "Shear: dx=" << x - fp.x() << " dy=" << y - fp.y() << "mask=" << mask  << endl;
    shear(page, x - fp.x(), y - fp.y(), true);
  }
  else if(state == S_Rotate)
  {
    state = S_Pick;
    rotate(page, fp.x(), fp.y(), x, y, true);
  }
}

void SelectTool::processKeyPressEvent(QKeyEvent *e, GPage *page, Canvas */*canvas*/)
{
  if(!page->selectionIsEmpty())
  {
    if(e->key() == Qt::Key_Escape)
    {
      // clear selection
      page->unselectAllObjects();
      page->updateSelection();
      return;
    }
    double big_step = 10.0;
    double small_step = 2.0;
    double dx = 0;
    double dy = 0;
    bool shift = e->state() & Qt::ShiftButton;
    switch(e->key())
    {
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
    }
    if(dx != 0 || dy != 0)
      translate(page, dx, dy, false, true);
  }

  if(e->key() == Qt::Key_Tab)
  {
    page->selectNextObject();
    page->updateSelection();
  }
}

void SelectTool::translate(GPage *page, double dx, double dy, bool snap, bool permanent)
{
  for(QPtrListIterator<GObject> it(page->getSelection()); it.current(); ++it)
    (*it)->initTmpMatrix();
  page->calcBoxes();
  if(snap)
  {
    KoRect obox = page->shapeBoxForSelection();
    KoRect nbox = toolController()->view()->canvas()->snapTranslatedBoxToGrid(obox.translate(dx, dy));
    dx = nbox.x() - obox.x();
    dy = nbox.y() - obox.y();
  }
  if(dx == 0 && dy == 0)
    return;
  if(permanent)
  {
    QPtrListIterator<GObject> it(page->getSelection());
    for(; it.current(); ++it)
      (*it)->setWorkInProgress(false);
    KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
    TranslateCmd *cmd = new TranslateCmd(page->document(), dx, dy);
    doc->history()->addCommand(cmd);
  }
  else
  {
    QPtrListIterator<GObject> it(page->getSelection());
    QWMatrix m;
    m.translate(dx, dy);
    for(; it.current(); ++it)
    {
      (*it)->setWorkInProgress(true);
      (*it)->ttransform(m);
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

void SelectTool::scale(GPage *page, double dx, double dy, bool type, bool permanent)
{
  double sx;
  double sy;
  for(QPtrListIterator<GObject> it(page->getSelection()); it.current(); ++it)
    (*it)->initTmpMatrix();
  page->calcBoxes();
  KoRect origbox = page->shapeBoxForSelection();
  KoRect newbox = origbox;
  if(mask & Kontour::HPosRight)
    newbox.setRight(newbox.right() + dx);
  if(mask & Kontour::HPosBottom)
    newbox.setBottom(newbox.bottom() + dy);
  if(mask & Kontour::HPosLeft)
    newbox.setLeft(newbox.left() + dx);
  if(mask & Kontour::HPosTop)
    newbox.setTop(newbox.top() + dy);

  newbox = toolController()->view()->canvas()->snapScaledBoxToGrid(newbox, mask);

  sx = newbox.width() / origbox.width();
  sy = newbox.height() / origbox.height();

  if(type)
    sx = sy;

  if(permanent)
  {
    QPtrListIterator<GObject> it(page->getSelection());
    for(; it.current(); ++it)
      (*it)->setWorkInProgress(false);
    KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
    ScaleCmd *cmd = new ScaleCmd(page->document(), mask, sx, sy, origbox);
    doc->history()->addCommand(cmd);
  }
  else
  {
    QWMatrix m1, m2, m3;
    double xoff = origbox.x();
    double yoff = origbox.y();
    double xback = xoff;
    double yback = yoff;
    if(mask & Kontour::HPosLeft)
      xback = origbox.left() + origbox.width() * (1.0 - sx);
    if(mask & Kontour::HPosTop)
      yback = origbox.top() + origbox.height() * (1.0 - sy);
    if(mask & Kontour::HPosCenter)
    {
      xback = origbox.left() + origbox.width() * 0.5 * (1.0 - sx);
      yback = origbox.top() + origbox.height() * 0.5 * (1.0 - sy);
    }

    m1.translate(-xoff, -yoff);
    m2.scale(sx, sy);
    m3.translate(xback, yback);

    for(QPtrListIterator<GObject> it(page->getSelection()); it.current(); ++it)
    {
      (*it)->setWorkInProgress(true);
      (*it)->ttransform(m1 * m2 * m3);
    }
    page->updateSelection();
  }

  QString msgbuf = i18n("Scale");
  msgbuf += " [";
  msgbuf += QString::number(sx * 100.0, 'f', 3);
  msgbuf += QString(" %, ");
  msgbuf += QString::number(sy * 100.0, 'f', 3);
  msgbuf += QString(" %]");
  toolController()->view()->setStatus(msgbuf);
}

void SelectTool::shear(GPage *page, double dx, double dy, bool permanent)
{
  for(QPtrListIterator<GObject> it(page->getSelection()); it.current(); ++it)
    (*it)->initTmpMatrix();
  page->calcBoxes();
  KoRect origbox = page->shapeBoxForSelection();
  KoRect newbox = origbox;
  double sx = 0.0;
  double sy = 0.0;
  if(mask == Kontour::HPosTopR)
    sx = -dx / origbox.width();
  else if(mask == Kontour::HPosBottomR)
    sx = dx / origbox.width();
  else if(mask == Kontour::HPosLeftR)
    sy = -dy / origbox.height();
  else if(mask == Kontour::HPosRightR)
    sy = dy / origbox.height();

  if(permanent)
  {
    QPtrListIterator<GObject> it(page->getSelection());
    for(; it.current(); ++it)
      (*it)->setWorkInProgress(false);
    KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
    ShearCmd *cmd = new ShearCmd(page->document(), page->handle().rotCenter(), sx, sy);
    doc->history()->addCommand(cmd);
  }
  else
  {
    QWMatrix m1, m2, m3;
    m1.translate(-page->handle().rotCenter().x(), -page->handle().rotCenter().y());
    m2.shear(sx, sy);
    m3.translate(page->handle().rotCenter().x(), page->handle().rotCenter().y());
    for(QPtrListIterator<GObject> it(page->getSelection()); it.current(); ++it)
    {
      (*it)->setWorkInProgress(true);
      (*it)->ttransform(m1 * m2 * m3);
    }
    page->updateSelection();
  }

  QString msgbuf = i18n("Shear");
  msgbuf += " [";
  msgbuf += QString::number(sx * 100.0, 'f', 3);
  msgbuf += QString(" %, ");
  msgbuf += QString::number(sy * 100.0, 'f', 3);
  msgbuf += QString(" %]");
  toolController()->view()->setStatus(msgbuf);
}

void SelectTool::rotate(GPage *page, double xf, double yf, double xp, double yp, bool permanent)
{
  KoPoint rc = page->handle().rotCenter();
  double angle;
  angle = atan2(yp - rc.y(), xp - rc.x());
  if(xp - rc.x() < 0.0)
    angle += Kontour::pi;
  angle -= atan2(yf - rc.y(), xf - rc.x());
  if(xf - rc.x() < 0.0)
    angle += Kontour::pi;

  angle *= 180.0 / Kontour::pi;
  if(angle < -180.0)
    angle += 360.0;
  if(angle > 180.0)
    angle -= 360.0;

  if(permanent)
  {
    QPtrListIterator<GObject> it(page->getSelection());
    for(; it.current(); ++it)
    (*it)->setWorkInProgress(false);
    KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
    RotateCmd *cmd = new RotateCmd(page->document(), rc, angle);
    doc->history()->addCommand(cmd);
  }
  else
  {
    QWMatrix m1, m2, m3;
    m1.translate(-page->handle().rotCenter().x(), -page->handle().rotCenter().y());
    m2.rotate(angle);
    m3.translate(page->handle().rotCenter().x(), page->handle().rotCenter().y());

    for(QPtrListIterator<GObject> it(page->getSelection()); it.current(); ++it)
    {
      (*it)->setWorkInProgress(true);
      (*it)->initTmpMatrix();
      (*it)->ttransform(m1 * m2 * m3);
    }
    page->updateSelection();
  }
  MeasurementUnit unit = toolController()->view()->unit();
  QString u = unitToString(unit);
  double xval = cvtPtToUnit(unit, rc.x());
  double yval = cvtPtToUnit(unit, rc.y());

  QString msgbuf = i18n("Rotate");
  msgbuf +=" [";
  msgbuf += QString::number(angle, 'f', 0);
  msgbuf += QString(" - ");
  msgbuf += QString::number(xval, 'f', 3);
  msgbuf += QString(" ") + u + QString(", ");
  msgbuf += QString::number(yval, 'f', 3);
  msgbuf += QString(" ") + u + QString("]");
  toolController()->view()->setStatus(msgbuf);
}

#include "SelectTool.moc"
