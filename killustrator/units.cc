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

#include <units.h>
#include <qstring.h>
#include <klocale.h>

/*
 * Functions for converting Point <-> Inch <-> Millimeter
 * <-> Pica <-> Centimetre  <-> Didot <-> Cicero
 * 1 Inch = 72 pt = 6 pica = 25.4 mm = 67.54151050 dd =  5.628459208 cc
 */

float cvtPtToMm (float value) {
  return 25.4 * value / 72.0;
}

float cvtPtToCm (float value) {
  return 2.54 * value / 72.0;
}

float cvtPtToPica (float value) {
  return value / 12.0;
}

float cvtPicaToPt (float value) {
  return value * 12.0;
}

float cvtPtToInch (float value) {
  return value / 72.0;
}

float cvtPtToDidot (float value) {
  return value * 1238.0 / 1157.0; // 1157 dd = 1238 pt
}

float cvtPtToCicero (float value) {
  return value * 1238.0 / (1157.0 * 12.0); // 1 cc = 12 dd
}

float cvtInchToPt (float value) {
  return value * 72.0;
}

float cvtMmToPt (float value) {
  return value / 25.4 * 72.0;
}

float cvtCmToPt (float value) {
  return value / 2.54 * 72.0;
}

float cvtDidotToPt (float value) {
  return value * 1157.0 / 1238.0;
}

float cvtCiceroToPt (float value) {
  return value * (1157.0 * 12.0) / 1238.0;
}

float cvtPtToUnit (MeasurementUnit unit, float value) {
  switch (unit) {
  case UnitMillimeter:
    return cvtPtToMm (value);
    break;
  case UnitPica:
    return cvtPtToPica (value);
    break;
  case UnitInch:
    return cvtPtToInch (value);
    break;
  case UnitCentimeter:
    return cvtPtToCm (value);
    break;
  case UnitDidot:
    return cvtPtToDidot (value);
    break;
  case UnitCicero:
    return cvtPtToCicero (value);
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
  case UnitCentimeter:
    return cvtCmToPt (value);
    break;
  case UnitDidot:
    return cvtDidotToPt (value);
    break;
  case UnitPica:
    return cvtPicaToPt (value);
    break;
  case UnitCicero:
    return cvtCiceroToPt (value);
    break;
  default:
    return value;
  }
}

QString unitToString (MeasurementUnit unit) {
    if(unit==UnitPoint)
        return i18n("pt");
    else if(unit==UnitMillimeter)
        return i18n("mm");
    else if(unit==UnitInch)
        return i18n("inch");
    else if(unit==UnitPica)
        return i18n("pica");
    else if(unit==UnitCentimeter)
        return i18n("cm");
    else if(unit==UnitDidot)
        return i18n("didot");
    else
        return i18n("cicero");
}
