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

#include "version.h"
#include <stdlib.h>
#include <stdio.h>
#include "FloatSpinBox.h"
#include "FloatSpinBox.moc"

#if QT_VERSION >= 199
#define TEXT(s) s.ascii ()
#else
#define TEXT(s) s
#endif

FloatSpinBox::FloatSpinBox (QWidget* parent, const char* name, 
			    int) : QSpinBox (parent, name/*, align*/) {
  minval = 1;
  maxval = 10;
  step = 1;
  value_ = 0.0;

  format = "%3.2f";
  setValue ((float) 1);
  setSteps (10, 10);
  val = new QDoubleValidator (minval, maxval, 2, this);
  setValidator (val);
  //  connect (this, SIGNAL (valueIncreased ()), SLOT(slotIncrease ()));
  //  connect (this, SIGNAL (valueDecreased ()), SLOT(slotDecrease ()));
  //  connect (this, SIGNAL (valueChanged (int)), this, SLOT(reportChanges ()));
  /*connect ((QObject *) editor (), SIGNAL (returnPressed ()), this,
	   SLOT (slotValueChange ()));*/
}

FloatSpinBox::~FloatSpinBox () {
}

float FloatSpinBox::getValue () {
  //  return atof (TEXT (text ()));
  return value_;
}

void FloatSpinBox::setFormatString (const char* fmt) {
  format = fmt;
}

void FloatSpinBox::setValue (float value) {
  char buf[20];
  if (minval <= value && value <= maxval) {
    sprintf (buf, (const char *) format, value);
    //    KSpinBox::setValue (buf);
    value_ = value;
    QSpinBox::setValue (int (value * 100.0));
  }
}

void FloatSpinBox::setStep (float s) {
  step = s;
  setSteps (step * 10.0, step * 10.0);
}

float FloatSpinBox::getStep () const {
  return step;
}

void FloatSpinBox::setRange (float minVal, float maxVal) {
  if (minVal < maxVal) {
    minval = minVal;
    maxval = maxVal;
    QRangeControl::setRange (int (minVal * 100.0), int (maxVal * 100.0));
    val->setRange (minVal, maxVal, 2);
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

void FloatSpinBox::valueChange () {
    updateDisplay();
    emit valueChanged( (float) getValue() );
}



/*void FloatSpinBox::reportChanges () {
  printf ("SpinBox::changed\n");
}*/

int FloatSpinBox::mapTextToValue (bool *ok) {
  const char *txt = TEXT (text ());
  value_ = atof (txt);
  *ok = true;
  return int (value_ * 100.0);
}

QString FloatSpinBox::mapValueToText (int v) {
  float f = float (v) / 100.0;
  QString buf;
  buf.sprintf ((const char *) format, f);
  return buf;
}
