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

#include "ShearCmd.h"

#include <klocale.h>

#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

ShearCmd::ShearCmd(GDocument *aGDoc, const KoPoint &center, double x, double y):
TransformationCmd(aGDoc, i18n("Shear"))
{
  rcenter = center;
  sx = x;
  sy = y;
}

void ShearCmd::execute()
{
  QWMatrix m1, m2, m3;
  m1.translate(-rcenter.x(), -rcenter.y());
  m2.shear(sx, sy);
  m3.translate(rcenter.x(), rcenter.y());
  TransformationCmd::execute();
  for(unsigned int i = 0; i < objects.count(); i++)
  {
    objects[i]->transform(m1);
    objects[i]->transform(m2);
    objects[i]->transform(m3, true);
  }
  document()->activePage()->updateSelection();
}
