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

#include "ReorderCmd.h"

#include <klocale.h>

#include "GDocument.h"
#include "GPage.h"
#include "GLayer.h"
#include "GObject.h"

ReorderCmd::ReorderCmd(GDocument *aGDoc, ReorderPosition pos):
Command(aGDoc, i18n("Reorder"))
{
  objects.resize(document()->activePage()->selectionCount());
  oldpos.resize(document()->activePage()->selectionCount());

  QPtrListIterator<GObject> it(document()->activePage()->getSelection());
  for(unsigned int i = 0; it.current(); ++it, ++i)
  {
    GObject *o = *it;
    o->ref();
    objects.insert(i, o);
  }
  position = pos;
}

ReorderCmd::~ReorderCmd()
{
  for(unsigned int i = 0; i < objects.count(); i++)
    objects[i]->unref();
}

void ReorderCmd::execute()
{
  for(unsigned int i = 0; i < objects.count(); i++)
  {
    unsigned int newidx =  0;

    /* look for the object */
    unsigned int idx = document()->activePage()->findIndexOfObject(objects[i]);
    oldpos[i] = idx;
    if(position == RP_ToFront || position == RP_ForwardOne)
    {
      if(idx == objects[i]->layer()->objectCount() - 1)
      /* already at the first position */
        continue;
      /* move the object */
      if(position == RP_ToFront)
        newidx = objects[i]->layer()->objectCount() - 1;
      else
        newidx = idx + 1;
    }
    else
    {
      if(idx == 0)
      /* already at the last position */
        continue;
      /* move the object */
      if(position == RP_ToBack)
        newidx = 0;
      else
        newidx = idx - 1;
    }
    document()->activePage()->moveObjectToIndex(objects[i], newidx);
  }
}

void ReorderCmd::unexecute()
{
  for(unsigned int i = 0; i < objects.count(); i++)
    document()->activePage()->moveObjectToIndex(objects[i], oldpos[i]);
}
