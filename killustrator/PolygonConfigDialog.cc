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

#include <PolygonConfigDialog.h>

#include <qradiobutton.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qvbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>

#include <PolygonTool.h>
#include <PolygonPreview.h>

PolygonConfigDialog::PolygonConfigDialog (QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n("Setup Polygon Tool"),
                KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok, true) {
    createWidget(makeMainWidget());
}

void PolygonConfigDialog::createWidget(QWidget* parent) {

    QGridLayout *layout=new QGridLayout(parent, 4, 3, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QButtonGroup *group = new QVButtonGroup(parent);
    layout->addMultiCellWidget(group, 0, 0, 0, 1);

    convexButton = new QRadioButton(i18n("Polygon"), group);
    connect (convexButton, SIGNAL(clicked ()), this,
             SLOT(slotConvexPolygon ()));

    concaveButton = new QRadioButton(i18n("Concave Polygon"), group);
    connect (concaveButton, SIGNAL(clicked ()), this,
             SLOT(slotConcavePolygon ()));

    QLabel* label = new QLabel(i18n("Corners:"), parent);
    layout->addWidget(label, 1, 0);

    spinbox = new QSpinBox(3, 100, 1, parent);
    layout->addWidget(spinbox, 1, 1);

    sharpnessLabel = new QLabel(i18n("Sharpness:"), parent);
    layout->addWidget(sharpnessLabel, 2, 0);

    slider = new QSlider(0, 100, 50, 0, QSlider::Horizontal, parent);
    slider->setLineStep(10);
    layout->addWidget(slider, 2, 1);

    preview = new PolygonPreview(parent);
    layout->addMultiCellWidget(preview, 0, 3, 2, 2);
    layout->setColStretch(2, 1);
    layout->setRowStretch(3, 1);

    connect (slider, SIGNAL(sliderMoved (int)), preview,
             SLOT(slotSharpness (int)));
    connect (concaveButton, SIGNAL(clicked ()), preview,
             SLOT(slotConcavePolygon ()));
    connect (convexButton, SIGNAL(clicked ()), preview,
             SLOT(slotConvexPolygon ()));
    connect (spinbox, SIGNAL(valueChanged (int)), preview,
             SLOT(setNumOfCorners (int)));
}

unsigned int PolygonConfigDialog::numCorners () {
    return spinbox->value ();
}

void PolygonConfigDialog::setNumCorners (unsigned int num) {
    spinbox->setValue (num);
    preview->setNumOfCorners (spinbox->value ());
}

unsigned int PolygonConfigDialog::sharpness () {
    return slider->value ();
}

void PolygonConfigDialog::setSharpness (unsigned int value) {
    slider->setValue (value);
    preview->slotSharpness(value);
}

bool PolygonConfigDialog::concavePolygon () {
    return concaveButton->isChecked ();
}

void PolygonConfigDialog::setConcavePolygon (bool flag) {
    concaveButton->setChecked (flag);
    convexButton->setChecked (!flag);
    slider->setEnabled (flag);
    sharpnessLabel->setEnabled(flag);
    if(flag)
        preview->slotConcavePolygon();
    else
        preview->slotConvexPolygon();
}

void PolygonConfigDialog::slotConcavePolygon () {
    slider->setEnabled (true);
    sharpnessLabel->setEnabled(true);
}

void PolygonConfigDialog::slotConvexPolygon () {
    slider->setEnabled (false);
    sharpnessLabel->setEnabled(false);
}

void PolygonConfigDialog::setupTool (PolygonTool* tool) {

    PolygonConfigDialog dialog;
    dialog.setNumCorners (tool->numCorners ());
    dialog.setSharpness (tool->sharpness ());
    dialog.setConcavePolygon (tool->concavePolygon ());

    if(dialog.exec() == Accepted) {
        tool->setNumCorners (dialog.numCorners ());
        tool->setSharpness (dialog.sharpness ());
        tool->setConcavePolygon (dialog.concavePolygon ());
        tool->writeOutConfig();
    }
}

#include <PolygonConfigDialog.moc>
