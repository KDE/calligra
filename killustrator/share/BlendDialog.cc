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

#include "BlendDialog.h"
#include "BlendDialog.moc"

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

BlendDialog::BlendDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, true) {
  QPushButton* button;
  QWidget* widget;

  setCaption (i18n ("Blend"));

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
 
  setMinimumSize (280, 220);
  setMaximumSize (290, 220);
}

QWidget* BlendDialog::createWidget (QWidget* parent) {
  QWidget* w;
  QGroupBox* box;

  w = new QWidget (parent);

  box = new QGroupBox (w);
  box->setTitle (i18n ("Blend Objects"));
  box->setGeometry (10, 10, 240, 80);

  QLabel* label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Steps"));
  label->move (20, 20);

  spinbox = new KNumericSpinBox (box);
  spinbox->setValue (10);
  spinbox->setStep (1);
  spinbox->setRange (0, 1000);
  spinbox->move (100, 20);
 
  w->setMinimumSize (230, 140);
  w->setMaximumSize (330, 140);
  return w;
}

void BlendDialog::helpPressed () {
}

int BlendDialog::getNumOfSteps () {
  BlendDialog dialog (0L, "Blend");

  int result = dialog.exec ();
  if (result == Accepted) 
    return dialog.spinbox->getValue ();
  else
    return 0;
}
  
