/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
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

#include "ToPathCmd.h"

#include <klocale.h>

#include "GDocument.h"
#include "GPage.h"
#include "GPath.h"

ToPathCmd::ToPathCmd(GDocument *aGDoc):
Command(aGDoc, i18n("Convert to Path"))
{
  QPtrListIterator<GObject> it(document()->activePage()->getSelection());
  for(; it.current(); ++it)
    if((*it)->isConvertible())
    {
      (*it)->ref();
      objects.append(*it);
    }
}

ToPathCmd::~ToPathCmd()
{
  GObject *o;
  for(o = objects.first(); o != 0L; o = objects.next())
    o->unref();
  for(o = paths.first(); o != 0L; o = paths.next())
    o->unref();
}

void ToPathCmd::execute()
{
  for(GObject *i = objects.first(); i != 0L; i = objects.next())
  {
    unsigned int idx = document()->activePage()->findIndexOfObject(i);
    GPath *path = i->convertToPath();
    if(path)
    {
      paths.append(path);
      document()->activePage()->deleteObject(i);
      document()->activePage()->insertObjectAtIndex(path, idx);
      document()->activePage()->selectObject(path);
    }
  }
}

void ToPathCmd::unexecute()
{
  GPath *c = paths.first();
  GObject *o = objects.first();
  for(; c != 0L; c = paths.next(), o = objects.next())
  {
    unsigned int idx = document()->activePage()->findIndexOfObject(c);
    document()->activePage()->deleteObject(c);
    document()->activePage()->insertObjectAtIndex(o, idx);
  }
}
