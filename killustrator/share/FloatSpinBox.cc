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

#include <stdlib.h>
#include <stdio.h>
#include "FloatSpinBox.h"
#include "FloatSpinBox.moc"

FloatSpinBox::FloatSpinBox (QWidget* parent, const char* name, 
			    int align) : KSpinBox (parent, name, align) {
  minval = 1;
  maxval = 10;
  step = 1;

  format = "%3.2f";
  setValue ((float) 1);

  connect (this, SIGNAL (valueIncreased ()), SLOT(slotIncrease ()));
  connect (this, SIGNAL (valueDecreased ()), SLOT(slotDecrease ()));
  connect (_edit, SIGNAL (returnPressed ()), this,
	   SLOT (slotValueChange ()));
}

FloatSpinBox::~FloatSpinBox () {
}

float FloatSpinBox::getValue () {
  return atof (KSpinBox::getValue ());
}

void FloatSpinBox::setFormatString (const char* fmt) {
  format = fmt;
}

void FloatSpinBox::setValue (float value) {
  char buf[20];
  if (minval <= value && value <= maxval) {
    sprintf (buf, (const char *) format, value);
    KSpinBox::setValue (buf);
  }
}

void FloatSpinBox::setStep (float val) {
  step = val;
}

float FloatSpinBox::getStep () const {
  return step;
}

void FloatSpinBox::setRange (float minVal, float maxVal) {
  if (minVal < maxVal) {
    minval = minVal;
    maxval = maxVal;
  }
}

void FloatSpinBox::getRange (float& minVal, float& maxVal) {
  minVal = minval; maxVal = maxval;
}

void FloatSpinBox::slotIncrease () {
  setValue ((float) getValue () + step);
}

void FloatSpinBox::slotDecrease () {
  setValue ((float) getValue () - step);
}

void FloatSpinBox::slotValueChange () {
  emit valueChanged (getValue ());
}
