/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <SplitLineCmd.h>
#include <klocale.h>
#include <GDocument.h>
#include <GPolyline.h>
#include <kdebug.h>
#include "GPage.h"

SplitLineCmd::SplitLineCmd (GDocument* doc, GPolyline* o, int idx)
  : Command(i18n("Split Line"))
{
  document = doc;
  obj = o;
  obj->ref ();
  obj1 = obj2 = 0L;
  index = idx;
  pos = 0;
}

SplitLineCmd::~SplitLineCmd () {
  obj->unref ();
  if (obj1) obj1->unref ();
  if (obj2) obj2->unref ();
}

void SplitLineCmd::execute () {
  if (obj1) obj1->unref ();
  if (obj2) obj2->unref ();
  obj1 = obj2 = 0L;

  if (obj->splitAt (index, obj1, obj2))
  {
    pos = document->activePage()->findIndexOfObject (obj);
    document->activePage()->deleteObject (obj);
    document->activePage()->unselectAllObjects ();
    document->activePage()->insertObjectAtIndex (obj1, pos);
    document->activePage()->selectObject (obj1);
    if (obj2)
    {
      document->activePage()->insertObjectAtIndex (obj2, pos + 1);
      document->activePage()->selectObject (obj2);
    }
  }
}

void SplitLineCmd::unexecute () {
  if (obj1) document->activePage()->deleteObject (obj1);
  if (obj2) document->activePage()->deleteObject (obj2);
  document->activePage()->insertObjectAtIndex (obj, pos);
  document->activePage()->selectObject (obj);
}

