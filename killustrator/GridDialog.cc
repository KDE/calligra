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

#include <GridDialog.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qcolor.h>

#include <klocale.h>
#include <kcolorbtn.h>
#include <kdebug.h>

#include <Canvas.h>
#include <UnitBox.h>

GridDialog::GridDialog (QWidget* parent, const char* name) :
    KDialogBase(parent, name, true, i18n ("Grid"),
                KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok, true) {
    createGridWidget(makeMainWidget());
}

void GridDialog::createGridWidget (QWidget* parent) {

    QGridLayout *layout=new QGridLayout(parent, 3, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QGroupBox *box=new QGroupBox(i18n("Distance"), parent);
    layout->addMultiCellWidget(box, 0, 0, 0, 1);

    QBoxLayout *vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(box->fontMetrics().height()/2);
    QGridLayout *grid=new QGridLayout(vboxlayout, 2, 2);
    QLabel* label = new QLabel(i18n("Horizontally"), box);
    grid->addWidget(label, 0, 0);

    hspinbox = new UnitBox(box);
    hspinbox->setFormatString ("%-3.3f");
    hspinbox->setEditable (true);
    hspinbox->setRange (0, 1000);
    grid->addWidget(hspinbox, 0, 1);

    label=new QLabel(i18n("Vertically"), box);
    grid->addWidget(label, 1, 0);

    vspinbox = new UnitBox (box);
    vspinbox->setFormatString ("%-3.3f");
    vspinbox->setEditable (true);
    vspinbox->setRange (0, 1000);
    grid->addWidget(vspinbox, 1, 1);

    gbutton = new QCheckBox(i18n("Snap To Grid"), parent);
    layout->addWidget(gbutton, 1, 0);

    sbutton = new QCheckBox(i18n("Show Grid"), parent);
    layout->addWidget(sbutton, 1, 1);

    cbutton = new KColorButton(parent);
    QLabel* clabel = new QLabel(i18n("Grid Color"), this);
    layout->addWidget(cbutton, 2, 1);
    layout->addWidget(clabel, 2, 0);
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

QColor GridDialog::gridColor()
 {
  return cbutton->color();
 }

void GridDialog::setShowGridOn (bool flag) {
  sbutton->setChecked (flag);
}

void GridDialog::setSnapToGridOn (bool flag)
{
   kdDebug(38000)<<"GridDialog::setSnapToGridOn: flag "<<int(flag)<<endl;
  gbutton->setChecked (flag);
}

void GridDialog::setDistances (float h, float v)
{
   hspinbox->setValue (h);
   vspinbox->setValue (v);
}

void GridDialog::setGridColor(QColor color)
{
   cbutton->setColor(color);
}

void GridDialog::setupGrid (Canvas* canvas)
{
   GridDialog dialog (0L, "Grid");
/*   dialog.setShowGridOn (canvas->showGrid ());
   dialog.setSnapToGridOn (canvas->snapToGrid ());
   dialog.setDistances ((float) canvas->getHorizGridDistance (),
                        (float) canvas->getVertGridDistance ());
   dialog.setGridColor (canvas->gridColor());*/

   if (dialog.exec() == Accepted)
   {
/*      canvas->setGridDistance (dialog.horizontalDistance (),
                               dialog.verticalDistance ());
      canvas->showGrid (dialog.showGrid ());
      canvas->snapToGrid (dialog.snapToGrid ());
      canvas->setGridColor (dialog.gridColor());
      canvas->repaint ();
      canvas->saveGridProperties();*/
   }
}

#include <GridDialog.moc>
