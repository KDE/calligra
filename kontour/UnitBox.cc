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

#include "UnitBox.h"

#include <qcombobox.h>
#include <qlayout.h>
#include <knuminput.h>

MeasurementUnit UnitBox::defaultUnit = UnitPoint;

UnitBox::UnitBox(QWidget *parent, const char *name):
QHBox(parent, name)
{
  valueBox = new KDoubleNumInput(this, "valueBox");
  ptMinVal = 1.0;
  ptMaxVal = 10.0;
  mStep = 1.0;
  valueBox->setPrecision(2);
  valueBox->setRange(ptMinVal, ptMaxVal, mStep);

  unitCombo = new QComboBox(this, "unitCombo");
  unitCombo->insertItem(unitToString(UnitPoint));
  unitCombo->insertItem(unitToString(UnitMillimeter));
  unitCombo->insertItem(unitToString(UnitInch));
  unitCombo->insertItem(unitToString(UnitPica));
  unitCombo->insertItem(unitToString(UnitCentimeter));
  unitCombo->insertItem(unitToString(UnitDidot));
  unitCombo->insertItem(unitToString(UnitCicero));
  unitCombo->setCurrentItem(int(unit = defaultUnit));
  connect(unitCombo, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));
  connect(valueBox, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged(double)));
  isUnitEnabled = true;
}

UnitBox::~UnitBox()
{
}

void UnitBox::setPrecision(int prec)
{
  valueBox->setPrecision(prec);
}

double UnitBox::getValue()
{
  if(isUnitEnabled)
    return cvtUnitToPt(unit, valueBox->value());
  else
    return valueBox->value();
}

void UnitBox::setValue(double value)
{
  if(isUnitEnabled)
    valueBox->setValue(cvtPtToUnit(unit, value));
  else
    valueBox->setValue(value);
}

void UnitBox::setStep(double step)
{
  if(mStep != step)
  {
    valueBox->setRange(ptMinVal, ptMaxVal, step, false);
    mStep = step;
  }
}

double UnitBox::getStep() const
{
  return mStep;
}

void UnitBox::setRange(double minVal, double maxVal)
{
  ptMinVal = minVal;
  ptMaxVal = maxVal;
  valueBox->setRange(cvtPtToUnit(unit, minVal), cvtPtToUnit(unit, maxVal), mStep, false);
}

void UnitBox::getRange(double &minVal, double &maxVal)
{
  minVal = ptMinVal;
  maxVal = ptMaxVal;
}

void UnitBox::unitChanged(int id)
{
  MeasurementUnit newUnit = (MeasurementUnit)id;
  double ptValue = getValue();
  unit = newUnit;
  valueBox->setRange(cvtPtToUnit(unit, ptMinVal), cvtPtToUnit(unit, ptMaxVal), mStep, false);
  setValue(ptValue);
}

void UnitBox::setDefaultMeasurementUnit(MeasurementUnit unit)
{
  defaultUnit = unit;
}

void UnitBox::slotValueChanged(double f)
{
  /* convert the value according current unit */
  if(isUnitEnabled)
  {
    double val = cvtUnitToPt(unit, static_cast<double>(f));
    emit valueChanged(val);
  }
  else
    emit valueChanged(static_cast<double>(f));
}

void UnitBox::enableUnits(bool flag)
{
  unitCombo->setEnabled(flag);
  isUnitEnabled = flag;
}

#include "UnitBox.moc"
