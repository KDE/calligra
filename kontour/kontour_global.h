/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
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

#ifndef __kontour_global_h__
#define __kontour_global_h__

#include <koPoint.h>

#include <math.h>

namespace Kontour
{
  const double nearDistance = 4.0;
  const double minZoomFactor = 0.06;
  const double maxZoomFactor = 100.0;
  const double pi = 3.14159265358979323846;
  
  inline double segLength(const KoPoint &c1, const KoPoint &c2)
  {
    double dx = c2.x() - c1.x();
    double dy = c2.y() - c1.y();
    return sqrt(dx * dx + dy * dy);
  }
}

#endif
