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

#include "PolygonConfigDialog.h"
#include "PolygonConfigDialog.moc"

#include <stdio.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>

#include <qpushbt.h>
#include <qbttngrp.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgrpbox.h>
#include <qframe.h>

PolygonConfigDialog::PolygonConfigDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, true) {
  QPushButton* button;
  QWidget* widget;

  setCaption (i18n ("Setup Polygon Tool"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  widget = createWidget (this);
  vl->addWidget (widget);

  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (i18n ("OK"));
  connect (button, SIGNAL (clicked ()), SLOT (accept ()));
  button = bbox->addButton (i18n ("Cancel"));
  connect (button, SIGNAL (clicked ()), SLOT (reject ()));
  bbox->addStretch (1);
  button = bbox->addButton (i18n ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
  bbox->layout ();
  bbox->setMinimumSize (bbox->sizeHint ());

  vl->addWidget (bbox);
  vl->activate ();
 
  setMinimumSize (410, 250);
  setMaximumSize (410, 250);
}

QWidget* PolygonConfigDialog::createWidget (QWidget* parent) {
  QWidget* w;
  QButtonGroup* group;

  w = new QWidget (parent);

  group = new QButtonGroup (w);
  group->setGeometry (10, 10, 180, 70);
  
  convexButton = new QRadioButton (group);
  convexButton->setText (i18n ("Polygon"));
  convexButton->setGeometry (10, 10, 150, 15);
  connect (convexButton, SIGNAL(clicked ()), this, 
	   SLOT(slotConvexPolygon ()));

  concaveButton = new QRadioButton (group);
  concaveButton->setText (i18n ("Concave Polygon"));
  concaveButton->setGeometry (10, 40, 150, 15);
  connect (concaveButton, SIGNAL(clicked ()), this, 
	   SLOT(slotConcavePolygon ()));

  QLabel* label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Corners:"));
  label->move (10, 90);

  spinbox = new KNumericSpinBox (w);
  spinbox->setRange (3, 100);
  spinbox->setStep (1);
  spinbox->move (90, 90);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Sharpness:"));
  label->move (10, 130);

  slider = new KSlider (KSlider::Horizontal, w);
  slider->setRange (0, 100);
  slider->setSteps (10, 50);
  slider->move (90, 130);

  QFrame* frame = new QFrame (w);
  frame->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  frame->setLineWidth (2);

  preview = new PolygonPreview (frame);
  preview->move (2, 2);
  frame->setGeometry (210, 10, preview->width () + 4, 
		      preview->height () + 4);

  connect (slider, SIGNAL(sliderMoved (int)), preview, 
	   SLOT(slotSharpness (int)));
  connect (concaveButton, SIGNAL(clicked ()), preview, 
	   SLOT(slotConcavePolygon ()));
  connect (convexButton, SIGNAL(clicked ()), preview, 
	   SLOT(slotConvexPolygon ()));
  connect (spinbox, SIGNAL(valueIncreased ()), preview, 
	   SLOT(increaseNumOfCorners ()));
  connect (spinbox, SIGNAL(valueDecreased ()), preview, 
	   SLOT(decreaseNumOfCorners ()));

  w->setMinimumSize (400, 170);
  w->setMaximumSize (400, 170);
  return w;
}

void PolygonConfigDialog::helpPressed () {
}

unsigned int PolygonConfigDialog::numCorners () {
  return spinbox->getValue ();
}

void PolygonConfigDialog::setNumCorners (unsigned int num) {
  spinbox->setValue (num);
}

unsigned int PolygonConfigDialog::sharpness () {
  return slider->value ();
}

void PolygonConfigDialog::setSharpness (unsigned int value) {
  slider->setValue (value);
}

bool PolygonConfigDialog::concavePolygon () {
  return concaveButton->isChecked ();
}

void PolygonConfigDialog::setConcavePolygon (bool flag) {
  concaveButton->setChecked (flag);
  convexButton->setChecked (! flag);
  slider->setEnabled (flag);
}

void PolygonConfigDialog::slotConcavePolygon () {
  slider->setEnabled (true);
}

void PolygonConfigDialog::slotConvexPolygon () {
  slider->setEnabled (false);
}

void PolygonConfigDialog::setupTool (PolygonTool* tool) {
  PolygonConfigDialog dialog;

  dialog.setNumCorners (tool->numCorners ());
  dialog.setSharpness (tool->sharpness ());
  dialog.setConcavePolygon (tool->concavePolygon ());

  int result = dialog.exec ();
  if (result == Accepted) {
    tool->setNumCorners (dialog.numCorners ());
    tool->setSharpness (dialog.sharpness ());
    tool->setConcavePolygon (dialog.concavePolygon ());
  }
}
  
