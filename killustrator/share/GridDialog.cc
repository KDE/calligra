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

#include "GridDialog.h"
#include "GridDialog.moc"

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

GridDialog::GridDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, true) {
  QPushButton* button;
  QWidget* widget;

  setCaption (i18n ("Grid"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  widget = createGridWidget (this);
  vl->addWidget (widget);

  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (i18n (OK));
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

QWidget* GridDialog::createGridWidget (QWidget* parent) {
  QWidget* w;
  QGroupBox* box;

  w = new QWidget (parent);

  box = new QGroupBox (w);
  box->setTitle (i18n ("Distance"));
  box->setGeometry (10, 10, 260, 90);

  QLabel* label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Horizontally"));
  label->move (20, 20);

  hspinbox = new UnitBox (box);
  hspinbox->setFormatString ("%-3.3f");
  hspinbox->setEditable (true);
  hspinbox->setRange (0, 1000);
  hspinbox->move (100, 20);
 
  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Vertically"));
  label->move (20, 55);

  vspinbox = new UnitBox (box);
  vspinbox->setFormatString ("%-3.3f");
  vspinbox->setEditable (true);
  vspinbox->setRange (0, 1000);
  vspinbox->move (100, 55);

  gbutton = new QCheckBox (w);
  gbutton->setText (i18n ("Snap To Grid"));
  gbutton->setGeometry (10, 110, 200, 15);

  sbutton = new QCheckBox (w);
  sbutton->setText (i18n ("Show Grid"));
  sbutton->setGeometry (10, 130, 200, 15);

  w->setMinimumSize (230, 160);
  w->setMaximumSize (330, 160);
  return w;
}

void GridDialog::helpPressed () {
}

float GridDialog::horizontalDistance () {
  return hspinbox->getValue ();
}

float GridDialog::verticalDistance () {
  return vspinbox->getValue ();
}

bool GridDialog::showGrid () {
  return sbutton->isOn ();
}

bool GridDialog::snapToGrid () {
  return gbutton->isOn ();
}

void GridDialog::setShowGridOn (bool flag) {
  sbutton->setChecked (flag);
}

void GridDialog::setSnapToGridOn (bool flag) {
  gbutton->setChecked (flag);
}

void GridDialog::setDistances (float h, float v) {
  hspinbox->setValue (h);
  vspinbox->setValue (h);
}

void GridDialog::setupGrid (Canvas* canvas) {
  GridDialog dialog (0L, "Grid");
  dialog.setShowGridOn (canvas->showGrid ());
  dialog.setSnapToGridOn (canvas->snapToGrid ());
  dialog.setDistances ((float) canvas->getHorizGridDistance (),
		       (float) canvas->getVertGridDistance ());

  int result = dialog.exec ();
  if (result == Accepted) {
    canvas->setGridDistance ((int) dialog.horizontalDistance (),
			     (int) dialog.verticalDistance ());
    canvas->showGrid (dialog.showGrid ());
    canvas->snapToGrid (dialog.snapToGrid ());
  }
}
  
