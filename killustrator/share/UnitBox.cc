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

#include <qcombobox.h>

#include "UnitBox.h"
#include "UnitBox.moc"

MeasurementUnit UnitBox::defaultUnit = UnitPoint;

UnitBox::UnitBox (QWidget* parent, const char* name) : QWidget (parent, name) {
  valueBox = new FloatSpinBox (this, "valueBox");
//  valueBox->setGeometry (0, 0, 80, valueBox->height ());
  valueBox->setGeometry (0, 0, 80, 24);

  unitCombo = new QComboBox (this, "unitCombo");
  unitCombo->insertItem (unitToString (UnitPoint));
  unitCombo->insertItem (unitToString (UnitMillimeter));
  unitCombo->insertItem (unitToString (UnitInch));
  unitCombo->insertItem (unitToString (UnitPica));
  unitCombo->insertItem (unitToString (UnitCentimeter));
  unitCombo->insertItem (unitToString (UnitDidot));
  unitCombo->insertItem (unitToString (UnitCicero));
  unitCombo->setCurrentItem (int (unit = defaultUnit));
  unitCombo->setGeometry (valueBox->width () + 5, 0, 50, valueBox->height ());
  connect (unitCombo, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));
  connect (valueBox, SIGNAL (valueChanged (float)), 
	   this, SLOT(slotValueChange (float)));
  setMinimumSize (valueBox->width () + unitCombo->width () + 5,
	          valueBox->height ());
  isUnitEnabled = true;
}
 
UnitBox::~UnitBox () {
}
  
void UnitBox::setFormatString (const char* fmt) {
  valueBox->setFormatString (fmt);
}

float UnitBox::getValue () {
  if(isUnitEnabled)
     return cvtUnitToPt (unit, valueBox->getValue ());
  else
     return valueBox->getValue ();
}

void UnitBox::setValue (float value) {
  if(isUnitEnabled)
     valueBox->setValue (cvtPtToUnit (unit, value));
  else
     valueBox->setValue (value);
}
  
void UnitBox::setStep (float step) {
  valueBox->setStep (step);
}

float UnitBox::getStep () const {
  return valueBox->getStep ();
}

void UnitBox::setRange (float minVal, float maxVal) {
  ptMinVal = minVal;
  ptMaxVal = maxVal;
  valueBox->setRange (cvtPtToUnit (unit, minVal), cvtPtToUnit (unit, maxVal));
}
 
void UnitBox::getRange (float& minVal, float& maxVal) {
  minVal = ptMinVal;
  maxVal = ptMaxVal;  
}

void UnitBox::setEditable (bool flag) {
  //  valueBox->setEditable (flag);
}

void UnitBox::unitChanged (int id) {
  MeasurementUnit newUnit = (MeasurementUnit) id;
  float ptValue = getValue ();
  unit = newUnit;
  valueBox->setRange (cvtPtToUnit (unit, ptMinVal), cvtPtToUnit (unit, ptMaxVal));
  setValue (ptValue);
}

void UnitBox::setDefaultMeasurementUnit (MeasurementUnit unit) {
  defaultUnit = unit;
}

void UnitBox::slotValueChange (float f) {
  // convert the value according current unit
  if(isUnitEnabled){
    float val = cvtUnitToPt (unit, f);
    emit valueChanged (val);
  }
  else 
    emit valueChanged (f);
}

void UnitBox::enableUnits (bool flag) {
  unitCombo->setEnabled (flag);
  isUnitEnabled = flag;
}
