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

#include "ZoomTool.h"

#include <qpainter.h>

#include <kaction.h>
#include <klocale.h>

#include "kontour_view.h"
#include "Canvas.h"
#include "ToolController.h"

ZoomTool::ZoomTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction* zoom = new ToolSelectAction(actionCollection(), "ToolAction");
  KRadioAction *mT1 = new KRadioAction(i18n("Zoom in"), "viewmag+", 0, actionCollection());
  KRadioAction *mT2 = new KRadioAction(i18n("Zoom out"), "viewmag-", 0, actionCollection());
  mT1->setExclusiveGroup("ZoomTool");
  mT2->setExclusiveGroup("ZoomTool");
  zoom->insert(mT1);
  zoom->insert(mT2);
}
  
void ZoomTool::activate()
{
  state = S_Init;
}

void ZoomTool::deactivate()
{
}

void ZoomTool::processEvent(QEvent *e)
{
  Canvas *canvas = toolController()->view()->canvas();
  if(e->type() == QEvent::MouseButtonPress)
  {
    if(state == S_Init)
    {
      QMouseEvent *me = (QMouseEvent *)e;
      p1.setX(me->x());
      p1.setY(me->y());
      state = S_Rubberband;
    }
  }
  else if(e->type() == QEvent::MouseMove)
  {
    if(state == S_Rubberband)
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
      p.setPen(QPen(blue, 1, Qt::DotLine));
      p.drawRect(r);
    }
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
    if(state == S_Rubberband)
    {
      state = S_Init;
    }
  }
  else if(e->type() == QEvent::KeyPress)
  {
  }
}

#include "ZoomTool.moc"
