/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include "units.h"
#include <iostream.h>

const char* unitNames[] = { "pt", "mm", "inch" };

/*
 * Functions for converting Point <-> Inch <-> Millimeter
 *
 * 1 Inch = 72 pt = 25.4 mm
 */

float cvtPtToMm (float value) {
  return 25.4 * value / 72.0;
}

float cvtPtToInch (float value) {
  return value / 72.0;
}

float cvtInchToPt (float value) {
  return value * 72.0;
}

float cvtMmToPt (float value) {
  return value / 25.4 * 72.0;
}

float cvtPtToUnit (MeasurementUnit unit, float value) {
  switch (unit) {
  case UnitMillimeter:
    return cvtPtToMm (value);
    break;
  case UnitInch:
    return cvtPtToInch (value);
    break;
  default:
    return value;
  }
}

float cvtUnitToPt (MeasurementUnit unit, float value) {
  switch (unit) {
  case UnitMillimeter:
    return cvtMmToPt (value);
    break;
  case UnitInch:
    return cvtInchToPt (value);
    break;
  default:
    return value;
  }
}

const char* unitToString (MeasurementUnit unit) {
  return unitNames[unit];
}
