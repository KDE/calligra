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

#include "SetPropertyCmd.h"

#include <klocale.h>

#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

SetPropertyCmd::SetPropertyCmd(GDocument *aGDoc, const GStyle &s):
Command(aGDoc, i18n("Set property"))
{
  st = s;
  objects.resize(document()->activePage()->selectionCount());
  states.resize(document()->activePage()->selectionCount());
  QPtrListIterator<GObject> it(document()->activePage()->getSelection());
  for(unsigned int i = 0; it.current(); ++it, ++i)
  {
    (*it)->ref();
    objects.insert(i, (*it));
    states[i] = (*it)->style();
  }
}

SetPropertyCmd::~SetPropertyCmd()
{
  for(unsigned int i = 0; i < objects.count(); i++)
    objects[i]->unref();
}

void SetPropertyCmd::execute()
{
  for(unsigned int i = 0; i < objects.count(); i++)
    objects[i]->style(st);
}

void SetPropertyCmd::unexecute()
{
  document()->activePage()->unselectAllObjects();
  for(unsigned int i = 0; i < objects.count(); i++)
  {
    objects[i]->style(states[i]);
    document()->activePage()->selectObject(objects[i]);
  }
  document()->activePage()->updateSelection();
}
