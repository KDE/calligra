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

#include "DeleteCmd.h"

#include <klocale.h>

#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

DeleteCmd::DeleteCmd(GDocument *aGDoc):
Command(aGDoc, i18n("Delete"))
{
  objects.setAutoDelete(true);
  for(QPtrListIterator<GObject> it(document()->activePage()->getSelection()); it.current(); ++it)
  {
    MyPair *p = new MyPair;
    p->o=*it;
    p->o->ref();
    /* store the old position of the object */
    p->pos = document()->activePage()->findIndexOfObject(p->o);
    objects.append(p);
  }
}

DeleteCmd::~DeleteCmd()
{
  for(MyPair *p = objects.first(); p != 0; p = objects.next())
    p->o->unref();
}

void DeleteCmd::execute()
{
  for(MyPair *p = objects.first(); p != 0L; p = objects.next())
    document()->activePage()->deleteObject(p->o);
}

void DeleteCmd::unexecute()
{
  document()->activePage()->unselectAllObjects();
  for(MyPair *p = objects.first(); p != 0L; p = objects.next())
  {
    /* insert the object at the old position */
    document()->activePage()->insertObjectAtIndex(p->o, p->pos);
    document()->activePage()->selectObject(p->o);
  }
}
