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

#include <UnitBox.h>

#include <qcombobox.h>
#include <qlayout.h>
#include <knuminput.h>

MeasurementUnit UnitBox::defaultUnit = UnitPoint;

UnitBox::UnitBox (QWidget* parent, const char* name) : QHBox(parent, name) {

    valueBox = new KDoubleNumInput(this, "valueBox");
    ptMinVal=1.0;
    ptMaxVal=10.0;
    m_step=1.0;
    valueBox->setFormat("%3.2f");
    valueBox->setRange(ptMinVal, ptMaxVal, m_step);

    unitCombo = new QComboBox (this, "unitCombo");
    unitCombo->insertItem (unitToString (UnitPoint));
    unitCombo->insertItem (unitToString (UnitMillimeter));
    unitCombo->insertItem (unitToString (UnitInch));
    unitCombo->insertItem (unitToString (UnitPica));
    unitCombo->insertItem (unitToString (UnitCentimeter));
    unitCombo->insertItem (unitToString (UnitDidot));
    unitCombo->insertItem (unitToString (UnitCicero));
    unitCombo->setCurrentItem (int (unit = defaultUnit));
    connect (unitCombo, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));
    connect (valueBox, SIGNAL (valueChanged(double)),
             this, SLOT(slotValueChanged(double)));
    isUnitEnabled = true;
}

UnitBox::~UnitBox () {
}

void UnitBox::setFormatString (const char* fmt) {
    valueBox->setFormat(fmt);
}

float UnitBox::getValue () {
    if(isUnitEnabled)
        return cvtUnitToPt (unit, valueBox->value());
    else
        return valueBox->value();
}

void UnitBox::setValue (float value) {
    if(isUnitEnabled)
        valueBox->setValue (cvtPtToUnit (unit, value));
    else
        valueBox->setValue (value);
}

void UnitBox::setStep (float step) {
    if(m_step!=step) {
        valueBox->setRange(ptMinVal, ptMaxVal, step, false);
        m_step=step;
    }
}

float UnitBox::getStep () const {
    return m_step;
}

void UnitBox::setRange (float minVal, float maxVal) {
    ptMinVal = minVal;
    ptMaxVal = maxVal;
    valueBox->setRange(cvtPtToUnit(unit, minVal), cvtPtToUnit(unit, maxVal), m_step, false);
}

void UnitBox::getRange (float& minVal, float& maxVal) {
    minVal = ptMinVal;
    maxVal = ptMaxVal;
}

void UnitBox::setEditable (bool /*flag*/) {
    //  valueBox->setEditable (flag);
}

void UnitBox::unitChanged (int id) {
    MeasurementUnit newUnit = (MeasurementUnit) id;
    float ptValue = getValue ();
    unit = newUnit;
    valueBox->setRange(cvtPtToUnit (unit, ptMinVal), cvtPtToUnit (unit, ptMaxVal), m_step, false);
    setValue(ptValue);
}

void UnitBox::setDefaultMeasurementUnit (MeasurementUnit unit) {
    defaultUnit = unit;
}

void UnitBox::slotValueChanged(double f) {
    // convert the value according current unit
    if(isUnitEnabled){
        float val = cvtUnitToPt (unit, static_cast<float>(f));
        emit valueChanged (val);
    }
    else
        emit valueChanged(static_cast<float>(f));
}

void UnitBox::enableUnits (bool flag) {
    unitCombo->setEnabled (flag);
    isUnitEnabled = flag;
}

#include <UnitBox.moc>
