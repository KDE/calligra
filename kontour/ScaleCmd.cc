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

#include "ScaleCmd.h"

#include <klocale.h>
#include <kdebug.h>

#include "kontour_global.h"
#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

ScaleCmd::ScaleCmd(GDocument *aGDoc, int mask, double x, double y, KoRect r):
TransformationCmd(aGDoc, i18n("Scale"))
{
  sx = x;
  sy = y;
  hmask = mask;
  box = r;
}

void ScaleCmd::execute()
{
  QWMatrix m1, m2, m3;

  double xoff = box.x();
  double yoff = box.y();
  double xback = xoff;
  double yback = yoff;

  kdDebug() << "SX = " << sx << " SY = " << sy << endl;

  if(hmask & Kontour::HPos_Left)
    xback = box.left() + box.width() * (1.0 - sx);
  if(hmask & Kontour::HPos_Top)
    yback = box.top() + box.height() * (1.0 - sy);
  if(hmask & Kontour::HPos_Center)
  {
    xback = box.left() + box.width() * 0.5 * (1.0 - sx);
    yback = box.top() + box.height() * 0.5 * (1.0 - sy);
  }

  m1.translate(-xoff, -yoff);
  m2.scale(sx, sy);
  m3.translate(xback, yback);

  TransformationCmd::execute();
  for(unsigned int i = 0; i < objects.count(); i++)
  {
    objects[i]->transform(m1);
    objects[i]->transform(m2);
    objects[i]->transform(m3, true);
  }
  document()->activePage()->updateSelection();
}
