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

#include "RectTool.h"

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
#include "GRect.h"
#include "CreateRectCmd.h"

RectTool::RectTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *zoom = new ToolSelectAction(actionCollection(), "ToolAction");
  KRadioAction *mT1 = new KRadioAction(i18n("Square"), "rect1", 0, actionCollection());
  mT1->setExclusiveGroup("RectTool");
  connect(mT1,SIGNAL(activated()),SLOT(disableRoundness()));
  connect(mT1,SIGNAL(activated()),SLOT(enableFill()));
  connect(mT1,SIGNAL(activated()),SLOT(enableSquare()));
  KRadioAction *mT2 = new KRadioAction(i18n("Rectangle"),"rect2", 0, actionCollection());
  mT2->setExclusiveGroup("RectTool");
  connect(mT2,SIGNAL(activated()),SLOT(disableRoundness()));
  connect(mT2,SIGNAL(activated()),SLOT(enableFill()));
  connect(mT2,SIGNAL(activated()),SLOT(disableSquare()));
  KRadioAction *mT3 = new KRadioAction(i18n("Square"), "rect3", 0, actionCollection());
  mT3->setExclusiveGroup("RectTool");
  connect(mT3,SIGNAL(activated()),SLOT(enableRoundness()));
  connect(mT3,SIGNAL(activated()),SLOT(enableFill()));
  connect(mT3,SIGNAL(activated()),SLOT(enableSquare()));
  KRadioAction *mT4 = new KRadioAction(i18n("Rectangle"), "rect4", 0, actionCollection());
  mT4->setExclusiveGroup("RectTool");
  connect(mT4,SIGNAL(activated()),SLOT(enableRoundness()));
  connect(mT4,SIGNAL(activated()),SLOT(enableFill()));
  connect(mT4,SIGNAL(activated()),SLOT(disableSquare()));
  KRadioAction *mT5 = new KRadioAction(i18n("Square"), "rect5", 0, actionCollection());
  mT5->setExclusiveGroup("RectTool");
  connect(mT5,SIGNAL(activated()),SLOT(disableRoundness()));
  connect(mT5,SIGNAL(activated()),SLOT(disableFill()));
  connect(mT5,SIGNAL(activated()),SLOT(enableSquare()));
  KRadioAction *mT6 = new KRadioAction(i18n("Rectangle"), "rect6", 0, actionCollection());
  mT6->setExclusiveGroup("RectTool");
  connect(mT6,SIGNAL(activated()),SLOT(disableRoundness()));
  connect(mT6,SIGNAL(activated()),SLOT(disableFill()));
  connect(mT6,SIGNAL(activated()),SLOT(disableSquare()));
  KRadioAction *mT7 = new KRadioAction(i18n("Square"), "rect7", 0, actionCollection());
  mT7->setExclusiveGroup("RectTool");
  connect(mT7,SIGNAL(activated()),SLOT(enableRoundness()));
  connect(mT7,SIGNAL(activated()),SLOT(disableFill()));
  connect(mT7,SIGNAL(activated()),SLOT(enableSquare()));
  KRadioAction *mT8 = new KRadioAction(i18n("Rectangle"), "rect8", 0, actionCollection());
  mT8->setExclusiveGroup("RectTool");
  connect(mT8,SIGNAL(activated()),SLOT(enableRoundness()));
  connect(mT8,SIGNAL(activated()),SLOT(disableFill()));
  connect(mT8,SIGNAL(activated()),SLOT(disableSquare()));
  zoom->insert(mT1);
  zoom->insert(mT2);
  zoom->insert(mT3);
  zoom->insert(mT4);
  zoom->insert(mT5);
  zoom->insert(mT6);
  zoom->insert(mT7);
  zoom->insert(mT8);
}

void RectTool::activate()
{
  state = S_Init;
  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
  toolController()->view()->setStatus(i18n("Rectangle Mode"));
}

void RectTool::deactivate()
{
}

void RectTool::processEvent(QEvent *e)
{
  Canvas *canvas = toolController()->view()->canvas();
  GPage *page = toolController()->view()->activeDocument()->activePage();
  if(e->type() == QEvent::MouseButtonPress)
  {
    if(state == S_Init)
    {
      QMouseEvent *me = (QMouseEvent *)e;
      p1.setX(me->x());
      p1.setY(me->y());
      state = S_Resize;
    }
//    float xpos = me->x (), ypos = me->y ();
//    canvas->snapPositionToGrid (xpos, ypos);

  }
  else if(e->type() == QEvent::MouseMove)
  {
    if(state == S_Resize)
    {
      QMouseEvent *me = (QMouseEvent *) e;
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
      if(mRoundness)
        p.drawRoundRect(r);
      else
        p.drawRect(r);
    }
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
    if(state == S_Resize)
    {
      if(r.width() == 0 && r.height() == 0)
      {
        //TODO Insert rect dialog
      }
      else
      {
        double zoom = toolController()->view()->activeDocument()->zoomFactor();
        GRect *rect = new GRect(r.width() / zoom, r.height() / zoom, mRoundness);
        QWMatrix m;
        m = m.translate((r.left() - canvas->xOffset()) / zoom, (r.top() - canvas->yOffset()) / zoom);
        rect->transform(m);
        CreateRectCmd *cmd = new CreateRectCmd(toolController()->view()->activeDocument(), rect);
        KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
        rect->style(doc->document()->styles()->style());  // copy current style
	if(!mFill)
	  rect->style()->filled(GStyle::NoFill);
        doc->history()->addCommand(cmd);
      }
      state = S_Init;
    }
  }
}

void RectTool::enableRoundness()
{
  mRoundness = true;
}

void RectTool::disableRoundness()
{
  mRoundness = false;
}

void RectTool::enableFill()
{
  mFill = true;
}

void RectTool::disableFill()
{
  mFill = false;
}

void RectTool::enableSquare()
{
  mSquare = true;
}

void RectTool::disableSquare()
{
  mSquare = false;
}

#include "RectTool.moc"
