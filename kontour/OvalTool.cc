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

#include "OvalTool.h"
#include "kontour_factory.h"
#include <kiconloader.h>
#include <qpainter.h>

#include <klocale.h>
#include <kdebug.h>

#include "kontour_view.h"
#include "kontour_doc.h"
#include "GDocument.h"
#include "GPage.h"
#include "Canvas.h"
#include "ToolController.h"
#include "GOval.h"
#include "CreateOvalCmd.h"

OvalTool::OvalTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction* zoom = new ToolSelectAction(actionCollection(), "ToolAction" );
  KRadioAction *mT1 = new KRadioAction(i18n("Circle"), "ellipse1", 0, actionCollection());
  KRadioAction *mT2 = new KRadioAction(i18n("Circle"), "ellipse2", 0, actionCollection());
  mT1->setExclusiveGroup("OvalTool");
  mT2->setExclusiveGroup("OvalTool");
  zoom->insert(mT1);
  zoom->insert(mT2);
}

void OvalTool::activate()
{
  kdDebug(38000) << "OvalTool::activate()" << endl;
  state = S_Init;
}

void OvalTool::deactivate()
{
  kdDebug(38000) << "OvalTool::deactivate()" << endl;
}

void OvalTool::processEvent(QEvent *e)
{
  Canvas *canvas = toolController()->view()->canvas();
  if(e->type() == QEvent::MouseButtonPress)
  {
    if(state == S_Init)
    {
      QMouseEvent *me = (QMouseEvent *)e;
      p1.setX(me->x());
      p1.setY(me->y());
      state = S_Resize;
    }
  }
  else if(e->type() == QEvent::MouseMove)
  {
    if(state == S_Resize)
    {
      QMouseEvent *me = (QMouseEvent *)e;
      canvas->repaint(r);
      if(p1.x() <= me->x())
      {
        r.setLeft(p1.x());
        r.setRight(me->x());
      }
      else
      {
        r.setLeft(me->x());
        r.setRight(p1.x());
      }
      if(p1.y() <= me->y())
      {
        r.setTop(p1.y());
        r.setBottom(me->y());
      }
      else
      {
        r.setTop(me->y());
        r.setBottom(p1.y());
      }
      QPainter p(canvas);
      p.setPen(blue);
      p.drawEllipse(r);
    }
/*      if (rect == 0L)
         return;

      QMouseEvent *me = (QMouseEvent *) e;
      float xpos = me->x (), ypos = me->y ();
      canvas->snapPositionToGrid (xpos, ypos);
      rect->setEndPoint (Coord (xpos, ypos));
      bool flag = me->state () & Qt::ControlButton;

      Rect r = rect->boundingBox ();
      MeasurementUnit unit =
         PStateManager::instance ()->defaultMeasurementUnit ();
      QString u = unitToString (unit);
      float xval, yval, wval, hval;
      xval = cvtPtToUnit (unit, r.x ());
      yval = cvtPtToUnit (unit, r.y ());
      wval = cvtPtToUnit (unit, r.width ());
      hval = cvtPtToUnit (unit, r.height ());

      msgbuf=flag ? i18n("Create Square") : i18n("Create Rectangle");
      msgbuf+=" ["+QString::number(xval, 'f', 3);
      msgbuf+=QString(" ") + u + QString(", ");
      msgbuf+=QString::number(yval, 'f', 3);
      msgbuf+=QString(" ") + u + QString(", ");
      msgbuf+=QString::number(wval, 'f', 3);
      msgbuf+=QString(" ") + u + QString(", ");
      msgbuf+=QString::number(hval, 'f', 3);
      msgbuf+=QString(" ") + u + QString("]");
      m_toolController->emitModeSelected (m_id,msgbuf);*/
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
    if(state == S_Resize)
    {
      GOval *oval = new GOval();
      float zoom = toolController()->view()->activeDocument()->zoomFactor();
      kdDebug(38000) << "OvalTool zoom : " << zoom << endl;
      oval->startPoint(KoPoint((r.left() - canvas->xOffset()) / zoom, (r.top() - canvas->yOffset()) / zoom));
      oval->endPoint(KoPoint((r.right() - canvas->xOffset()) / zoom, (r.bottom() - canvas->yOffset()) / zoom));
      CreateOvalCmd *cmd = new CreateOvalCmd(toolController()->view()->activeDocument(), oval);
      KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
	  oval->style(*(doc->document()->styles()->current())); // copy current style
      doc->history()->addCommand(cmd);
      canvas->updateBuf(r);
      canvas->repaint(r);
      state = S_Init;
    }
  }
  else if(e->type() == QEvent::KeyPress)
  {
    QKeyEvent *ke = (QKeyEvent *)e;
    if(ke->key() == Qt::Key_Escape);
  }
}

#include "OvalTool.moc"
