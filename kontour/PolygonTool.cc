/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
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

#include "PolygonTool.h"

#include <kaction.h>
#include <klocale.h>

#include "kontour_view.h"
#include "GPage.h"
#include "Canvas.h"
#include "ToolController.h"

PolygonTool::PolygonTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *zoom = new ToolSelectAction(actionCollection(), "ToolAction");
  KRadioAction *mT1 = new KRadioAction(i18n("Polygon"), "polygontool", 0, actionCollection());
  mT1->setExclusiveGroup("PolygonTool");
  KRadioAction *mT2 = new KRadioAction(i18n("Polygon"),"polygontool", 0, actionCollection());
  mT2->setExclusiveGroup("PolygonTool");
  zoom->insert(mT1);
  zoom->insert(mT2);
}

void PolygonTool::activate()
{
//  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
//  toolController()->view()->setStatus(i18n("Rectangle Mode"));
}

void PolygonTool::deactivate()
{
}

void PolygonTool::processEvent(QEvent *e)
{
  Canvas *canvas = toolController()->view()->canvas();
  GPage *page = toolController()->view()->activeDocument()->activePage();
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

#include "PolygonTool.moc"
