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

#include "ToolController.h"

#include <kxmlguifactory.h>
#include <kdebug.h>

#include "kontour_view.h"

using namespace Kontour;

ToolController::ToolController(KontourView *aView)
{
  mActiveTool = 0L;
  mView = aView;
}

ToolController::~ToolController()
{
}

void ToolController::registerTool(Tool *tool)
{
  tools.append(tool);
}

void ToolController::selectTool(Tool *t)
{
  if(mActiveTool == t || !t)
    return;

  if(mActiveTool)
    mActiveTool->deactivate();

  mActiveTool = t;

  QPtrListIterator<Tool> it(tools);
  for(; it.current(); ++it)
    if(it.current()->action())
      if(it.current() != mActiveTool)
        it.current()->action()->setToggleState(false);
      else
        it.current()->action()->setToggleState(true);

  mActiveTool->activate();
}

void ToolController::selectTool(QString id)
{
  QPtrListIterator<Tool> it(tools);
  for(; it.current(); ++it)
    if(it.current()->id() == id)
    {
      selectTool(it.current());
      return;
    }
}

void ToolController::delegateEvent(QEvent *e)
{
  if(mActiveTool)
    mActiveTool->processEvent(e);
}

void ToolController::initToolBar()
{
  QWidget *tb = mView->factory()->container("tools", mView);
  tb->hide();

  kdDebug(38000) << "Tools:" << endl;
  QPtrListIterator<Tool> it(tools);
  for(; it.current(); ++it)
  {
//    kdDebug(38000) << it.currentKey().local8Bit() << endl;
    KAction *ta = it.current()->action();
    if(ta && tb)
    {
      ta->plug(tb);
      connect(ta, SIGNAL(activated()), SLOT(toolActivated()));
    }
  }
  tb->show();
}

void ToolController::toolActivated()
{
  ToolSelectAction *ta = (ToolSelectAction*)sender();

  QPtrListIterator<Tool> it(tools);
  for(; it.current(); ++it)
    if(it.current()->action() == ta)
    {
      selectTool(it.current());
      break;
    }
}

#include "ToolController.moc"
