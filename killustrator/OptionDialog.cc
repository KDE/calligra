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

#include <OptionDialog.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qhbox.h>
#include <qcombobox.h>

#include <klocale.h>

#include <UnitBox.h>
#include <PStateManager.h>

OptionDialog::OptionDialog (QWidget* parent, const char* name) :
    KDialogBase(KDialogBase::TreeList, i18n("Option"),
                KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,
                parent, name, true) {
    createGeneralWidget(addPage(i18n("General")));
    createEditWidget(addPage(i18n("Edit")));
}

void OptionDialog::createGeneralWidget (QWidget* parent) {

    QGridLayout *layout=new QGridLayout(parent, 2, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QLabel *label = new QLabel(i18n("Unit:"), parent);
    layout->addWidget(label, 0, 0);

    unit = new QComboBox (parent);
    unit->insertItem (unitToString (UnitPoint));
    unit->insertItem (unitToString (UnitMillimeter));
    unit->insertItem (unitToString (UnitInch));
    unit->insertItem (unitToString (UnitPica));
    unit->insertItem (unitToString (UnitCentimeter));
    unit->insertItem (unitToString (UnitDidot));
    unit->insertItem (unitToString (UnitCicero));
    layout->addWidget(unit, 0, 1);
    layout->setRowStretch(1, 1);

    unit->setCurrentItem ((int)
                          PStateManager::instance ()->defaultMeasurementUnit ());
}

void OptionDialog::createEditWidget (QWidget* parent) {

    QBoxLayout *layout=new QVBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QGroupBox *box = new QGroupBox(i18n("Duplicate Offset"), parent);
    layout->addWidget(box);

    QBoxLayout *vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(box->fontMetrics().height()/2);
    QGridLayout *grid=new QGridLayout(vboxlayout, 2, 2);
    QLabel *label = new QLabel(i18n("Horizontal:"), box);
    grid->addWidget(label, 0, 0);

    horiz = new UnitBox(box);
    horiz->setRange (-1000.0, 1000.0);
    horiz->setStep (0.1);
    horiz->setEditable (true);
    grid->addWidget(horiz, 0, 1);

    label = new QLabel(i18n("Vertical:"), box);
    grid->addWidget(label, 1, 0);

    vert = new UnitBox(box);
    vert->setRange (-1000.0, 1000.0);
    vert->setStep (0.1);
    vert->setEditable (true);
    grid->addWidget(vert, 1, 1);

    box = new QGroupBox(i18n("Step Distance"), parent);
    layout->addWidget(box);

    vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(box->fontMetrics().height()/2);
    grid=new QGridLayout(vboxlayout, 2, 2);
    label = new QLabel(i18n("Small step:"), box);
    grid->addWidget(label, 0, 0);

    smallStep = new UnitBox(box);
    smallStep->setRange (-1000.0, 1000.0);
    smallStep->setStep (0.1);
    smallStep->setEditable (true);
    grid->addWidget(smallStep, 0, 1);

    label = new QLabel(i18n("Big step:"), box);
    grid->addWidget(label, 1, 0);

    bigStep = new UnitBox(box);
    bigStep->setRange (-1000.0, 1000.0);
    bigStep->setStep (0.1);
    bigStep->setEditable (true);
    grid->addWidget(bigStep, 1, 1);

    PStateManager *psm = PStateManager::instance ();
    horiz->setValue (psm->duplicateXOffset ());
    vert->setValue (psm->duplicateYOffset ());
    smallStep->setValue (psm->smallStepSize ());
    bigStep->setValue (psm->bigStepSize ());
}

int OptionDialog::setup ()
{

   OptionDialog dialog (0L, "Options");

   int res=dialog.exec();
   if(res == QDialog::Accepted)
   {
      int selection = dialog.unit->currentItem ();
      PStateManager* psm = PStateManager::instance ();
      switch (selection)
      {
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
      psm->setStepSizes (dialog.smallStep->getValue (),dialog.bigStep->getValue ());
      psm->setDuplicateOffsets (dialog.horiz->getValue (),dialog.vert->getValue ());
   }
   return res;
}

#include <OptionDialog.moc>
