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

#include "TextTool.h"

#include <kaction.h>
#include <klocale.h>

#include "kontour_view.h"
#include "GPage.h"
#include "Canvas.h"
#include "ToolController.h"

TextTool::TextTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *text = new ToolSelectAction(actionCollection(), "ToolAction");
  KAction *mT1 = new KAction(i18n("Text"), "texttool", 0, actionCollection());
  text->insert(mT1);
}

void TextTool::activate()
{
//  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
}

void TextTool::deactivate()
{
}

void TextTool::processEvent(QEvent *e)
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

void TextTool::processButtonPressEvent(QMouseEvent *, GPage *, Canvas *)
{
}

void TextTool::processMouseMoveEvent(QMouseEvent *, GPage *, Canvas *)
{
}

void TextTool::processButtonReleaseEvent(QMouseEvent *, GPage *, Canvas *)
{
}

void TextTool::processKeyPressEvent(QKeyEvent *, GPage *, Canvas *)
{
}

#include "TextTool.moc"
