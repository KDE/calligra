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

#include <cmath>

#include <qpainter.h>

#include <kaction.h>
#include <klocale.h>

#include "kontour_global.h"
#include "kontour_view.h"
#include "GPage.h"
#include "Canvas.h"
#include "GPolygon.h"
#include "CreatePolygonCmd.h"
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
  n = 5;
}

void PolygonTool::activate()
{
  state = S_Init;
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
  double x = static_cast<QMouseEvent *>(e)->x() - canvas->xOffset();
  double y = static_cast<QMouseEvent *>(e)->y() - canvas->yOffset();
  if(e->type() == QEvent::MouseButtonPress)
  {
    if(state == S_Init)
    {
      mCenter.setX(x);
      mCenter.setY(y);
      radius = 0;
      state = S_Resize;
    }
  }
  else if(e->type() == QEvent::MouseMove)
  {
    if(state == S_Resize)
    {
      QRect rect = QRect(static_cast<int>(mCenter.x() - radius + canvas->xOffset()), static_cast<int>(mCenter.y() - radius + canvas->yOffset()), 2 * static_cast<int>(radius), 2 * static_cast<int>(radius));
      canvas->repaint(rect);  
      radius = sqrt((x - mCenter.x()) * (x - mCenter.x()) + (y - mCenter.y()) * (y - mCenter.y()));
      double a = (x - mCenter.x()) / radius;
      a = acos(a);
      if(y < mCenter.y())
        a = 2.0 * Kontour::pi - a;
      drawStar(radius, 0.5 * radius, a);
      //drawPolygon(radius, a);
    }
  }
  else if(e->type() == QEvent::MouseButtonRelease)
  {
    if(state == S_Resize)
    {
      radius = sqrt((x - mCenter.x()) * (x - mCenter.x()) + (y - mCenter.y()) * (y - mCenter.y()));
      double a = (x - mCenter.x()) / radius;
      a = acos(a);
      if(y < mCenter.y())
        a = 2.0 * Kontour::pi - a;
      GPolygon *polygon = new GPolygon(mCenter, n, radius, a);
      CreatePolygonCmd *cmd = new CreatePolygonCmd(toolController()->view()->activeDocument(), polygon);
      KontourDocument *doc = (KontourDocument *)toolController()->view()->koDocument();
      polygon->style(doc->document()->styles()->style());
//    if(!mFill)
//    rect->style()->filled(GStyle::NoFill);
      doc->history()->addCommand(cmd);
      state = S_Init;
    }
  }
}

void PolygonTool::drawPolygon(double r, double a)
{
  Canvas *canvas = toolController()->view()->canvas();
  QPainter p(canvas);
  p.setPen(blue);
  double ca = 2.0 * Kontour::pi / static_cast<double>(n);
  double x = mCenter.x() + r * cos(a);
  double y = mCenter.y() + r * sin(a);
  int cx = static_cast<int>(x + canvas->xOffset());
  int cy = static_cast<int>(y + canvas->yOffset());
  p.moveTo(cx, cy);
  for(int i = 0; i < n; i++)
  {
    a += ca;
    x = mCenter.x() + r * cos(a);
    y = mCenter.y() + r * sin(a);
    cx = static_cast<int>(x + canvas->xOffset());
    cy = static_cast<int>(y + canvas->yOffset());
    p.lineTo(cx, cy);
  }
}

void PolygonTool::drawStar(double r1, double r2, double a)
{
  Canvas *canvas = toolController()->view()->canvas();
  QPainter p(canvas);
  p.setPen(blue);
  double ca = Kontour::pi / static_cast<double>(n);
  double x = mCenter.x() + r1 * cos(a);
  double y = mCenter.y() + r1 * sin(a);
  int cx = static_cast<int>(x + canvas->xOffset());
  int cy = static_cast<int>(y + canvas->yOffset());
  p.moveTo(cx, cy);
  for(int i = 0; i < n; i++)
  {
    a += ca;
    x = mCenter.x() + r2 * cos(a);
    y = mCenter.y() + r2 * sin(a);
    cx = static_cast<int>(x + canvas->xOffset());
    cy = static_cast<int>(y + canvas->yOffset());
    p.lineTo(cx, cy);
    a += ca;
    x = mCenter.x() + r1 * cos(a);
    y = mCenter.y() + r1 * sin(a);
    cx = static_cast<int>(x + canvas->xOffset());
    cy = static_cast<int>(y + canvas->yOffset());
    p.lineTo(cx, cy);
  }
}

#include "PolygonTool.moc"
