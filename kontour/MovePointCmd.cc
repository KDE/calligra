/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Jansen (rm@kde.org)

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

#include "MovePointCmd.h"

#include <klocale.h>

#include "GObject.h"

MovePointCmd::MovePointCmd(GDocument *aGDoc, GObject *o, int idx, double dx, double dy):
Command(aGDoc, i18n("Move Point"))
{
  obj = o;
  obj->ref();
  index = idx;
  xoff = dx;
  yoff = dy;
}

MovePointCmd::~MovePointCmd()
{
  obj->unref();
}

void MovePointCmd::execute()
{
  obj->movePoint(index, xoff, yoff);
}

void MovePointCmd::unexecute()
{
  obj->movePoint(index, -xoff, -yoff);
}
