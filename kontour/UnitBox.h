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

#ifndef __UnitBox_h__
#define __UnitBox_h__

#include <qhbox.h>
#include "units.h"

class KComboBox;
class KDoubleNumInput;

namespace Kontour {
class UnitBox : public QHBox
{
  Q_OBJECT
public:
  UnitBox(QWidget *parent = 0L, const char *name = 0L);
  ~UnitBox();

  void setPrecision(int prec);

  double getValue();
  void setValue(double value);

  void setStep(double step);
  double getStep() const;

  void setRange(double minVal, double maxVal);
  void getRange(double &minVal, double &maxVal);

  void enableUnits(bool flag);

  static void setDefaultMeasurementUnit(MeasurementUnit unit);

signals:
  void valueChanged(double val);

protected slots:
  void unitChanged(int id);
  void slotValueChanged(double f);

private:
  KDoubleNumInput *valueBox;
  bool isUnitEnabled;
  KComboBox *mUnitCombo;
  MeasurementUnit unit;
  double ptMinVal, ptMaxVal; // the minimal and maximal value in points
  double mStep;

  static MeasurementUnit defaultUnit;
};
};
using namespace Kontour;

#endif
