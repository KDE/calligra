/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <kapp.h>
#include <klocale.h>
#include <kseparator.h>
#include <kbuttonbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include "OptionDialog.h"
#include "OptionDialog.moc"
#include "PStateManager.h"

OptionDialog::OptionDialog (QWidget* parent, const char* name) : 
    QTabDialog (parent, name, true) {
  QWidget* widget;

  setCaption (i18n ("Option"));

  widget = createGeneralWidget (this);
  addTab (widget, i18n ("General"));

  widget = createEditWidget (this);
  addTab (widget, i18n ("Edit"));

  setOkButton (i18n ("OK"));
  setCancelButton (i18n ("Cancel"));

  adjustSize ();
 
  setMinimumSize (300, 310);
  setMaximumSize (300, 310);
}

QWidget* OptionDialog::createGeneralWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;

  w = new QWidget (parent);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Unit:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (10, 20);

  unit = new QComboBox (w);
  unit->insertItem (unitToString (UnitPoint));
  unit->insertItem (unitToString (UnitMillimeter));
  unit->insertItem (unitToString (UnitInch));
  unit->insertItem (unitToString (UnitPica));
  unit->insertItem (unitToString (UnitCentimeter));
  unit->insertItem (unitToString (UnitDidot));
  unit->insertItem (unitToString (UnitCicero));
  unit->move (80, 20);

  unit->setCurrentItem ((int) 
			PStateManager::instance ()->defaultMeasurementUnit ());

  return w;
}

QWidget* OptionDialog::createEditWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;
  QGroupBox* box;
  w = new QWidget (parent);

  box = new QGroupBox (w);
  box->setTitle (i18n ("Duplicate Offset"));
  box->move (20, 15);

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Horizontal:"));
  label->move (20, 20);

  horiz = new UnitBox (box);
  horiz->setRange (-1000.0, 1000.0);
  horiz->setStep (0.1);
  horiz->setEditable (true);
  horiz->move (90, 20);

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Vertical:"));
  label->move (20, 50);

  vert = new UnitBox (box);
  vert->setRange (-1000.0, 1000.0);
  vert->setStep (0.1);
  vert->setEditable (true);
  vert->move (90, 50);
  box->adjustSize ();

  box = new QGroupBox (w);
  box->setTitle (i18n ("Step Distance"));
  box->move (20, 120);

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Small step:"));
  label->move (20, 20);

  smallStep = new UnitBox (box);
  smallStep->setRange (-1000.0, 1000.0);
  smallStep->setStep (0.1);
  smallStep->setEditable (true);
  smallStep->move (90, 20);

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Big step:"));
  label->move (20, 50);

  bigStep = new UnitBox (box);
  bigStep->setRange (-1000.0, 1000.0);
  bigStep->setStep (0.1);
  bigStep->setEditable (true);
  bigStep->move (90, 50);

  PStateManager *psm = PStateManager::instance ();
  horiz->setValue (psm->duplicateXOffset ());
  vert->setValue (psm->duplicateYOffset ());
  smallStep->setValue (psm->smallStepSize ());
  bigStep->setValue (psm->bigStepSize ());
  box->adjustSize ();

  return w;
}

void OptionDialog::applyPressed () {
  accept ();
}

void OptionDialog::helpPressed () {
}

int OptionDialog::setup () {
  int res;
  OptionDialog dialog (0L, "Options");
  
  res = dialog.exec ();
  if (res == QDialog::Accepted) {
    int selection = dialog.unit->currentItem ();
    PStateManager* psm = PStateManager::instance ();
    switch (selection) {
    case 0:
      psm->setDefaultMeasurementUnit (UnitPoint);
      break;
    case 1:
      psm->setDefaultMeasurementUnit (UnitMillimeter);
      break;
    case 2:
      psm->setDefaultMeasurementUnit (UnitInch);
      break;
    case 3:
      psm->setDefaultMeasurementUnit (UnitPica);
      break;
    case 4:
      psm->setDefaultMeasurementUnit (UnitCentimeter);
      break;
    case 5:
      psm->setDefaultMeasurementUnit (UnitDidot);
      break;
    case 6:
      psm->setDefaultMeasurementUnit (UnitCicero);
      break;
    default:
      break;
    }
    psm->setStepSizes (dialog.smallStep->getValue (), 
		       dialog.bigStep->getValue ());
    psm->setDuplicateOffsets (dialog.horiz->getValue (), 
			      dialog.vert->getValue ());
  }
  return res;
}
  
