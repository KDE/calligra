/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 2002 Igor Janssen (rm@kde.org)

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

#include "PathTool.h"

#include <qpainter.h>

#include <kaction.h>
#include <klocale.h>

#include "kontour_view.h"
#include "GPage.h"
#include "Canvas.h"
#include "ToolController.h"

PathTool::PathTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *zoom = new ToolSelectAction(actionCollection(), "ToolAction");
  KRadioAction *mT1 = new KRadioAction(i18n("Path"), "linetool", 0, actionCollection());
  mT1->setExclusiveGroup("PathTool");
  KRadioAction *mT2 = new KRadioAction(i18n("Path"),"beziertool", 0, actionCollection());
  mT2->setExclusiveGroup("PathTool");
  KRadioAction *mT3 = new KRadioAction(i18n("Path"), "beziertool", 0, actionCollection());
  mT3->setExclusiveGroup("PathTool");
  KRadioAction *mT4 = new KRadioAction(i18n("Path"),"linetool", 0, actionCollection());
  mT4->setExclusiveGroup("PathTool");
  zoom->insert(mT1);
  zoom->insert(mT2);
  zoom->insert(mT3);
  zoom->insert(mT4);
}

void PathTool::activate()
{
  state = S_Init;
  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
}

void PathTool::deactivate()
{
}

void PathTool::processEvent(QEvent *e)
{
  Canvas *canvas = toolController()->view()->canvas();  
  GPage *page = toolController()->view()->activeDocument()->activePage();
  double x = static_cast<QMouseEvent *>(e)->x() - canvas->xOffset();
  double y = static_cast<QMouseEvent *>(e)->y() - canvas->yOffset();
  if(e->type() == QEvent::MouseButtonPress)
  {
  }
  else if(e->type() == QEvent::MouseMove)
  {
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
  }
}

#include "PathTool.moc"
