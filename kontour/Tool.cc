/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

#include "Tool.h"

#include <qwidget.h>

#include <ktoolbar.h>
#include <ktoolbarbutton.h>

#include "kontour_factory.h"
#include "ToolController.h"

using namespace Kontour;

ToolSelectAction::ToolSelectAction(QObject *parent, const char *name):
KActionMenu("",parent,name)
{
  mActSelf = false;
  mInit = false;
  mDef = 0L;
  mCount = 0;
}

void ToolSelectAction::insert(KAction *a, int index)
{
  mCount++;
  KActionMenu::insert(a, index);
  if(!mInit)
  {
    setDefaultAction(a);
    mInit = true;
  }
  connect(a, SIGNAL(activated()), SLOT(childActivated()));
}

void ToolSelectAction::remove(KAction *a)
{
  mCount--;
  KActionMenu::remove(a);
  a->disconnect(this, SIGNAL(activated()));
}

int ToolSelectAction::plug(QWidget *widget, int index)
{
  if(widget->inherits("KToolBar"))
  {
    KToolBar *bar = (KToolBar*)widget;
    int i = (mCount == 1 ) ? KAction::plug(widget,index) : KActionMenu::plug(widget, index);
    bar->setToggle(itemId(i), true);
    return i;
  }
  return -1;
}

void ToolSelectAction::setDefaultAction(KAction *a)
{
  KAction::setText(a->text());
  setShortcut(KShortcut(a->shortcut().keyCodeQt()));
  setGroup(a->group());
  setWhatsThis(a->whatsThis());
  setToolTip(a->toolTip());
  setEnabled(a->isEnabled());
  setIcon(a->icon());
  mDef = a;
}

void ToolSelectAction::slotActivated()
{
  emit activated();

  if(mDef)
  {
    mActSelf = true;
    if(mDef->inherits("KToggleAction"))
    {
      KToggleAction *ta = (KToggleAction*)mDef;
      ta->setChecked(false);
      ta->activate();
      ta->setChecked(true);
    }
    else
      mDef->activate();
    mActSelf = false;
  }
}

void ToolSelectAction::childActivated()
{
  setDefaultAction((KAction*)sender());
  if(!mActSelf)
    activate();
}

void ToolSelectAction::setToggleState(bool state)
{
  int len = containerCount();
  for(int id = 0; id < len; ++id)
  {
    KToolBar *w = (KToolBar*)container(id);
    KToolBarButton *b = w->getButton(itemId(id));
    b->on(state);
  }
}

/*-----------------------*/

Tool::Tool(QString aId, ToolController *tc)
{
  setInstance(KontourFactory::global());
  mToolController = tc;
  mId = aId;
}

Tool::~Tool()
{

}

ToolSelectAction *Tool::action()
{
  return (ToolSelectAction*)actionCollection()->action("ToolAction");
}

#include "Tool.moc"
