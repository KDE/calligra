/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Jansen (rm@kde.org)

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

#include <qpainter.h>

#include <klocale.h>
#include <kdebug.h>

#include "kontour_factory.h"
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
  ToolSelectAction *oval = new ToolSelectAction(actionCollection(), "ToolAction");
  KRadioAction *mT11 = new KRadioAction(i18n("Ellipse"), "ellipse1", 0, actionCollection());
  mT11->setExclusiveGroup("OvalTool");
  KRadioAction *mT12 = new KRadioAction(i18n("Circle"), "ellipse1", 0, actionCollection());
  mT12->setExclusiveGroup("OvalTool");
  KRadioAction *mT13 = new KRadioAction(i18n("Ellipse Arc"), "ellipse1", 0, actionCollection());
  mT13->setExclusiveGroup("OvalTool");
  KRadioAction *mT14 = new KRadioAction(i18n("Circle Arc"), "ellipse2", 0, actionCollection());
  mT14->setExclusiveGroup("OvalTool");
  KRadioAction *mT15 = new KRadioAction(i18n("Ellipse Sector"), "ellipse1", 0, actionCollection());
  mT15->setExclusiveGroup("OvalTool");
  KRadioAction *mT16 = new KRadioAction(i18n("Circle Sector"), "ellipse2", 0, actionCollection());
  mT16->setExclusiveGroup("OvalTool");
  KRadioAction *mT21 = new KRadioAction(i18n("Ellipse Segment"), "ellipse1", 0, actionCollection());
  mT21->setExclusiveGroup("OvalTool");
  KRadioAction *mT22 = new KRadioAction(i18n("Circle Segment"), "ellipse2", 0, actionCollection());
  mT22->setExclusiveGroup("OvalTool");
  KRadioAction *mT23 = new KRadioAction(i18n("Circle"), "ellipse1", 0, actionCollection());
  mT23->setExclusiveGroup("OvalTool");
  KRadioAction *mT24 = new KRadioAction(i18n("Circle"), "ellipse2", 0, actionCollection());
  mT24->setExclusiveGroup("OvalTool");
  KRadioAction *mT25 = new KRadioAction(i18n("Circle"), "ellipse1", 0, actionCollection());
  mT25->setExclusiveGroup("OvalTool");
  KRadioAction *mT26 = new KRadioAction(i18n("Circle"), "ellipse2", 0, actionCollection());
  mT26->setExclusiveGroup("OvalTool");
  oval->insert(mT11);
  oval->insert(mT12);
  oval->insert(mT13);
  oval->insert(mT14);
  oval->insert(mT15);
  oval->insert(mT16);
  oval->insert(mT21);
  oval->insert(mT22);
  oval->insert(mT23);
  oval->insert(mT24);
  oval->insert(mT25);
  oval->insert(mT26);
}

void OvalTool::activate()
{
  state = S_Init;
  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
}

void OvalTool::deactivate()
{
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
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
    if(state == S_Resize)
    {
      double zoom = toolController()->view()->activeDocument()->zoomFactor();
      GOval *oval = new GOval(r.width() / zoom / 2.0, r.height() / zoom / 2.0);
      QWMatrix m;
      m = m.translate((r.left() + r.width() / 2 - canvas->xOffset()) / zoom, (r.top() + r.height() / 2 - canvas->yOffset()) / zoom);
      oval->transform(m);
      CreateOvalCmd *cmd = new CreateOvalCmd(toolController()->view()->activeDocument(), oval);
      KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
      oval->style(doc->document()->styles()->style()); // copy current style
      doc->history()->addCommand(cmd);
      state = S_Init;
    }
  }
}

#include "OvalTool.moc"
