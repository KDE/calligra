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

#include <TransformationDialog.h>

#include <klocale.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kdebug.h>

#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qhbox.h>

#include <TranslateCmd.h>
#include <RotateCmd.h>
#include <ScaleCmd.h>
#include <InsertObjCmd.h>
#include <Handle.h> // for scaling directions
#include <GDocument.h>
#include "GPage.h"
#include <GObject.h>
#include <CommandHistory.h>
#include <UnitBox.h>
#include "KIllustrator_factory.h"



TransformationDialog::TransformationDialog (CommandHistory* cmdHist,
                                            QWidget* parent, const char* name) :
  KDialogBase(KDialogBase::Tabbed, i18n("Transform"), KDialogBase::Close,
              KDialogBase::Close, parent, name, false) {

    document = 0L;
    history = cmdHist;
    createPositionWidget(addPage(i18n("Position")));
    createDimensionWidget(addPage(i18n("Dimension")));
    createRotationWidget (addPage(i18n("Rotation")));
    createMirrorWidget(addPage(i18n("Mirror")));
}

void TransformationDialog::createPositionWidget (QWidget* parent) {

    QGridLayout *layout=new QGridLayout(parent, 5, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QLabel* label = new QLabel(i18n("Horizontal:"), parent);
    layout->addWidget(label, 0, 0);

    horizPosition = new UnitBox (parent);
    horizPosition->setRange (-1000.0, 1000.0);
    horizPosition->setStep (0.1);
    horizPosition->setEditable (true);
    layout->addWidget(horizPosition, 0, 1);

    label = new QLabel(i18n("Vertical:"), parent);
    layout->addWidget(label, 1, 0);

    vertPosition = new UnitBox(parent);
    vertPosition->setRange (-1000.0, 1000.0);
    vertPosition->setStep (0.1);
    vertPosition->setEditable (true);
    layout->addWidget(vertPosition, 1, 1);

    relativePosition = new QCheckBox(i18n("Relative Position"), parent);
    connect( relativePosition, SIGNAL(clicked()),
             this, SLOT(relativePositionSlot()) );
    layout->addMultiCellWidget(relativePosition, 2, 2, 0, 1);
    layout->setRowStretch(3, 1);

    applyBttn[0] = new QPushButton(i18n("Apply"), parent);
    connect (applyBttn[0], SIGNAL(clicked()), this, SLOT(applyPressed()));
    layout->addWidget(applyBttn[0], 4, 0);

    applyToDupBttn[0] = new QPushButton(i18n("Apply To Duplicate"), parent);
    connect (applyToDupBttn[0], SIGNAL(clicked()),
             this, SLOT(applyToDuplicatePressed()));
    layout->addWidget(applyToDupBttn[0], 4, 1);
}

void TransformationDialog::createDimensionWidget (QWidget* parent) {

    QGridLayout *layout=new QGridLayout(parent, 6, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QButtonGroup *group=new QHButtonGroup(parent);
    group->setFrameStyle (QFrame::NoFrame);
    group->setExclusive (true);
    layout->addMultiCellWidget(group, 0, 0, 0, 1);

    absolute = new QRadioButton(i18n("Absolute"), group);
    absolute->setChecked (true);
    connect (absolute, SIGNAL(clicked ()), this, SLOT(slotAbsScale ()));

    percent = new QRadioButton(i18n("Percentage"), group);
    connect (percent, SIGNAL(clicked ()), this, SLOT(slotPercentScale ()));

    QLabel *label = new QLabel(i18n("Horizontal:"), parent);
    layout->addWidget(label, 1, 0);

    horizDim = new UnitBox(parent);
    horizDim->setRange (-1000.0, 1000.0);
    horizDim->setStep (0.1);
    horizDim->setEditable (true);
    connect (horizDim, SIGNAL(valueChanged (float)),
             this, SLOT(updateProportionalDimension (float)));
    layout->addWidget(horizDim, 1, 1);

    label = new QLabel(i18n("Vertical:"), parent);
    layout->addWidget(label, 2, 0);

    vertDim = new UnitBox(parent);
    vertDim->setRange (-1000.0, 1000.0);
    vertDim->setStep (0.1);
    vertDim->setEditable (true);
    connect (vertDim, SIGNAL(valueChanged (float)),
             this, SLOT(updateProportionalDimension (float)));
    layout->addWidget(vertDim, 2, 1);

    proportional = new QCheckBox(i18n("Proportional"), parent);
    layout->addMultiCellWidget(proportional, 3, 3, 0, 1);
    layout->setRowStretch(4, 1);

    applyBttn[1] = new QPushButton(i18n("Apply"), parent);
    connect (applyBttn[1], SIGNAL(clicked()), this, SLOT(applyPressed()));
    layout->addWidget(applyBttn[1], 5, 0);

    applyToDupBttn[1] = new QPushButton(i18n("Apply To Duplicate"), parent);
    connect (applyToDupBttn[1], SIGNAL(clicked()),
             this, SLOT(applyToDuplicatePressed()));
    layout->addWidget(applyToDupBttn[1], 5, 1);
}

void TransformationDialog::createRotationWidget (QWidget* parent) {

    QGridLayout *layout=new QGridLayout(parent, 4, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QGroupBox *box = new QGroupBox(i18n("Center"), parent);
    layout->addMultiCellWidget(box, 0, 0, 0, 1);
    QBoxLayout *vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(fontMetrics().height()/2);
    QGridLayout *grid=new QGridLayout(vboxlayout, 3, 2);

    QLabel *label = new QLabel(i18n("Horizontal:"), box);
    grid->addWidget(label, 0, 0);

    horizRotCenter = new UnitBox(box);
    horizRotCenter->setRange (-1000.0, 1000.0);
    horizRotCenter->setStep (0.1);
    horizRotCenter->setEditable (true);
    grid->addWidget(horizRotCenter, 0, 1);

    label = new QLabel(i18n("Vertical:"), box);
    grid->addWidget(label, 1, 0);

    vertRotCenter = new UnitBox(box);
    vertRotCenter->setRange (-1000.0, 1000.0);
    vertRotCenter->setStep (0.1);
    vertRotCenter->setEditable (true);
    grid->addWidget(vertRotCenter, 1, 1);

    relativeRotCenter = new QCheckBox(i18n("Relative Position"), box);
    connect( relativeRotCenter, SIGNAL(clicked()),
             this, SLOT(relativeRotCenterSlot()) );
    grid->addMultiCellWidget(relativeRotCenter, 2, 2, 0, 1);

    QHBox *hbox=new QHBox(parent);
    label = new QLabel(i18n("Angle:"), hbox);

    rotAngle = new KDoubleNumInput(hbox);
    rotAngle->setRange(-360.0, 360.0, 0.1, false);
    horizPosition->setStep (0.1);
    layout->addMultiCellWidget(hbox, 1, 1, 0, 1);
    layout->setRowStretch(2, 1);

    applyBttn[2] = new QPushButton(i18n("Apply"), parent);
    connect (applyBttn[2], SIGNAL(clicked()), this, SLOT(applyPressed()));
    layout->addWidget(applyBttn[2], 3, 0);

    applyToDupBttn[2] = new QPushButton(i18n("Apply To Duplicate"), parent);
    connect (applyToDupBttn[2], SIGNAL(clicked()),
             this, SLOT(applyToDuplicatePressed()));
    layout->addWidget(applyToDupBttn[2], 3, 1);
}

void TransformationDialog::createMirrorWidget (QWidget* parent) {

    QBoxLayout *layout=new QVBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QButtonGroup *group = new QHButtonGroup(parent);
    group->setFrameStyle (QFrame::NoFrame);
    group->setExclusive (true);
    layout->addWidget(group);

    horizMirror = new QPushButton(group);
    horizMirror->setToggleButton(true);
    horizMirror->setPixmap(SmallIcon("hmirror",KIllustratorFactory::global()));

    vertMirror = new QPushButton(group);
    vertMirror->setToggleButton(true);
    vertMirror->setPixmap(SmallIcon("vmirror",KIllustratorFactory::global()));

    group=new QHButtonGroup(parent);
    group->setFrameStyle (QFrame::NoFrame);
    layout->addWidget(group);
    applyBttn[3] = new QPushButton(i18n("Apply"), group);
    connect (applyBttn[3], SIGNAL(clicked()), this, SLOT(applyPressed()));

    applyToDupBttn[3] = new QPushButton(i18n("Apply To Duplicate"), group);
    connect (applyToDupBttn[3], SIGNAL(clicked()),
             this, SLOT(applyToDuplicatePressed()));
    layout->addStretch(1);
}

void TransformationDialog::relativePositionSlot(){
  if( relativePosition->isChecked()){
     horizPosition->setValue(0.0);
     vertPosition-> setValue(0.0);
  }
  else
  {
     Rect r = document->activePage()->boundingBoxForSelection ();
     horizPosition->setValue (r.left ());
     vertPosition->setValue (r.top ());
  }
}

void TransformationDialog::relativeRotCenterSlot(){
  if( relativeRotCenter->isChecked()){
     horizRotCenter->setValue(0.0);
     vertRotCenter-> setValue(0.0);
  }
  else
  {
     Rect r = document->activePage()->boundingBoxForSelection ();
     horizRotCenter->setValue (r.center ().x ());
     vertRotCenter->setValue (r.center ().y ());
  }
}

void TransformationDialog::applyToDuplicatePressed () {
  if (document == 0L || document->activePage()->selectionIsEmpty ())
    return;

  int buttonId = -1;

  for (int i = 0; i < 4; i++)
    if (sender () == applyToDupBttn[i]) {
      buttonId = i;
      break;
    }
  switch (buttonId) {
  case 0:
    translate (true);
    break;
  case 1:
    scale (true);
    break;
  case 2:
    rotate (true);
    break;
  case 3:
    mirror (true);
    break;
  default:
    return;
  }
}

void TransformationDialog::applyPressed () {
  if (document == 0L || document->activePage()->selectionIsEmpty ())
    return;

  int buttonId = -1;

  for (int i = 0; i < 4; i++)
    if (sender () == applyBttn[i]) {
      buttonId = i;
      break;
    }
  switch (buttonId) {
  case 0:
    translate (false);
    break;
  case 1:
    scale (false);
    break;
  case 2:
    rotate (false);
    break;
  case 3:
    mirror (false);
    break;
  default:
    return;
  }
}

void TransformationDialog::translate (bool onDuplicate) {
  float xval, yval;

  xval = horizPosition->getValue ();
  yval = vertPosition->getValue ();

  if (! relativePosition->isChecked ()) {
    // the given values are relative to the current bounding box
    Rect r = document->activePage()->boundingBoxForSelection ();
    xval -= r.left ();
    yval -= r.top ();
  }
  if (onDuplicate) {
    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for(QListIterator<GObject> it(document->activePage()->getSelection()); it.current(); ++it) {
      GObject* obj = (*it)->copy ();
      QWMatrix m;
      m.translate (xval, yval);
      obj->transform (m, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    TranslateCmd* cmd = new TranslateCmd (document, xval, yval);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::scale (bool onDuplicate) {
  float xval, yval;

  xval = horizDim->getValue ();
  yval = vertDim->getValue ();

  if (percent->isChecked ()) {
    // the given values are percentage values
    xval /= 100.0;
    yval /= 100.0;
  }
  else {
    // the values are absolute values, so compute the scaling factors
    Rect r = document->activePage()->boundingBoxForSelection ();
    xval /= r.width ();
    yval /= r.height ();
  }
  if (onDuplicate) {
    Rect box = document->activePage()->boundingBoxForSelection ();
    float xoff = box.x (), yoff = box.y ();

    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (QListIterator<GObject> it(document->activePage()->getSelection()); it.current(); ++it) {
      GObject* obj = (*it)->copy ();
      QWMatrix m1, m2, m3;

      m1.translate (-xoff, -yoff);
      m2.scale (xval, yval);
      m3.translate (xoff, yoff);

      obj->transform (m1);
      obj->transform (m2);
      obj->transform (m3, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    ScaleCmd* cmd = new ScaleCmd (document, Handle_Right | Handle_Bottom,
                                 xval, yval);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::rotate (bool onDuplicate) {
  float xcenter, ycenter, angle;

  xcenter = horizRotCenter->getValue ();
  ycenter = vertRotCenter->getValue ();
  angle = rotAngle->value();
  kdDebug(38000)<<"TransformationDialog::rotate() angle=="<<angle<<endl;

  if (relativeRotCenter->isChecked ()) {
    // the given values are relative to the current bounding box
    Rect r = document->activePage()->boundingBoxForSelection ();
    xcenter += r.left ();
    ycenter += r.top ();
  }
  if (onDuplicate) {
    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (QListIterator<GObject> it(document->activePage()->getSelection()); it.current(); ++it) {
      GObject* obj = (*it)->copy ();
      QWMatrix m1, m2, m3;
      m1.translate (-xcenter, -ycenter);
      m2.rotate (angle);
      m3.translate (xcenter, ycenter);
      obj->transform (m1);
      obj->transform (m2);
      obj->transform (m3, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    RotateCmd* cmd = new RotateCmd (document, Coord (xcenter, ycenter),
                                    angle);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::mirror (bool onDuplicate) {
  float sx = 1, sy = 1;

  if (horizMirror->isOn ())
    sx = -1;
  else if (vertMirror->isOn ())
    sy = -1;
  if (onDuplicate) {
    Rect box = document->activePage()->boundingBoxForSelection ();
    float xoff = box.x (), yoff = box.y ();

    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (QListIterator<GObject> it(document->activePage()->getSelection()); it.current(); ++it) {
      GObject* obj = (*it)->copy ();
      QWMatrix m1, m2, m3;

      m1.translate (-xoff, -yoff);
      m2.scale (sx, sy);
      m3.translate (xoff, yoff);

      obj->transform (m1);
      obj->transform (m2);
      obj->transform (m3, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    ScaleCmd* cmd = new ScaleCmd (document, Handle_Right | Handle_Bottom,
                                  sx, sy);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::setDocument (GDocument* doc) {
  document = doc;
  update ();
}

void TransformationDialog::showTab (int id) {
  show();
  showPage(id);
  raise();
}

void TransformationDialog::update () {
  if (sender () != 0L)
    document = (GDocument *) sender ();

  Rect r = document->activePage()->boundingBoxForSelection ();

  // position
  horizPosition->setValue (r.left ());
  vertPosition->setValue (r.top ());
  relativePosition->setChecked (false);

  // dimension
  absolute->setChecked (true);
  horizDim->setValue (r.width ());
  vertDim->setValue (r.height ());
  dimRatio = r.width () / r.height ();
  selWidth = r.width ();
  selHeight = r.height ();

  // rotation
  rotAngle->setValue (90.0);
  horizRotCenter->setValue (r.center ().x ());
  vertRotCenter->setValue (r.center ().y ());
  relativeRotCenter->setChecked (false);
  setActiveWindow ();
  raise ();
}

void TransformationDialog::updateProportionalDimension (float /*value*/) {
  if (proportional->isChecked ()) {
    if (sender () == horizDim) {
        kdDebug(38000) << "updateProportionalDimension--Horizontal" << endl;
        if (percent->isChecked ()){
            kdDebug(38000) << "updateProportionalDimension--Percental" << endl;
            vertDim->setValue (horizDim->getValue ());}
        else {
            kdDebug(38000) << "updateProportionalDimension--Non-Percental" << endl;
            float h = horizDim->getValue ();
            vertDim->setValue (h / dimRatio);
        }
    }
    else if (sender () == vertDim) {
        kdDebug(38000) << "updateProportionalDimension--vertical" << endl;
        if (percent->isChecked ()){
            kdDebug(38000) << "updateProportionalDimension--percental" << endl;
            horizDim->setValue (vertDim->getValue ());}
        else {
            kdDebug(38000) << "updateProportionalDimension--non-percental" << endl;
            float v = vertDim->getValue ();
            horizDim->setValue (v * dimRatio);
        }
    }
  }
}

void TransformationDialog::slotAbsScale () {
  horizDim->enableUnits (true);
  horizDim->setValue (selWidth);
  vertDim->enableUnits (true);
  vertDim->setValue (selHeight);
}

void TransformationDialog::slotPercentScale () {
  horizDim->enableUnits (false);
  horizDim->setValue (100.0);
  vertDim->enableUnits (false);
  vertDim->setValue (100.0);
}

#include <TransformationDialog.moc>
