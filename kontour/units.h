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

#ifndef __units_h__
#define __units_h__

class QString;

namespace Kontour {
enum MeasurementUnit
{
  UnitPoint, UnitMillimeter, UnitInch, UnitPica, UnitCentimeter,
  UnitDidot, UnitCicero
};

double cvtPtToMm(double value);
double cvtPtToCm(double value);
double cvtPtToInch(double value);
double cvtPtToPica(double value);
double cvtPtToDidot(double value);
double cvtPtToCicero(double value);
double cvtMmToPt(double value);
double cvtCmToPt(double value);
double cvtInchToPt(double value);
double cvtPicaToPt(double value);
double cvtDidotToPt(double value);
double cvtCiceroToPt(double value);

double cvtPtToUnit(MeasurementUnit unit, double value);
double cvtUnitToPt(MeasurementUnit unit, double value);

QString unitToString(MeasurementUnit unit);
};
using namespace Kontour;

#endif
