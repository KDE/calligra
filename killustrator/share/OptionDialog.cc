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

#include <kapp.h>
#include <klocale.h>
#include <kseparator.h>
#include <kbuttonbox.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlayout.h>
#include "OptionDialog.h"
#include "OptionDialog.moc"
#include "PStateManager.h"

OptionDialog::OptionDialog (QWidget* parent, const char* name) : 
    QTabDialog (parent, name, true) {
  QWidget* widget;

  setCaption (i18n ("Option"));

  widget = createGeneralWidget (this);

  addTab (widget, i18n ("General"));
  setOkButton (i18n ("Ok"));
  setCancelButton (i18n ("Cancel"));

  adjustSize ();
 
  setMinimumSize (300, 250);
  setMaximumSize (300, 250);

  unit->setCurrentItem ((int) 
			PStateManager::instance ()->defaultMeasurementUnit ());

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

  unit = new KCombo (w);
  unit->insertItem ("pt");
  unit->insertItem ("mm");
  unit->insertItem ("inch");
  unit->move (80, 20);
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
    default:
      break;
    }
  }
  return res;
}
  
