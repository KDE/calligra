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

#include "EditPointTool.h"

#include <kaction.h>
#include <klocale.h>

#include "kontour_view.h"
#include "Canvas.h"
#include "ToolController.h"

EditPointTool::EditPointTool(QString aId, ToolController *tc):
Tool(aId, tc)
{
  ToolSelectAction *editpoint = new ToolSelectAction(actionCollection(), "ToolAction");
  KAction *mT1 = new KAction(i18n("Edit Point"), "", 0, actionCollection());
  editpoint->insert(mT1);
}

void EditPointTool::activate()
{
  toolController()->view()->canvas()->setCursor(Qt::crossCursor);
}

void EditPointTool::deactivate()
{
}

void EditPointTool::processEvent(QEvent *e)
{
}

#include "EditPointTool.moc"
